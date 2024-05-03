/*
 * src/model-as-mesh.shader
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

// this is for an instanced Mesh.

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 colour; // For instanced objects, the colour is set
                                     // in the generic vertex (i.e. this), not the material.

layout(location = 3) in vec4 colour_instanced;
layout(location = 4) in vec4 model_rotation_translation_scale_0;
layout(location = 5) in vec4 model_rotation_translation_scale_1;
layout(location = 6) in vec4 model_rotation_translation_scale_2;
layout(location = 7) in vec4 model_rotation_translation_scale_3;

uniform mat4 mvp;
uniform mat4 view_rotation;
out vec4 colour_transfer;
out vec3 normal_transfer;
out vec4 frag_pos_transfer;
out mat3 model_rotation_transfer;

void main() {
   mat4 model_rotation_translation_scale = mat4(model_rotation_translation_scale_0,
                                                model_rotation_translation_scale_1,
                                                model_rotation_translation_scale_2,
                                                model_rotation_translation_scale_3);
   mat3 model_rotation = mat3(model_rotation_translation_scale_0.xyz,
                              model_rotation_translation_scale_1.xyz,
                              model_rotation_translation_scale_2.xyz);

   vec4 frag_pos = model_rotation_translation_scale * vec4(position, 1.0);

   gl_Position = mvp * frag_pos;

   normal_transfer = normal * transpose(model_rotation); // lights are a funny thing.

   colour_transfer = colour_instanced;
   frag_pos_transfer = frag_pos;
}


// -----------------------------------------
#shader fragment

#version 330 core

struct LightSource {
    bool is_on;
    bool directional;
    vec4 position;
    vec3 direction_in_molecule_coordinates_space;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 halfVector;
    vec3 spotDirection;
    float spotExponent;
    float spotCutoff;
    float spotCosCutoff;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};
struct Material {
   vec4 ambient;
   vec4 diffuse;
   float shininess;
   float specular_strength;
   vec4 specular;
};
uniform LightSource light_sources[2];
uniform vec3 eye_position;
uniform Material material;
uniform bool is_perspective_projection;
uniform vec4 background_colour;
uniform bool do_depth_fog;

in vec4 frag_pos_transfer;
in vec4 colour_transfer; // for instanced objects, the colour is set in the generic vertex, not the material
in vec3 normal_transfer;
in mat3 model_rotation_transfer;

out vec4 outputColor;

float get_fog_amount(float depth_in) {

   if (! is_perspective_projection) {
      return depth_in * depth_in;
   } else {
      // needs tweaking
      float d = depth_in;
      float d4 = d * d * d * d;
      return d4;
   }
}

void main() {

   vec4 bg_col = background_colour;
   vec4 sum_col = vec4(0,0,0,0);
   outputColor = vec4(0,0,0,0);

   float specular_strength = 1.0 * material.specular_strength;
   //specular_strength = 0.0;
   vec4 specular_light_colour = material.specular;

   // if (gl_FragCoord.z < 0.5) discard; // useful later maybe (in another shader)

   for (int i=0; i<2; i++) {
      if (light_sources[i].is_on) {
         vec3 light_dir = light_sources[i].direction_in_molecule_coordinates_space;
         float dp = dot(-normal_transfer, light_dir);
         // we can't have specular lights where there is no diffuse light
         if (dp <= 0.0)
            specular_strength = 0.0;
         dp = clamp(dp, 0.0, 1.0); // no negative dot products for diffuse

         // ambient
         vec4 ambient = 0.5 * light_sources[i].ambient * material.ambient * colour_transfer;

         // diffuse
         vec3 norm_2 = normalize(normal_transfer); // not needed?
         vec4 diffuse = 5.5 * light_sources[i].diffuse * dp * material.diffuse * colour_transfer;

         // specular
         // float shininess = material.shininess;
         // vec3 eye_pos = eye_position * transpose(model_rotation_transfer);
         // vec3 view_dir = normalize(eye_pos - frag_pos_transfer.xyz); // frag_pos_transfer is a vec3 in model.shader
         // vec3 reflect_dir = reflect(-light_dir, norm_2);
         // reflect_dir = normalize(reflect_dir); // belt and braces, needed?
         // float dp_view_reflect = dot(view_dir, reflect_dir);
         // dp_view_reflect = clamp(dp_view_reflect, 0.0, 1.0);
         // float spec = pow(dp_view_reflect, shininess);
         // vec4 specular = specular_strength * spec * light_sources[i].specular;
         // vec4 col_specular = 3.0 * specular_strength * spec * specular_light_colour * light_sources[i].specular;
         // sum_col += ambient + diffuse + col_specular;

         sum_col += ambient + diffuse;

      }
   }

   float fog_amount = 0.0;
   if (do_depth_fog)
      fog_amount = get_fog_amount(gl_FragCoord.z);
   outputColor += mix(sum_col, bg_col, fog_amount);

}
