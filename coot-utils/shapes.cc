/*
 * coot-utils/shapes.cc
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

#include "shapes.hh"
#include "cylinder.hh"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>  // to_string()
#include <glm/gtx/rotate_vector.hpp>

coot::simple_mesh_t
coot::cone_mesh(shapes::cone_t cone) {

   auto coord_orth_to_glm = [] (const clipper::Coord_orth &co) {
                               return glm::vec3(co.x(), co.y(), co.z());
                            };

   std::pair<glm::vec3, glm::vec3 > start_end(coord_orth_to_glm(cone.start_point),
                                              coord_orth_to_glm(cone.end_point));
   float h = glm::distance(start_end.first, start_end.second);
   // the cone radii may need switching 50/50.
   glm::vec4 base_colour(cone.col.red, cone.col.green, cone.col.blue, 1.0f);
   unsigned int n_slices = 32; // nice and smooth, with 16 we can just see the ngon.
   cylinder c(start_end, cone.radius, 0.0f, h, base_colour, n_slices, 2);
   c.add_flat_end_cap();
   simple_mesh_t mesh(c.vertices, c.triangles);
   return mesh;

}
   
coot::simple_mesh_t
coot::arrow_mesh(shapes::arrow_t arrow) {

   auto coord_orth_to_glm = [] (const clipper::Coord_orth &co) {
                               return glm::vec3(co.x(), co.y(), co.z());
                            };

   std::pair<glm::vec3, glm::vec3 > start_end(coord_orth_to_glm(arrow.start_point),
                                              coord_orth_to_glm(arrow.end_point));

   float h = glm::distance(start_end.first, start_end.second);
   glm::vec4 base_colour(arrow.col.red, arrow.col.green, arrow.col.blue, 1.0f);
   unsigned int n_slices = 8;
   cylinder c(start_end, arrow.radius, arrow.radius, h, base_colour, n_slices, 2);
   c.add_flat_start_cap();

   clipper::Coord_orth delta_uv = arrow.end_point - arrow.start_point;
   clipper::Coord_orth cone_start = arrow.end_point + 1.3 * delta_uv;
   clipper::Coord_orth cone_end   = arrow.end_point;
   shapes::cone_t cone(cone_start, cone_end, arrow.cone_radius);
   simple_mesh_t cm = cone_mesh(cone);

   simple_mesh_t mesh(c.vertices, c.triangles);
   mesh.add_submesh(cm);
   return mesh;
}

coot::simple_mesh_t
coot::torus_mesh(shapes::torus_t torus) {

   auto colour_holder_to_glm = [] (const coot::colour_holder &ch) {
      return glm::vec4(ch.red, ch.green, ch.blue, 1.0f);
   };

   // phi goes around the ring viewed from the top (down the z axis)
   // theta goes around eaach segment

   // torus.radius_1 is R
   // torus.radius_2 is r

   // x = (R + r * cos(theta)) * cos(phi);
   // y = (R + r * cos(theta)) * sin(phi);
   // z = r * sin(theta)
   //
   // the ring around the middle of the doughnut:
   // x_ring = R * cos(phi)
   // y_ring = R * sin(phi)
   // z_ring = 0;

   const unsigned int n_theta_steps = 60;
   const unsigned int n_phi_steps = 60;
   std::vector<api::vnc_vertex> vertices(n_theta_steps * n_phi_steps);
   std::vector<g_triangle> triangles;
   const float R = torus.radius_1;
   const float r = torus.radius_2;
   const float pi = 3.1415926535;
   glm::vec4 col = colour_holder_to_glm(torus.col);

   glm::vec4 centre(torus.position.x(), torus.position.y(), torus.position.z(), 1.0);
   glm::vec3 ring_normal(torus.normal.x(), torus.normal.y(), torus.normal.z());

   // ring_normal = glm::vec3(0,0,1);

   glm::mat4 ori = glm::orientation(ring_normal, glm::vec3(0,0,1));

   for (unsigned int ip=0; ip<n_phi_steps; ip++) {
      float phi = 2.0f * pi * static_cast<float>(ip)/static_cast<float>(n_phi_steps);
      for (unsigned int it=0; it<n_theta_steps; it++) {
         float theta = 2.0f * pi * static_cast<float>(it)/static_cast<float>(n_theta_steps);
         api::vnc_vertex v;
         glm::vec4 pos;
         pos.x = (R + r * cosf(theta)) * cosf(phi);
         pos.y = (R + r * cosf(theta)) * sinf(phi);
         pos.z = torus.height_scale * r * sinf(theta);
         pos.w = 1.0; // or 0?
         glm::mat4 tori = glm::transpose(ori);
         v.pos = glm::vec3(pos * tori) + glm::vec3(centre);
         glm::vec4 normal;
         normal.x = cosf(theta) * cosf(phi);
         normal.y = cosf(theta) * sinf(phi);
         normal.z = sinf(theta);
         normal.w = 1.0;
         v.normal = glm::vec3(normal * tori);
         v.color = col;
         vertices[ip * n_theta_steps + it] = v;
      }
   }

   // carefully, carefully :-)
   for (unsigned int ip=0; ip<n_phi_steps; ip++) {
      unsigned int ip_this = ip;
      unsigned int ip_next = ip + 1;
      if (ip_next == n_phi_steps) ip_next = 0;
      unsigned int idx_base_phi_00 = ip_this * n_theta_steps;
      unsigned int idx_base_phi_10 = ip_next * n_theta_steps;
      for (unsigned int it=0; it<n_theta_steps; it++) {
         unsigned int it_this = it;
         unsigned int it_next = it + 1;
         if (it_next == n_theta_steps) it_next = 0;

         unsigned int idx_00 = idx_base_phi_00 + it_this;
         unsigned int idx_01 = idx_base_phi_00 + it_next;
         unsigned int idx_10 = idx_base_phi_10 + it_this;
         unsigned int idx_11 = idx_base_phi_10 + it_next;

         g_triangle t1(idx_00, idx_10, idx_11);
         g_triangle t2(idx_00, idx_11, idx_01);
         triangles.push_back(t1);
         triangles.push_back(t2);
      }
   }

   return simple_mesh_t(vertices, triangles);

}

coot::simple_mesh_t
coot::arc_mesh(const shapes::arc_t arc) {

   auto colour_holder_to_glm = [] (const coot::colour_holder &ch) {
      return glm::vec4(ch.red, ch.green, ch.blue, 1.0f);
   };

   auto coord_orth_to_glm = [] (const clipper::Coord_orth &co) {
                               return glm::vec3(co.x(), co.y(), co.z());
                            };

   const unsigned int n_phi_steps = 60;
   const unsigned int n_theta_steps = 60;
   std::vector<api::vnc_vertex> vertices((n_theta_steps + 1) * n_phi_steps); // 20211007-PE this doesn't look right!
                                                                             // c.f. add_dashed_angle_markup()
   std::vector<g_triangle> triangles;
   const float R = arc.radius;
   const float r = arc.radius_inner;
   const float pi = 3.1415926535;

   float angle_delta_deg = arc.delta_angle;
   float angle_delta = angle_delta_deg * pi / 180.0f;
   glm::vec3 start_point = coord_orth_to_glm(arc.start_point); // this is the central atom position
   glm::vec4 col = colour_holder_to_glm(arc.col);
   glm::mat4 rot_mat = glm::orientation(coord_orth_to_glm(arc.normal), glm::vec3(0.0, 0.0, 1.0));
   std::cout << "rot_mat: " << glm::to_string(rot_mat)<< std::endl;

   const clipper::Mat33<double> &m = arc.orientation_matrix;
   glm::mat3 ori_mat(m(0,0), m(0,1), m(0,2),
                     m(1,0), m(1,1), m(1,2),
                     m(2,0), m(2,1), m(2,2));

   for (unsigned int ip=0; ip<=n_phi_steps; ip++) {
      float phi_raw = angle_delta * static_cast<float>(ip)/static_cast<float>(n_phi_steps);
      float phi = phi_raw;
      for (unsigned int it=0; it<n_theta_steps; it++) {
         float theta = 2.0f * pi * static_cast<float>(it)/static_cast<float>(n_theta_steps);
         api::vnc_vertex v;
         v.pos.x = (R + r * cosf(theta)) * cosf(phi);
         v.pos.y = (R + r * cosf(theta)) * sinf(phi);
         v.pos.z = r * sinf(theta);
         v.normal.x = cosf(theta) * cosf(phi);
         v.normal.y = cosf(theta) * sinf(phi);
         v.normal.z = sinf(theta);
         // now move orient the fragment and normal
         // v.pos    = glm::vec3(rot_mat * glm::vec4(v.pos,    1.0f));
         // v.normal = glm::vec3(rot_mat * glm::vec4(v.normal, 1.0f));;
         v.pos    = ori_mat * v.pos;
         v.normal = ori_mat * v.normal;
         v.pos += start_point; // central atom position
         v.color = col;
         unsigned int vertex_idx = ip * n_theta_steps + it;
         vertices[vertex_idx] = v;
      }
   }

   // carefully, carefully :-)
   for (unsigned int ip=0; ip<n_phi_steps; ip++) {
      unsigned int ip_this = ip;
      unsigned int ip_next = ip + 1;
      unsigned int idx_base_phi_00 = ip_this * n_theta_steps;
      unsigned int idx_base_phi_10 = ip_next * n_theta_steps;
      for (unsigned int it=0; it<n_theta_steps; it++) {
         unsigned int it_this = it;
         unsigned int it_next = it + 1;
         if (it_next == n_theta_steps) it_next = 0;

         unsigned int idx_00 = idx_base_phi_00 + it_this;
         unsigned int idx_01 = idx_base_phi_00 + it_next;
         unsigned int idx_10 = idx_base_phi_10 + it_this;
         unsigned int idx_11 = idx_base_phi_10 + it_next;

         g_triangle t1(idx_00, idx_10, idx_11);
         g_triangle t2(idx_00, idx_11, idx_01);
         triangles.push_back(t1);
         triangles.push_back(t2);
      }
   }

   return simple_mesh_t(vertices, triangles);
}
