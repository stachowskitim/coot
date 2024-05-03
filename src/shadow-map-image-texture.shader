/*
 * src/shadow-map-image-texture.shader
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

// rename this "hud-image-texture.shader"
// because it is a shader for HUD images,
// not text.

#shader vertex

#version 330 core

layout (location = 0) in vec2 vertex;
layout (location = 1) in vec2 texCoord;

out vec2 texCoord_transfer;

// This shader is for textures - there is no instancing.
// 20210908-PE uses the same window resize scale and offset correction as is in rama-plot-phi-psi-markers.shader

uniform vec2 position;
uniform vec2 scales;

uniform vec2 window_resize_position_correction;
uniform vec2 window_resize_scales_correction;

void main() {

   vec2 c_scales = scales;
   // c_scales.x = 0.2;
   // c_scales.y = 0.2;
   vec2 scaled_vertices = vec2(vertex.x, vertex.y)  * c_scales;
   vec2 p1 = scaled_vertices + position;
   vec2 p2 = p1 * window_resize_scales_correction;
   vec2 p3 = p2 + window_resize_position_correction;

   gl_Position = vec4(vertex.x, -vertex.y , 0.0, 1.0); // invert the image in Y
   texCoord_transfer = texCoord;
}


#shader fragment

#version 330 core

uniform sampler2D image_texture;

in vec2 texCoord_transfer;

out vec4 outputColor;

void main() {

   bool this_is_the_hud_bar_labels = false; // pass this as a uniform

   vec4 sampled = texture(image_texture, texCoord_transfer);

   outputColor = sampled;
   outputColor = vec4(0.8 * sampled.r, 0.8 * sampled.r, 0.8 * sampled.r, 1.0);

}
