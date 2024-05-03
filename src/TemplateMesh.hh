/*
 * src/TemplateMesh.hh
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
 * Lesser General Public License for more details.
 *
 * You should have received a copies of the GNU General Public License and
 * the GNU Lesser General Public License along with this program; if not,
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA, 02110-1301, USA.
 * See http://www.gnu.org/licenses/
 *
 */

#ifndef HUD_TEXTURE_MESH_HH
#define HUD_TEXTURE_MESH_HH

#include <vector>
#include "g_triangle.hh"

#include "lights-info.hh"
#include "Shader.hh"
#include "Material.hh"

class XYMesh {
   GLuint vao;
   GLuint buffer_id;
   GLuint index_buffer_id;
   std::vector<glm::vec2> vertices;
   std::vector<g_triangle> triangles;
   std::string name;
   void init();
   void setup_buffers();
   bool draw_this_mesh;
   Material material;
   bool this_mesh_is_closed;

public:
   explicit XYMesh(const std::string &n) : name(n) { init(); }
   std::string get_name() const { return name; }
   bool get_draw_status() const { return draw_this_mesh; }
   void setup_quad();
   void set_material(Material mat) { material = mat; }
   void draw(Shader *shader_p);
   void close() {
      draw_this_mesh = false;
      this_mesh_is_closed = true; // and delete the buffers if not first time,
                                  //  so don't inline this function
   }
   void clear();
};


#endif // HUD_TEXTURE_MESH_HH
