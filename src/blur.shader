/*
 * src/blur.shader
 *
 * Copyright 2019 by Medical Research Council
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
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform float zoom;

out vec2 TexCoords;

void main() {

   TexCoords = aTexCoords;
   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);

}


#shader fragment

#version 330 core

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D screenDepth;
uniform float zoom;
uniform bool is_perspective_projection;
uniform bool do_depth_blur;
uniform bool do_outline;

layout(location = 0) out vec4 out_color;

float depth_scale(float depth_in_centre, float depth_in_ij, float lim) {

   // -1 means there is no blurring to be done.

   // xx_ij is the point being blurred. xx_centre is the point
   // being blurred into. Keep a clear head.

   // the deeper into the image we go, (depth_in_centre approaches 1.0)
   // then the more we want to the colour to be influenced by the surroundings.

   if (depth_in_centre < lim) {
      return -1.0;
   } else {
      if (depth_in_ij < lim) {
         return -1.0;
      } else {
         return (depth_in_ij-lim)/(1.0-lim);
      }
   }
}

vec3 sampling_blur(int n_pixels_max) {

   float depth_centre = texture(screenDepth, TexCoords).x;
   vec3 result = vec3(1.0, 1.0, 0.0);

   // if (depth_centre < 0.2) discard;

   // centre is the point being blurred *into*

   vec3 orig_colour = texture(screenTexture, TexCoords).rgb; // don't blur
   float lim = 0.501;
   if (is_perspective_projection) lim = 0.6; // needs checking
   if (depth_centre < lim) {
      return orig_colour;
   } else {
      vec3 sum_outer = vec3(0,0,0);
      vec3 sum_inner = vec3(0,0,0);
      vec2 tex_scale = 1.0/textureSize(screenTexture, 0);
      // the 0.03 here depends on how much of the origin colour we add back
      // that is currently 0.5
      float centre_points_scale = 0.06;
      int n_inner_neighbs = 0;
      int n_outer_neighbs = 0;
      float sum_for_alpha = 0;
      float w_inner_neighbs = 0;
      for (int ix= -n_pixels_max; ix<=n_pixels_max; ix++) {
         for (int iy= -n_pixels_max; iy<=n_pixels_max; iy++) {
            float r_sqrd = float(ix*ix + iy*iy) / float(n_pixels_max * n_pixels_max); // can optimize
            if (r_sqrd > 1.0) continue;
            vec2 offset_coords = TexCoords + vec2(tex_scale.x * ix, tex_scale.y * iy);
            float depth_ij = texture(screenDepth, offset_coords).x;
            if (depth_ij == 1.0) continue;
            if (depth_ij < lim) continue; // don't blur from the lines in focus
            vec3 colour_ij = texture(screenTexture, offset_coords).rgb;
            // depth_scale() return -1 for no blurring.
            float dbrs = depth_scale(depth_centre, depth_ij, lim);
            if (ix == 5550 && iy == 0) {
               // sum_inner += colour_ij; // use orig_colour
            } else {
               int inner_neighb_limit = 3;
               inner_neighb_limit = 2;
               if (abs(ix) < inner_neighb_limit && abs(iy) < inner_neighb_limit) {
                  float md = float(abs(ix) + abs(iy));
                  if (md == 0) md = 0.5; // was 0.5
                  float w = 0.2 + 1.0 / md;
                  sum_inner += w * colour_ij;
                  n_inner_neighbs++;
                  w_inner_neighbs += w;
               } else {
                  if (dbrs < 0.0) {
                     // nothing
                  } else {
                     float blur_radius = (depth_ij - lim) / (1.0 - lim);
                     // this scaling needs to be zoom dependent.
                     float k = 0.001; // gaussian scale
                     dbrs = 1.0;
                     float gauss = 1.0/dbrs * exp(-0.5 * k * r_sqrd/(blur_radius*blur_radius));
                     // gauss = 1.0;
                     float depth_factor = 2.0 * (1.0 - depth_centre); // 0 -> 1 for lim = 0.5
                     depth_factor = 1.0;
                     sum_outer += colour_ij * gauss * (1.0 - depth_ij) * depth_factor;
                     sum_for_alpha += gauss * (1.0 - depth_ij) * depth_factor;
                     n_outer_neighbs++;
                  }
               }
            }
         }
      }
      vec3 average_col_from_inner_neighbs = sum_inner / w_inner_neighbs;
      if (w_inner_neighbs == 0.0f)
         average_col_from_inner_neighbs = vec3(0,0,0); // sanitize before blending
      float alpha_inner = w_inner_neighbs;
      alpha_inner = clamp(alpha_inner, 0.0f, 1.0f);
      float alpha = 0.0f; // not at the moment. was clamp(0.5 * sum_for_alpha, 0.0f, 1.0f);
      // alpha = clamp(0.5 * sum_for_alpha, 0.0f, 1.0f);
      float Sc = 0.4f/float(n_pixels_max*n_pixels_max); // was 0.6
      vec3 result_intermediate = mix(orig_colour, average_col_from_inner_neighbs, alpha_inner);
      result = mix(result_intermediate, Sc * sum_outer, alpha);
      if (n_inner_neighbs == 0)
         result = orig_colour;
   }
   return result;
}

vec3 make_outline() {
   float depth_centre = texture(screenDepth, TexCoords).x;
   vec3 orig_colour = texture(screenTexture, TexCoords).rgb; // don't blur
   vec3 result = orig_colour; // update this as needed.
   vec2 tex_scale = 1.0/textureSize(screenTexture, 0);
   int n_deep_neighbs = 0;
   for (int ix= -1; ix<=1; ix++) {
      for (int iy= -1; iy<=1; iy++) {
         vec2 offset_coords = TexCoords + vec2(tex_scale.x * ix, tex_scale.y * iy);
         float depth_ij = texture(screenDepth, offset_coords).x;
         if ((depth_ij - depth_centre) > 0.1 * (1.0 - 0.1 * depth_centre)) {
            n_deep_neighbs++;
         }
      }
   }
   if (n_deep_neighbs > 1) {
      result = vec3(0.1, 0.1, 0.1);
   }
   return result;
}


void main() {

   vec3 result = vec3(0,0,0);

   if (do_depth_blur && ! do_outline) {
      result = sampling_blur(8); // 14 is good
   } else {
      if (do_outline) {
         result = make_outline();
      } else {
         float depth_centre = texture(screenDepth, TexCoords).x;
         result = texture(screenTexture, TexCoords).rgb; // don't blur
         if (zoom < 0.02)
            if (depth_centre > 0.99)
               result = vec3(0.1, 0.3, 0.1);
      }
   }

   out_color = vec4(result, 1.0);

}
