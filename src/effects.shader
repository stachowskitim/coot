/*
 * src/effects.shader
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

#shader vertex
// effects.shader

#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main() {

   TexCoords = aTexCoords;
   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);

}


#shader fragment
// effects.shader

#version 330 core

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D screenDepth;
uniform sampler2D ssao;

uniform bool use_ssao;
uniform float ssao_strength;
uniform vec4 background_colour;

// show *just* the ssao, that is
uniform bool show_ssao;
uniform int effects_output_type;
uniform float brightness;
uniform float gamma;


layout(location = 0) out vec4 out_color;


float get_fog_amount(float depth_in) {

   return 0.0;

   bool do_depth_fog = true;
   bool is_perspective_projection = true;
   if (do_depth_fog) {
      if (! is_perspective_projection) {
         float d = depth_in;
         return d * d;
      } else {
         // needs tweaking
         float d = depth_in;
         float d4 = d * d * d * d;
         // d4 = 0.25 * d; // 20220202-PE crow m,v,p matrices
         d4 = d * d * d * d;
         d4 = d;
         return d4;
      }
   } else {
      return 0.0;
   }
}

vec4 adjust_colour(vec4 colour_in, float brightness, float gamma) {

   vec3 c1 = vec3(brightness * colour_in.rgb);
   vec3 c2 = pow(c1, vec3(1.0/gamma));
   return vec4(c2, colour_in.a);

}


void main() {

   float depth = texture(screenDepth,   TexCoords).x;

   // this line make a difference. If it is not there, we are
   // editing the whole texture. If it is there, we are editing
   // only the pixels of the object (not the background).
   //

   gl_FragDepth = depth; // needed for depth for next shader // test remove

   float ao_mapped = 1.0;
   out_color = vec4(0,0,0,1);

   if (use_ssao) {
      float ao = texture(ssao, TexCoords).r;
      float inv_ao = 1.0 - ao; // now 1.0 is strong AO, 0.0 is no AO
      float str_inv_ao = 1.0 * ssao_strength * inv_ao;
      ao_mapped = 1.0 - str_inv_ao;
   }

   if (show_ssao) {
      out_color = vec4(vec3(ao_mapped), 1.0);
   } else {
      vec3 t = texture(screenTexture, TexCoords).rgb;
      vec4 t4 = vec4(t, 1.0);
#if 0
      float aafa = ao_mapped * get_fog_amount(gl_FragCoord.z);
      vec4 fogged_ssao = vec4(vec3(aafa), 1.0);
      out_color = 2.4 * t4 * fogged_ssao;
#endif

#if 1
      float compensating_scale = 1.0 + 0.1 * ssao_strength; // use more?

      // this doesn't work becuse if I add it there is a big colour descrepancy
      // betwween pixels at the back (0.99) and background pixels at 1.0
      // if (depth == 1.0)
      // compensating_scale = 1.0;

      out_color = compensating_scale * ao_mapped * t4;
      // 20221126-PE when SSAO strength is high and background is white,
      // we don't want the colour to fade out (become transparent), we want
      // it to get darker.
      out_color.a = 1.0;

      // Yikes! gl_FragCoord.z == 0.5 for all pixels.
      // if (gl_FragCoord.z == 0.5)
      // out_color = vec4(1,0,0,1);
      // maybe I can use depth, texture(screenDepth, TexCoords)?
#endif
      // out_color = texture(screenTexture, TexCoords);
      // out_color = vec4(vec3(ao_mapped), 1.0);
   }

   // see graphics-info.h effects_shader_output_type

   int eot = effects_output_type; // we can't change the uniform effects_output_type
   // eot = 2; // ssao

   // eot = 2; // force just the SSAO

   if (eot == 0) {
      // standard
   }
   if (eot == 1) {
      out_color = texture(screenTexture, TexCoords);
   }
   if (eot == 2) {
      out_color = vec4(vec3(texture(ssao, TexCoords).r), 1.0);
      // out_color = vec4(texture(ssao, TexCoords).rgb, 1.0);
   }
   if (eot == 3) {
      out_color = vec4(vec3(texture(screenDepth, TexCoords).r), 1.0);
   }

   // does this slow things down? Calculating pow() for every pixel?
   // 20220318-PE ... doesn't seem to affect the frame rate at all.
   //             when gamma and brightness are 1.0
   out_color = adjust_colour(out_color, brightness, gamma);

   // test hack!
   // out_color = texture(screenTexture, TexCoords);
   // out_color = texture(screenDepth, TexCoords);


   // out_color = vec4(1,1,0,1);

   // out_color = vec4(vec3(texture(ssao, TexCoords).r), 1.0);
   // out_color = 0.001 * gl_FragCoord;
   // out_color = texture(screenTexture, TexCoords);

   // out_color = texture(screenDepth, TexCoords);

}

