/*
 * src/texture-meshes-for-ssao.shader
 *
 * Copyright 2022 by Medical Research Council
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

// this used to be obj.shader when it was in hmt.

#shader vertex

#version 330 core

// make it work like 9.ssao_geometry.shader

// the normal calculation is probably wrong

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec3 bitangent;
layout(location = 4) in vec4 colour;
layout(location = 5) in vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 frag_pos_transfer;
out vec3 normal_transfer;

void main() {

   mat4 mvp = projection * view * model;
   gl_Position = mvp * vec4(position, 1.0);

   mat3 normal_matrix = mat3(view);
   mat3 transpose_normal_matrix = transpose(normal_matrix);

   normal_transfer = normal * transpose_normal_matrix;
   vec4 frag_pos_transfer_v4 = view * model * vec4(position, 1.0);
   frag_pos_transfer = frag_pos_transfer_v4.xyz;

}

#shader fragment

#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;

in vec3 frag_pos_transfer;
in vec3 normal_transfer;

void main() {

   vec3 normal = normal_transfer;

   // vec3 normal_view_rotated = (vec4(normal, 1.0) * view_rotation).xyz;
   // normal_view_rotated = normal; // I don't understand how this works.

   gPosition = frag_pos_transfer;
   gNormal = normalize(normal_transfer); // do we need to normalize?

}
