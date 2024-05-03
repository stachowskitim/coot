/*
 * density-contour/test-occlusion.cc
 * 
 * Copyright 2020 by Medical Research Council
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


#include <chrono>
#include <clipper/ccp4/ccp4_map_io.h>
#include "coot-utils/coot-map-utils.hh"
#include "occlusion.hh"

#include "density-contour/CIsoSurface.h"
#include "transfer-occlusions.hh"

int main(int argc, char **argv) {

   int status = 0;

   std::string file_name = "test.map";
   if (argc == 2)
      file_name = argv[1];

   try {
      std::cout << "Reading " << file_name << std::endl;

      auto tp_0 = std::chrono::high_resolution_clock::now();
      clipper::CCP4MAPfile file;
      clipper::Xmap<float> xmap;
      file.open_read(file_name);
      file.import_xmap(xmap);

      // now contour the xmap

      auto tp_1 = std::chrono::high_resolution_clock::now();
      CIsoSurface<float> my_isosurface;
      coot::CartesianPairInfo v;
      float contour_level = 0.8;
      float dy_radius = 118.0;
      coot::Cartesian centre(58, 0, 14);
      bool is_em_map = false;
      int isample_step = 1;
      int n_reams = 1;
      int iream_start = 0;

      coot::density_contour_triangles_container_t tri_con;
      tri_con = my_isosurface.GenerateTriangles_from_Xmap(xmap,
                                                          contour_level,
                                                          dy_radius, centre,
                                                          isample_step, iream_start,
                                                          n_reams, is_em_map);
      auto tp_2 = std::chrono::high_resolution_clock::now();

      // tri_con contains points, normals and triangle indices
      std::cout << "Got " << tri_con.points.size() << " points" << std::endl;
      std::cout << "Got " << tri_con.normals.size() << " normals" << std::endl;
      std::cout << "Got " << tri_con.point_indices.size() << " triangles" << std::endl;

      // convert (for now)
      auto tp_3 = std::chrono::high_resolution_clock::now();
      std::vector<coot::augmented_position> positions;
      positions.resize(tri_con.points.size());
      unsigned int n = tri_con.points.size();
      for (unsigned int i=0; i<n; i++) {
         const clipper::Coord_orth &pos  = tri_con.points[i];
         const clipper::Coord_orth &norm = tri_con.normals[i];
         positions[i] = coot::augmented_position(pos, norm);
      }
      auto tp_4 = std::chrono::high_resolution_clock::now();

      // calculate occlusions
      coot::set_occlusions(positions);
      auto tp_5 = std::chrono::high_resolution_clock::now();
      coot::transfer_occlusions(positions, &tri_con);

      // timings
      auto d10 = std::chrono::duration_cast<std::chrono::milliseconds>(tp_1 - tp_0).count();
      auto d21 = std::chrono::duration_cast<std::chrono::milliseconds>(tp_2 - tp_1).count();
      auto d32 = std::chrono::duration_cast<std::chrono::milliseconds>(tp_3 - tp_2).count();
      auto d43 = std::chrono::duration_cast<std::chrono::milliseconds>(tp_4 - tp_3).count();
      auto d54 = std::chrono::duration_cast<std::chrono::milliseconds>(tp_5 - tp_4).count();
      std::cout << "import " << d10 <<  " tri_cons " << d21 << " screen: " << d32
                << " convert " << d43 << " set_occlusions " << d54
                << " ms " << std::endl;

   }
   catch (const clipper::Message_base &exc) {
      std::cout << "WARNING:: failed to open " << file_name << std::endl;
   }



   return status;

}
