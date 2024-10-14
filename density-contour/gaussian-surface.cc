/*
 * density-contour/gaussian-surface.cc
 * 
 * Copyright 2023 by Medical Research Council
 * Author: Paul Emsley
 *
 * This file is part of Coot
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copies of the GNU General Public License and
 * the GNU Lesser General Public License along with this program; if not,
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA, 02110-1301, USA.
 * See http://www.gnu.org/licenses/
 */


#include <functional>
#include <chrono>

#include <clipper/core/nxmap.h>
#include <clipper/core/xmap.h>
#include <clipper/ccp4/ccp4_map_io.h>

#include "gaussian-surface.hh"
#include "coot-utils/coot-coord-utils.hh"
#include "coot-utils/coot-map-utils.hh"
#include "density-contour-triangles.hh"
#include "CIsoSurface.h"
#include "coot-utils/coot-map-utils.hh"

coot::simple_mesh_t
coot::gaussian_surface_t::get_surface() const {

   // std::cout << "debug::get_surface() n-vertices: " << mesh.vertices.size()
   //           << " n-triangles: " << mesh.triangles.size() << std::endl;
   return mesh;
}

void
coot::gaussian_surface_t::using_an_xmap(mmdb::Manager *mol, const std::string &chain_id,
                                        float sigma_in, float contour_level_in, float box_radius_in, float grid_scale,
                                        float fft_b_factor) {

   // these affect the smoothness/resolution of the surface
   //
   // small sigmas mean higher resolution - 0.6 is very high resolution.
   float sigma = 1.9; // neeeds testing
   // 1.9 seems to be too "high resolution"
   sigma = 3.6; // 3.6 contoured at 4.0 is a nice smooth low resolution surface
   sigma = 4.4; // also good.
   float gs = 0.7; // bigger number means more finely sampled grid (bigger numbers are cubic slower)
   float contour_level = 4.0;
   double box_radius = 5.0; // try smaller values - or larger ones for bigger sigma
   float b_factor = fft_b_factor;

   // 20230206-PE use the passed parameters
   sigma = sigma_in;
   contour_level = contour_level_in;
   box_radius = box_radius_in;
   gs = grid_scale;

   auto expo_index_to_float = [] (int i) {
      return static_cast<float>(i) * 0.01;
   };

   auto float_to_expo_index = [] (float &f) {
      if (f < 0.0) f = 0;
      return static_cast<int>(f * 100.0);
   };

   auto mmdb_to_clipper = [] (mmdb::Atom *at) {
      return clipper::Coord_orth(at->x, at->y, at->z);
   };

   auto clipper_to_cart = [] (const clipper::Coord_orth &co) {
      return coot::Cartesian(co.x(), co.y(), co.z());
   };

   auto place_atom_in_grid = [float_to_expo_index, sigma, box_radius] (const clipper::Coord_orth &pt,
                              clipper::Xmap<float> &xmap,
                              const std::vector<float> &expo) {
      // std::cout << "place_atom_in_grid " << pt.format() << std::endl;
      clipper::Coord_frac centre_f = pt.coord_frac(xmap.cell());
      float box_radius_sqrd = box_radius * box_radius;

      clipper::Coord_frac box0(
                               centre_f.u() - box_radius/xmap.cell().descr().a(),
                               centre_f.v() - box_radius/xmap.cell().descr().b(),
                               centre_f.w() - box_radius/xmap.cell().descr().c() );
      clipper::Coord_frac box1(
                               centre_f.u() + box_radius/xmap.cell().descr().a(),
                               centre_f.v() + box_radius/xmap.cell().descr().b(),
                               centre_f.w() + box_radius/xmap.cell().descr().c() );

      clipper::Grid_map grid( box0.coord_grid(xmap.grid_sampling()),
                              box1.coord_grid(xmap.grid_sampling()));

      clipper::Xmap_base::Map_reference_coord ix( xmap, grid.min() ), iu, iv, iw;
      for (iu = ix; iu.coord().u() <= grid.max().u(); iu.next_u() )  {
         for (iv = iu; iv.coord().v() <= grid.max().v(); iv.next_v() ) {
            for (iw = iv; iw.coord().w() <= grid.max().w(); iw.next_w() ) {
               clipper::Coord_grid c_g = iw.coord();
               clipper::Coord_frac c_f = c_g.coord_frac(xmap.grid_sampling());
               clipper::Coord_orth c_o = c_f.coord_orth(xmap.cell());
               // float plength = clipper::Coord_orth::length(c_o, pt);
               float plength_sqrd = (c_o - pt).lengthsq();
               float z_sqrd = plength_sqrd/(sigma*sigma);
               float x_prime = - z_sqrd;
               if (plength_sqrd < box_radius_sqrd) {
                  // int idx = float_to_expo_index(x_prime);
                  // float v = expo[idx];
                  float atomic_number_scaling = 1.0;
                  float v = atomic_number_scaling * expf(x_prime);
                  xmap[iw] += v;
                  if (false)
                     std::cout << "adding " << v << " to " << c_g.format() << " from "
                               << x_prime << " using z " << std::sqrt(z_sqrd) << " x_prime " << x_prime
                               << std::endl;
               }
            }
         }
      }
   };

   int sel_hnd = mol->NewSelection(); // d
   mol->SelectAtoms (sel_hnd, 0, chain_id.c_str(),
                     mmdb::ANY_RES, // starting resno, an int
                     "*", // any insertion code
                     mmdb::ANY_RES, // ending resno
                     "*", // ending insertion code
                     "!HOH", // any residue name
                     "*",
                     "*", // elements
                     "*"  // alt loc.
                     );
   std::pair<clipper::Coord_orth, clipper::Coord_orth> e = util::extents(mol, sel_hnd);
   mmdb::PAtom *atom_selection = NULL;
   int n_selected_atoms;
   mol->GetSelIndex(sel_hnd, atom_selection, n_selected_atoms);

   if (n_selected_atoms == 0)  return;

   if (false)
      std::cout << "debug: chain-id " << chain_id << " extents: "
                << e.first.format() << " "
                << e.second.format() << std::endl;

   mol->DeleteSelection(sel_hnd);
   // extend the extents
   clipper::Coord_orth delta(5,5,5);
   e.first  -= delta;
   e.second += delta;

   int nx = (e.second.x() - e.first.x()) * gs;
   int ny = (e.second.y() - e.first.y()) * gs;
   int nz = (e.second.z() - e.first.z()) * gs;

   const clipper::Coord_orth coords_base = e.first;
   glm::vec3 coords_base_glm(coords_base.x(), coords_base.y(), coords_base.z());
   coot::Cartesian coords_base_cart = clipper_to_cart(coords_base);
   clipper::Coord_orth dimensions = e.second - e.first;

   clipper::Grid grid(nx, ny, nz);
   clipper::Grid_sampling grid_sampling(nx, ny, nz);
   float ninety = clipper::Util::d2rad(90.0);
   clipper::Cell_descr cell_desc(dimensions.x(), dimensions.y(), dimensions.z(), ninety, ninety, ninety);
   clipper::Cell cell(cell_desc);

   if (false)
      std::cout << "debug:: cell and grid sampling: " << cell.format() << " "
                << grid_sampling.format() << "\n";

   clipper::Spacegroup spacegroup(clipper::Spacegroup::P1);
   clipper::Xmap<float> xmap(spacegroup, cell, grid_sampling);

   unsigned int n_expo_values = 10000;
   std::vector<float> expo(n_expo_values);
   for (unsigned int i=0; i<n_expo_values; i++) {
      float x = expo_index_to_float(i);
      expo[i] = exp(x);
   }

   int imod = 1;
   mmdb::Model *model_p = mol->GetModel(imod);
   if (model_p) {
      int n_chains = model_p->GetNumberOfChains();
      for (int ichain=0; ichain<n_chains; ichain++) {
         mmdb::Chain *chain_p = model_p->GetChain(ichain);
         std::string chain_id_this = chain_p->GetChainID();
         if (chain_id_this == chain_id) {
            int n_res = chain_p->GetNumberOfResidues();
            for (int ires=0; ires<n_res; ires++) {
               mmdb::Residue *residue_p = chain_p->GetResidue(ires);
               if (residue_p) {
                  std::string res_name_this = residue_p->GetResName();
                  if (res_name_this == "HOH") continue;
                  int n_atoms = residue_p->GetNumberOfAtoms();
                  for (int iat=0; iat<n_atoms; iat++) {
                     // std::cout << "atom " << iat << std::endl;
                     mmdb::Atom *at = residue_p->GetAtom(iat);
                     if (! at->isTer()) {
                        clipper::Coord_orth pt = mmdb_to_clipper(at);
                        clipper::Coord_orth position_in_grid = pt - coords_base;
                        place_atom_in_grid(position_in_grid, xmap, expo); // change xmap
                     }
                  }
               }
            }
         }
      }
   }

   if (b_factor > 0.0)
      util::sharpen_blur_map(&xmap, b_factor);

   coot::Cartesian centre(10, 10, 10);
   // std::pair<bool, clipper::Coord_orth> cc = coot::centre_of_molecule(mol);
   // if (cc.first)
      // centre = coot::Cartesian(cc.second.x(), cc.second.y(), cc.second.z());
   clipper::Coord_orth e_midpoint = 0.5 * (e.first + e.second);
   centre = clipper_to_cart(e_midpoint);
   centre -= coords_base_cart;
   // std::cout << "--------- Centre of molecule " << centre << " ----------" << std::endl;
   int isample_step = 1;
   int iream_start = 0;
   int n_reams = 1;
   bool is_em_map = true;
   double diag_distance = clipper::Coord_orth::length(e.first, e.second);
   float dy_radius = diag_distance / 2.0;

   bool use_vertex_gradients_for_map_normals_flag = true;
   CIsoSurface<float> my_isosurface;
   coot::density_contour_triangles_container_t tri_con =
        my_isosurface.GenerateTriangles_from_Xmap(std::cref(xmap),
                                                  contour_level, dy_radius, centre, isample_step,
                                                  iream_start, n_reams, is_em_map,
                                                  use_vertex_gradients_for_map_normals_flag);

   if (false)
      std::cout << "tri_con points: " << tri_con.points.size()
                << " n-triangles " << tri_con.point_indices.size() << " "
                << std::endl;

   // transfer tri_con to simple_mesh_t
   mesh.vertices.reserve(tri_con.points.size());
   glm::vec4 col(0.5, 0.5, 0.5, 1.0);
   for (size_t i = 0; i < tri_con.points.size(); i++) {
      const auto &p = tri_con.points[i];
      const auto &n = tri_con.normals[i];
      glm::vec3 pos(p.x(), p.y(), p.z());
      glm::vec3 normal(n.x(), n.y(), n.z());
      coot::api::vnc_vertex vertex(pos + coords_base_glm, normal, col);
      mesh.vertices.push_back(vertex);
   }

   mesh.triangles.reserve(tri_con.point_indices.size());
   for (size_t i = 0; i < tri_con.point_indices.size(); i++) {
      const auto &t = tri_con.point_indices[i];
      g_triangle tri(t.pointID[0], t.pointID[1], t.pointID[2]);
      mesh.triangles.push_back(tri);
   }


   // 20240911-PE do I need this now?
   // I think not.
   // normals_from_function_gradient(xmap, coords_base_glm); // changes the normal in the verties of the mesh
}

void
coot::gaussian_surface_t::normals_from_function_gradient(const clipper::Xmap<float> &xmap,
                                                         const glm::vec3 &coords_base_glm) {

   auto tp_0 = std::chrono::high_resolution_clock::now();
   float delta = 0.92;
   for (unsigned int i=0; i<mesh.vertices.size(); i++) {
      const auto &pos = mesh.vertices[i].pos - coords_base_glm;
      clipper::Coord_orth p_x_1(pos.x - delta, pos.y, pos.z);
      clipper::Coord_orth p_x_2(pos.x + delta, pos.y, pos.z);
      clipper::Coord_orth p_y_1(pos.x, pos.y - delta, pos.z);
      clipper::Coord_orth p_y_2(pos.x, pos.y + delta, pos.z);
      clipper::Coord_orth p_z_1(pos.x, pos.y, pos.z - delta);
      clipper::Coord_orth p_z_2(pos.x, pos.y, pos.z + delta);
      float f_x_1 = util::density_at_point(xmap, p_x_1);
      float f_x_2 = util::density_at_point(xmap, p_x_2);
      float f_y_1 = util::density_at_point(xmap, p_y_1);
      float f_y_2 = util::density_at_point(xmap, p_y_2);
      float f_z_1 = util::density_at_point(xmap, p_z_1);
      float f_z_2 = util::density_at_point(xmap, p_z_2);
      float f = util::density_at_point(xmap, clipper::Coord_orth(pos.x, pos.y, pos.z));
      glm::vec3 grr(f_x_1 - f_x_2, f_y_1 - f_y_2, f_z_1 - f_z_2);
      if (false)
         std::cout << "pos " << pos.x << " " << pos.y << " " << pos.z << " "
                   << "grr " << grr.x << " " << grr.y << " " << grr.z << " "
                   << " f " << f << " "
                   << "fx  " << f_x_1 << " " << f_x_2 << " "
                   << "fy  " << f_y_1 << " " << f_y_2 << " "
                   << "fz  " << f_z_1 << " " << f_z_2 << " "
                   << std::endl;
      if (grr.x != 0.0) {
         if (grr.y != 0.0) {
            if (grr.z != 0.0) {
               glm::vec3 gr = glm::normalize(grr);
               mesh.vertices[i].normal = gr;
            }
         }
      }
   }
   auto tp_1 = std::chrono::high_resolution_clock::now();
   auto d10  = std::chrono::duration_cast<std::chrono::milliseconds>(tp_1 - tp_0).count();

   if (false)
      std::cout << "normals_from_function_gradient(): time " << d10 << " ms " << std::endl;
}

void
coot::gaussian_surface_t::using_calc_density(mmdb::Manager *mol) {

   int atom_selection_handle = mol->NewSelection();
   mol->SelectAtoms (atom_selection_handle, 0, "*",
                     mmdb::ANY_RES, // starting resno, an int
                     "*", // any insertion code
                     mmdb::ANY_RES, // ending resno
                     "*", // ending insertion code
                     "*", // any residue name
                     "*",
                     "*", // elements
                     "*"  // alt loc.
                     );


   mmdb::PAtom *atom_selection = NULL;
   int n_selected_atoms;
   mol->GetSelIndex(atom_selection_handle, atom_selection, n_selected_atoms);
   std::cout << "INFO:: selected " << n_selected_atoms << " atoms" << std::endl;

   clipper::Cell cell(clipper::Cell_descr(100, 100, 100, 90, 90, 90));
   clipper::Spacegroup space_group(clipper::Spacegroup::P1);
   clipper::Grid_sampling grid_sampling(100,100,100);

   clipper::Xmap<float> xmap =
      coot::util::calc_atom_map(mol, atom_selection_handle, cell, space_group, grid_sampling);

   if (true) {
      clipper::CCP4MAPfile outmapfile;
      outmapfile.open_write("atom_calc.map");
      outmapfile.export_xmap(xmap);
      outmapfile.close_write();
   }

   // now how do I contour that?

   float contour_level = 0.5;
   coot::Cartesian centre(50, 50, 50);
   int isample_step = 1;
   int iream_start = 0;
   int n_reams = 100;
   bool is_em_map = true;
   float dy_radius = 50.0;

   bool use_vertex_gradients_for_map_normals_flag = true;
   CIsoSurface<float> my_isosurface;
   coot::density_contour_triangles_container_t tri_con =
        my_isosurface.GenerateTriangles_from_Xmap(std::cref(xmap),
                                                  contour_level, dy_radius, centre, isample_step,
                                                  iream_start, n_reams, is_em_map,
                                                  use_vertex_gradients_for_map_normals_flag);

   std::cout << "tri_con points: " << tri_con.points.size() << " vertices " << tri_con.point_indices.size() << " triangles"
             << std::endl;

   // transfer tri_con to simple_mesh_t
   glm::vec4 col(0.5, 0.5, 0.5, 1.0);
   for (size_t i = 0; i < tri_con.points.size(); i++) {
      const auto &p = tri_con.points[i];
      const auto &n = tri_con.normals[i];
      glm::vec3 pos(p.x(), p.y(), p.z());
      glm::vec3 normal(n.x(), n.y(), n.z());
      coot::api::vnc_vertex vertex(pos, normal, col);
   }


}

// optional args: float sigma=4.4, float contour_level=4.0, float box_radius=5.0, float grid_scale=0.7);
coot::gaussian_surface_t::gaussian_surface_t(mmdb::Manager *mol, const std::string &chain_id,
                                             float sigma, float contour_level, float box_radius, float grid_scale,
                                             float b_factor) {

   // using_calc_density(mol);
   using_an_xmap(mol, chain_id, sigma, contour_level, box_radius, grid_scale, b_factor);

}
