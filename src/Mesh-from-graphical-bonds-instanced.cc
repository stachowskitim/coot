/*
 * src/Mesh-from-graphical-bonds-instanced.cc
 *
 * Copyright 2021 by Medical Research Council
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
 * Lesser General Public License for more details.
 *
 * You should have received a copies of the GNU General Public License and
 * the GNU Lesser General Public License along with this program; if not,
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA, 02110-1301, USA.
 * See http://www.gnu.org/licenses/
 *
 */
//
#include <vector>
#include <iostream>
#include <map>
#include <set>

// Having this up here...
#include <mmdb2/mmdb_manager.h>
// Fixes this:
// In file included from /usr/include/X11/Xlib.h:44,
//                 from /usr/include/epoxy/glx.h:36,
//                 from ../../coot/src/Mesh.cc:9:
///home/paule/autobuild/Linux-penelope-pre-release-gtk3-python/include/mmdb2/mmdb_io_file.h:131:22: error: expected unqualified-id before numeric constant
//  131 |         inline bool  Success   () { return IOSuccess; }
//      |                      ^~~~~~~

#include <epoxy/gl.h>

// #ifndef __APPLE__
// #include <epoxy/glx.h>
// #endif

#include "Mesh.hh"
#include "Shader.hh"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>  // to_string()
#include <glm/gtx/rotate_vector.hpp>
// #include <glm/ext.hpp>

#include "coot-utils/cylinder.hh"
#include "coot-utils/oct.hh"

#include "coords/Bond_lines.h"

#include "bond-colour-mode.hh"

// Get rid of this at some stage - needs thinking about meshed objects
#include "old-generic-display-object.hh"

// make space
void
Mesh::setup_instancing_buffer_data(const Material &mat,
                                   const std::vector<glm::mat4> &instanced_matrices,
                                   const std::vector<glm::vec4> &instanced_colours) {

   is_instanced = true;
   is_instanced_colours = true;
   is_instanced_with_rts_matrix = true;
   material = mat;

   if (vao == VAO_NOT_SET)
      std::cout << "ERROR:: Mesh::setup_instancing_buffer_data() You forgot to setup this Mesh "
                << "\"" << name << "\" - or perhaps the Mesh had no vertices?" << std::endl;
   glBindVertexArray(vao);

   // 0 vertex position
   // 1 vertex normal
   // 2 vertex colour
   // 3 instance colour
   // 4 instance rot-trans-1  // these include the scale
   // 5 instance rot-trans-2
   // 6 instance rot-trans-3
   // 6 instance rot-trans-4

   if (false)
      std::cout << "::::::::::::: debug:: setup_instancing_buffer_data() calls setup_matrix_and_colour_instancing_buffers_standard"
                << std::endl;

   setup_matrix_and_colour_instancing_buffers_standard(instanced_matrices, instanced_colours);

}



void
Mesh::make_graphical_bonds_spherical_atoms_instanced_version(Shader *shader_p,
                                                             const Material &material,
                                                             const graphical_bonds_container &gbc,
                                                             int udd_handle_bonded_type,
                                                             float atom_radius,
                                                             float bond_radius,
                                                             unsigned int num_subdivisions,
                                                             const std::vector<glm::vec4> &colour_table) {

   // udd_handle_bonded_type can be NO_BOND, BONDED_WITH_STANDARD_ATOM_BOND, BONDED_WITH_BOND_TO_HYDROGEN
   // BONDED_WITH_HETATM_BOND.

   GLenum err = glGetError();
   if (err) std::cout << "error make_graphical_bonds_spherical_atoms() --start-- error "
                      << err << std::endl;

   bool atoms_have_bigger_radius_than_bonds = false;
   if (atom_radius > bond_radius)
      atoms_have_bigger_radius_than_bonds = true;

   // ----------------------- setup the vertices and triangles ----------------------

   std::pair<std::vector<glm::vec3>, std::vector<g_triangle> > octaphere_geom =
      tessellate_octasphere(num_subdivisions);

   std::vector<s_generic_vertex> local_vertices(octaphere_geom.first.size());
   for (unsigned int i=0; i<octaphere_geom.first.size(); i++) {
      const glm::vec3 &v(octaphere_geom.first[i]);
      glm::vec4 col = glm::vec4(0.5, 0.3, 0.7, 1.0);
      local_vertices[i] = s_generic_vertex(v, v, col);
   }

   const std::vector<g_triangle> &triangles = octaphere_geom.second;
   // is_instanced should be false here, so that the vertex colours go in their slot (20230813-PE that seems weird)
   is_instanced = false;
   err = glGetError();
   if (err) std::cout << "error make_graphical_bonds_spherical_atoms() pre-import error "
                      << err << std::endl;
   import(local_vertices, triangles);
   err = glGetError();
   if (err) std::cout << "error make_graphical_bonds_spherical_atoms() post-import error "
                      << err << std::endl;
   setup_buffers(); // gen the VAO
   err = glGetError();
   if (err) std::cout << "error make_graphical_bonds_spherical_atoms() post-setup_buffers() error "
                      << err << std::endl;

   // ----------------------- setup the instances ----------------------

   std::vector<glm::mat4> instanced_matrices;
   std::vector<glm::vec4> instanced_colours;

   // So here we want to distinguish between atoms that have no bonds
   // (say waters and metals) and those that have bonds.  And that is
   // because sphere have full octasphere vertices and atoms with bonds
   // have hemisphere vertices

   // which atoms have no bonds? They need spheres.

   glBindVertexArray(vao); // setup_buffers() unbinds the vao
   glm::mat4 unit(1.0);
   for (int icol=0; icol<gbc.n_consolidated_atom_centres; icol++) {
      const glm::vec4 &col = colour_table[icol];
      for (unsigned int i=0; i<gbc.consolidated_atom_centres[icol].num_points; i++) {
         const graphical_bonds_atom_info_t &at_info = gbc.consolidated_atom_centres[icol].points[i];

         mmdb::Atom *at = at_info.atom_p;
         bool do_it = true; // turn this off is there is only one bond to this atom - the graphical
                            // bonds container gbc should "know" that - we don't try to sort it out
                            // here.

#if 0
         bool do_it = atoms_have_bigger_radius_than_bonds;
         if (! do_it) {
            if (at) {
               int state = -1;
               at->GetUDData(udd_handle_bonded_type, state);
               if (state == graphical_bonds_container::NO_BOND) {
                  do_it = true;
               }
            }
         }
#endif

         if (do_it) {
            float scale = 1.0;
            if (at_info.is_hydrogen_atom) scale *= 0.5;
            glm::vec3 t(at->x, at->y, at->z);
            float sar = scale * atom_radius;
            glm::vec3 sc(sar, sar, sar);
            glm::mat4 mm = glm::scale(unit, sc);
            instanced_matrices.push_back(glm::translate(mm, t/sar));
            instanced_colours.push_back(col);
         }
      }
   }

   err = glGetError();
   if (err) std::cout << "error make_graphical_bonds_spherical_atoms() pre setup_instancing_buffer_data() error "
                      << err << std::endl;
   // make space and transfer
   // std::cout << "calling setup_instancing_buffer_data with instanced_matrices size " << instanced_matrices.size() << std::endl;
   setup_instancing_buffer_data(material, instanced_matrices, instanced_colours);
   err = glGetError();
   if (err) std::cout << "error make_graphical_bonds_spherical_atoms() post setup_instancing_buffer_data() error "
                      << err << std::endl;

   // transfer to the graphics_card: (not needed already done by the previous setup)
   // update_instancing_buffer_data(instanced_matrices, instanced_colours);
   // err = glGetError();
   // if (err) std::cout << "error make_graphical_bonds_spherical_atoms() post update_instancing_buffer_data() error "
   // << err << std::endl;

}

void
Mesh::make_graphical_bonds_hemispherical_atoms_instanced_version(Shader *shader_p,
                                                                 const Material &material,
                                                                 const graphical_bonds_container &gbc, int udd_handle_bonded_type,
                                                                 float atom_radius,
                                                                 float bond_radius,
                                                                 unsigned int num_subdivisions,
                                                                 const std::vector<glm::vec4> &colour_table) {

   // this is the wrong approach to do hemisphere - make all the atoms spheres for now.
   //
   // Later, make the bonds have rounded end caps where needed (and in that case, a separate spherical atom is not needed).

   return;

#if 0
   // udd_handle_bonded_type can be NO_BOND, BONDED_WITH_STANDARD_ATOM_BOND, BONDED_WITH_BOND_TO_HYDROGEN
   // BONDED_WITH_HETATM_BOND.

   // do these need to be passed to get_glm_colour_for_bonds()?
   bool is_intermediate_atoms_molecule = false;  // pass this
   int bonds_box_type = coot::NORMAL_BONDS; // pass this (or put it into thg gbc)

   bool atoms_have_bigger_radius_than_bonds = false;
   if (atom_radius > bond_radius)
      atoms_have_bigger_radius_than_bonds = true;

   // like above, different z axis because we want the hemisphere to extend outside the cylinder - and we don't need to
   // scale to bond length
   auto get_octahemi_matrix = [] (const glm::vec3 &pos_1, const glm::vec3 &pos_2, float radius) {
                                 glm::vec3 delta = pos_2 - pos_1;
                                 glm::mat4 u(1.0f);
                                 glm::mat4 sc = glm::scale(u, glm::vec3(radius, radius, radius));
                                 // orient
                                 glm::vec3 normalized_bond_orientation(glm::normalize(delta));
                                 glm::mat4 ori = glm::orientation(normalized_bond_orientation, glm::vec3(0.0, 0.0, -1.0));
                                 // translate
                                 glm::mat4 t = glm::translate(u, pos_1);
                                 glm::mat4 m = t * ori * sc;
                                 return m;
                              };

   // ----------------------- setup the vertices and triangles ----------------------

   std::pair<std::vector<glm::vec3>, std::vector<g_triangle> > octaphere_geom =
      tessellate_hemisphere_patch(num_subdivisions);

   std::vector<s_generic_vertex> local_vertices(octaphere_geom.first.size());
   for (unsigned int i=0; i<octaphere_geom.first.size(); i++) {
      const glm::vec3 &v(octaphere_geom.first[i]);
      glm::vec4 col = glm::vec4(0.5, 0.3, 0.7, 1.0);
      local_vertices[i] = s_generic_vertex(v, v, col);
   }

   const std::vector<g_triangle> &triangles = octaphere_geom.second;
   is_instanced = false;
   import(local_vertices, triangles);
   setup_buffers(); // gen the VAO
   GLenum err = glGetError();
   if (err) std::cout << "error make_graphical_bonds_hemispherical_atoms() post-setup_buffers() error "
                      << err << std::endl;

   // ----------------------- setup the instances ----------------------

   std::vector<glm::mat4> instanced_matrices;
   std::vector<glm::vec4> instanced_colours;

   // what is the other atom of the bond to this atom (if it has one). Just find the first one
   // if there are many
   //
   // this is hideous. And will be slow on a big molecule. Replace it.
   // When the bonds are calculated, the at_info should store
   // a pointer to a bonded atom (mmdb::Atom *bonded_atom_other_atom)
   //
   std::map<int, int> bonded_atom_other_atom;
   std::map<int, mmdb::Atom *> index_to_atom;
   for (int icol_bond=0; icol_bond<gbc.num_colours; icol_bond++) {
      graphical_bonds_lines_list<graphics_line_t> &ll = gbc.bonds_[icol_bond];
      for (int j=0; j<ll.num_lines; j++) {
         int idx_1 = ll.pair_list[j].atom_index_1;
         int idx_2 = ll.pair_list[j].atom_index_2;
         std::map<int, int>::const_iterator it;
         it = bonded_atom_other_atom.find(idx_1);
         if (it == bonded_atom_other_atom.end())
            bonded_atom_other_atom[idx_1] = idx_2;
         it = bonded_atom_other_atom.find(idx_2);
         if (it == bonded_atom_other_atom.end())
            bonded_atom_other_atom[idx_2] = idx_1;
      }
   }
   for (int icol=0; icol<gbc.n_consolidated_atom_centres; icol++) {
      for (unsigned int i=0; i<gbc.consolidated_atom_centres[icol].num_points; i++) {
         const graphical_bonds_atom_info_t &at_info = gbc.consolidated_atom_centres[icol].points[i];
         index_to_atom[at_info.atom_index] = at_info.atom_p;
      }
   }

   glBindVertexArray(vao); // setup_buffers() unbinds the vao

   glm::mat4 unit(1.0);
   for (int icol=0; icol<gbc.n_consolidated_atom_centres; icol++) {
      const glm::vec4 &col = colour_table[icol];
      for (unsigned int i=0; i<gbc.consolidated_atom_centres[icol].num_points; i++) {
         const graphical_bonds_atom_info_t &at_info = gbc.consolidated_atom_centres[icol].points[i];
         bool do_it = true;
         if (atoms_have_bigger_radius_than_bonds) do_it = false; // not as hemispheres, that is
         mmdb::Atom *at = at_info.atom_p;

         if (do_it) {
            if (at) {
               int state = -1;
               at->GetUDData(udd_handle_bonded_type, state);
               if (state == graphical_bonds_container::NO_BOND) {
                  do_it = false;
               }
            }
         }

         // Oh dear! I need to know where the atom at the other end of the bond is!
         // Which bond? Any bond. The first one. We need to do some pre-processing to know that.
         // That's done now.

         // There is an opportunity for missing atoms here. Things that are not NO_BOND but have not correctly
         // set the atom_index

         if (do_it) {

            float scale = 1.0;
            if (at_info.is_hydrogen_atom) scale *= 0.5;
            glm::vec3 t(at->x, at->y, at->z);
            float sar = scale * atom_radius;
            glm::vec3 sc(sar, sar, sar);
            instanced_matrices.push_back(glm::translate(glm::scale(unit, sc), t/sar));
            instanced_colours.push_back(col);

            int atom_index = at_info.atom_index;
            std::map<int, int>::const_iterator it = bonded_atom_other_atom.find(atom_index);
            if (it != bonded_atom_other_atom.end()) {
               // check the index before using it?
               int other_atom_index = it->second;
               mmdb::Atom *other_at = index_to_atom[other_atom_index];
               glm::vec3 other_atom_pos(other_at->x, other_at->y, other_at->z);
               glm::mat4 m = get_octahemi_matrix(t, other_atom_pos, bond_radius);
               instanced_matrices.push_back(m);
               instanced_colours.push_back(col);
            } else {
               std::cout << "ERROR:: oops in make_graphical_bonds_hemispherical_atoms() "<< atom_index << std::endl;
            }
         }
      }
   }

   err = glGetError();
   if (err) std::cout << "error make_graphical_bonds_hemispherical_atoms() pre setup_instancing_buffer_data() error "
                      << err << std::endl;
   // make space and transfer
   setup_instancing_buffer_data(shader_p, material, instanced_matrices, instanced_colours);
   err = glGetError();
   if (err) std::cout << "error make_graphical_bonds_hemispherical_atoms() post setup_instancing_buffer_data() error "
                      << err << std::endl;
#endif
}


void
Mesh::make_graphical_bonds_bonds_instanced_version(Shader *shader_p,
                                                   const Material &material,
                                                   const graphical_bonds_container &gbc,
                                                   float bond_radius,
                                                   unsigned int n_slices,
                                                   unsigned int n_stacks,
                                                   const std::vector<glm::vec4> &colour_table,
                                                   bool add_start_end_cap, bool add_end_end_cap) {

   bool is_intermediate_atoms_molecule = false;  // pass this (but why do I need it?)

   // unsigned int n_slices = 8;
   // unsigned int n_stacks = 2; // try 1 later.

   // do we need 4 version of this for the end caps? None, left only, right only and both.

   auto get_bond_matrix = [] (const glm::vec3 &pos_1, const glm::vec3 &pos_2, float radius) {
                             glm::vec3 delta = pos_2 - pos_1;
                             float l_delta = glm::distance(pos_2, pos_1);
                             glm::mat4 u(1.0f);
                             glm::mat4 sc = glm::scale(u, glm::vec3(radius, radius, l_delta));
                             // orient
                             glm::vec3 normalized_bond_orientation(glm::normalize(delta));
                             // 20230812-PE I added a minus sign here, because in the graphics, the bonds went
                             // the wrong way. I am using the same shaded as is used for the instanced atom contact display objects
                             glm::mat4 ori = glm::orientation(-normalized_bond_orientation, glm::vec3(0.0, 0.0, 1.0)); // nice
                             // translate
                             glm::mat4 t = glm::translate(u, pos_1);
                             glm::mat4 m = t * ori * sc;
                             return m;
                          };


   auto vnc_vertex_to_generic_vertex = [] (const coot::api::vnc_vertex &v) {
      return s_generic_vertex(v.pos, v.normal, v.color);
   };

   auto vnc_vertex_vector_to_generic_vertex_vector = [vnc_vertex_to_generic_vertex] (const std::vector<coot::api::vnc_vertex> &vv) {
      std::vector<s_generic_vertex> vo(vv.size());
      for (unsigned int i=0; i<vv.size(); i++)
         vo[i] = vnc_vertex_to_generic_vertex(vv[i]);
      return vo;
   };

   // ----------------------- setup the vertices and triangles ----------------------

   std::pair<glm::vec3, glm::vec3> pp(glm::vec3(0,0,0), glm::vec3(0,0,1));
   cylinder c(pp, 1.0, 1.0, 1.0, n_slices, n_stacks);
   if (add_start_end_cap) c.add_flat_start_cap();
   if (add_end_end_cap)   c.add_flat_end_cap();

   is_instanced = false;
   std::vector<s_generic_vertex> converted_vertices = vnc_vertex_vector_to_generic_vertex_vector(c.vertices);
   import(converted_vertices, c.triangles);

   if (false) // looks fine
      for (unsigned int i=0; i<c.vertices.size(); i++)
         std::cout << "debug vertices " << glm::to_string(c.vertices[i].pos) << std::endl;

   GLenum err = glGetError();
   if (err) std::cout << "error make_graphical_bonds_spherical_atoms() post-import error "
                      << err << std::endl;
   setup_buffers(); // gen the VAO
   err = glGetError();
   if (err) std::cout << "error make_graphical_bonds_spherical_atoms() post-setup_buffers() error "
                      << err << std::endl;


   // ----------------------- setup the instances ----------------------

   std::vector<glm::mat4> instanced_matrices;
   std::vector<glm::vec4> instanced_colours;

   for (int icol=0; icol<gbc.num_colours; icol++) {
      glm::vec4 col = colour_table[icol];
      // std::cout << "make_graphical_bonds_bonds_instanced_version() icol " << icol << " " << glm::to_string(col) << std::endl;
      graphical_bonds_lines_list<graphics_line_t> &ll = gbc.bonds_[icol];
      for (int j=0; j<ll.num_lines; j++) {

         const auto &gl = ll.pair_list[j];

         // does the capiness of the line match the cappiness of the mesh?
         // only add to this mesh, if that is the case:
         if (gl.has_begin_cap == add_start_end_cap && gl.has_end_cap == add_end_end_cap) {
            const coot::Cartesian &start  = gl.positions.getStart();
            const coot::Cartesian &finish = gl.positions.getFinish();
            glm::vec3 pos_1(start.x(),   start.y(),  start.z());
            glm::vec3 pos_2(finish.x(), finish.y(), finish.z());
            glm::mat4 ori = get_bond_matrix(pos_1, pos_2, bond_radius); // apply bond length and width
            float scale = 1.0;
            if (ll.thin_lines_flag) scale *= 0.5;
            if (ll.pair_list[j].cylinder_class == graphics_line_t::KEK_DOUBLE_BOND_INNER_BOND)
               scale *= 0.7;
            glm::vec3 sc(scale, scale, 1.0); // now adjust the width (radius) if needed
            glm::mat4 ori_sc = glm::scale(ori, sc);
            instanced_matrices.push_back(ori_sc);
            instanced_colours.push_back(col);
         }
      }
   }

   glBindVertexArray(vao); // setup_buffers() unbinds the vao
   setup_instancing_buffer_data(material, instanced_matrices, instanced_colours);
   err = glGetError();
   if (err) std::cout << "error make_graphical_bonds_spherical_atoms() post setup_instancing_buffer_data() error "
                      << err << std::endl;

}

// this is not instanced and should not be in this file
void
Mesh::make_symmetry_atoms_bond_lines(const std::vector<std::pair<graphical_bonds_container, std::pair<symm_trans_t, Cell_Translation> > > &symm_boxes,
                                     const glm::vec4 &symmetry_colour, double symmetry_colour_weight) {

   // symmetry_colour is the user-define colour  from the selector

   // this function doesn't use setup_buffers() because all it is making is unindexed lines

   auto get_colour = [] (int icol, int isymop, const glm::vec4 &symmetry_colour, double symmetry_colour_weight) {
      glm::vec4 c(0.5, 0.5, 0.5, 1.0);
      if (icol == 1) { c.x = 0.7; c.y = 0.7; }
      if (icol == 2) { c.x = 0.8; c.y = 0.4; c.z = 0.4; }
      if (icol == 3) { c.x = 0.4; c.y = 0.4; c.z = 0.8; }
      glm::vec4 d = symmetry_colour;
      float a = symmetry_colour_weight;
      float b = 1.0 - a;
      glm::vec4 r = b * c + a * d;
      return r;
   };

   if (first_time) {
      glGenVertexArrays(1, &vao);
      // std::cout << "make_symmetry_atoms_bond_lines() created vao " << vao << std::endl;
   } else {
      // std::cout << "make_symmetry_atoms_bond_lines() using pre-existing vao " << vao << std::endl;
   }
   glBindVertexArray(vao);

   // first count the number of lines, so that we can size the buffer
   unsigned int n_lines = 0;
   for (unsigned int i=0; i<symm_boxes.size(); i++) {
      const std::pair<graphical_bonds_container, std::pair<symm_trans_t, Cell_Translation> > &s_box = symm_boxes[i];
      const graphical_bonds_container &gbc = s_box.first;
      if (gbc.symmetry_has_been_created == 1) {
         for (int icol=0; icol<gbc.num_colours; icol++) {
            graphical_bonds_lines_list<graphics_line_t> &ll = gbc.symmetry_bonds_[icol];
            n_lines += ll.num_lines;
         }
      }
   }

   n_symmetry_atom_lines_vertices = n_lines * 2;
   std::vector<symmetry_atoms_line_vertex> vertices_for_symmetry_atoms_lines(n_symmetry_atom_lines_vertices);

   unsigned int icount = 0;
   for (unsigned int i=0; i<symm_boxes.size(); i++) {
      const std::pair<graphical_bonds_container, std::pair<symm_trans_t, Cell_Translation> > &s_box = symm_boxes[i];
      const graphical_bonds_container &gbc = s_box.first;
      int isymop = s_box.second.first.isym();
      if (gbc.symmetry_has_been_created == 1) {
         for (int icol=0; icol<gbc.num_colours; icol++) {
            // set the colour using icol and isymop
            glm::vec4 col = get_colour(icol, isymop, symmetry_colour, symmetry_colour_weight);
            graphical_bonds_lines_list<graphics_line_t> &ll = gbc.symmetry_bonds_[icol];
            for (int j=0; j<ll.num_lines; j++) {
               glm::vec3 pos_1(ll.pair_list[j].positions.getStart().get_x(),
                               ll.pair_list[j].positions.getStart().get_y(),
                               ll.pair_list[j].positions.getStart().get_z());
               glm::vec3 pos_2(ll.pair_list[j].positions.getFinish().get_x(),
                               ll.pair_list[j].positions.getFinish().get_y(),
                               ll.pair_list[j].positions.getFinish().get_z());
               vertices_for_symmetry_atoms_lines[icount  ].pos = pos_1;
               vertices_for_symmetry_atoms_lines[icount+1].pos = pos_2;
               vertices_for_symmetry_atoms_lines[icount  ].colour = col;
               vertices_for_symmetry_atoms_lines[icount+1].colour = col;
               icount += 2;
            }
         }
      }
   }

   if (first_time) {
   } else {
      glDeleteBuffers(1, &buffer_id);
   }
   unsigned int buffer_size = n_lines * 2 * sizeof(symmetry_atoms_line_vertex);
   glGenBuffers(1, &buffer_id);
   glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
   glBufferData(GL_ARRAY_BUFFER, buffer_size, &(vertices_for_symmetry_atoms_lines[0]), GL_STATIC_DRAW);

   glEnableVertexAttribArray(0); // position
   glEnableVertexAttribArray(1); // colour

   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(symmetry_atoms_line_vertex), 0);
   glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(symmetry_atoms_line_vertex), reinterpret_cast<void *>(sizeof(glm::vec3)));

   // glDisableVertexAttribArray(0);
   // glDisableVertexAttribArray(1);

   glBindVertexArray(0); // we don't need to disable the vertexattribarray if we unbind the vertex array
   GLenum err = glGetError();
   if (err) std::cout << "error Mesh::make_symmetry_atoms_bond_lines() check-before-return error "
                      << err << std::endl;

   first_time = false;

}
