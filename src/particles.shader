/*
 * src/particles.shader
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

#shader vertex

#version 330 core

layout(location = 0) in vec3 position; // per vertex for the quad/hex
layout(location = 1) in vec3 normal;   // per vertex for the quad/hex
layout(location = 2) in vec4 colour;   // per vertex for the quad/hex
layout(location = 3) in vec3 instance_translation;
layout(location = 4) in vec4 instance_colour;

uniform mat4 mvp;
uniform mat4 view_rotation;
uniform float rotation_angle;
out vec4 colour_transfer;

void main() {

   float scale = 1.0;
   float sc_inst_trans = 1.0;

   vec3 position_scaled = scale * position;
   vec3 p1 = position_scaled;
   float a = - 3.0 * rotation_angle; // negative so that they spin clockwise
   float sin_a = sin(a);
   float cos_a = cos(a);
   vec4 p2 = vec4(p1.x * cos_a - p1.y * sin_a,
                  p1.x * sin_a + p1.y * cos_a, 0.0, 1.0);
   mat4 trans = transpose(view_rotation);
   vec4 p3 = trans * p2;
   vec4 p6 = p3 + vec4(instance_translation, 0.0);
   gl_Position = mvp * p6;

   colour_transfer = instance_colour; // * instance_colour;

}


#shader fragment

#version 330 core

in  vec4 colour_transfer;
out vec4 outputColor;

void main() {

    outputColor = colour_transfer;
    // outputColor = vec4(0.8, 0.1, 0.8, 1.0) + 0.001 * colour_transfer;
}
