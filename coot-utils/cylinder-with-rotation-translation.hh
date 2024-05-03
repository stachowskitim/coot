/*
 * coot-utils/cylinder-with-rotation-translation.hh
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
 * Lesser General Public License for more details.
 *
 * You should have received a copies of the GNU General Public License and
 * the GNU Lesser General Public License along with this program; if not,
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA, 02110-1301, USA.
 * See http://www.gnu.org/licenses/
 *
 */

#ifndef COOT_CYLINDER_WITH_ROTATION_TRANSLATION_HH
#define COOT_CYLINDER_WITH_ROTATION_TRANSLATION_HH

#include <vector>
#include "vertex.hh"
#include "coot-utils/g_triangle.hh"

// this is not the right way, I think. Perhaps use inheritance?
// But then we have to worry about the layout of the memory...
//
class cylinder_with_rotation_translation {
   void add_flat_cap(int end_type); // 0 for base, 1 for top
   float height;
   float base_radius;
   float top_radius;
   glm::mat3 model_rotation_matrix;
   glm::vec3 model_translation;
   unsigned int n_slices;
   unsigned int n_stacks;
public:
   std::vector<coot::api::vertex_with_rotation_translation> vertices;
   std::vector<g_triangle> triangle_indices_vec;
   cylinder_with_rotation_translation() {} // needs to be filled
   cylinder_with_rotation_translation(const std::pair<glm::vec3, glm::vec3> &cart_pair,
                                      float base_radius, float top_radius, float height,
                                      unsigned int n_slices, unsigned int n_stacks);
   void add_flat_end_cap();
   void add_flat_start_cap();
   void add_spiral();
};

#endif
