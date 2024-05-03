/*
 * src/graphics-info-tick-function.cc
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
#ifdef USE_PYTHON
#include <Python.h>
#endif // USE_PYTHON

#define GLM_ENABLE_EXPERIMENTAL // # for norm things
// #include <glm/ext.hpp> // 20240326-PE
#include <glm/gtx/string_cast.hpp>  // to_string()

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <gtk/gtk.h>
#include <epoxy/gl.h>

#include "c-interface.h" // for update_go_to_atom_from_current_position()
#include "globjects.h"
#include "graphics-info.h"

#include "draw.hh"
#include "draw-2.hh"
#include "framebuffer.hh"

#include "text-rendering-utils.hh"
#include "cc-interface-scripting.hh"
// #include "cylinder-with-rotation-translation.hh" 20230108-PE

gint idle_contour_function(gpointer data);


// static
gboolean
graphics_info_t::tick_function_is_active() {

   if (false)
      std::cout << "tick_function_is_active() " << do_tick_particles << " spin: " << do_tick_spin << " " << do_tick_boids << " "
                << "H-bond: " << do_tick_hydrogen_bonds_mesh << " " << do_tick_happy_face_residue_markers << " "
                << " constant-draw "  << do_tick_constant_draw << std::endl;

   if (do_tick_particles ||
       do_tick_spin      ||
       do_tick_rock      ||
       do_tick_boids     ||
       do_tick_constant_draw       ||
       do_tick_hydrogen_bonds_mesh ||
       do_tick_outline_for_active_residue ||
       do_tick_happy_face_residue_markers ||
       do_tick_gone_diegos ||
       do_tick_gone_diff_map_peaks)
      return gboolean(TRUE);
   else
      return gboolean(FALSE);
}

// Put this and the above into graphics_info_t. And in it's own file.

gboolean
graphics_info_t::glarea_tick_func(GtkWidget *widget,
                                  G_GNUC_UNUSED GdkFrameClock *frame_clock,
                                  G_GNUC_UNUSED gpointer data) {

   graphics_info_t::tick_function_is_active();

   if (graphics_info_t::do_tick_particles) {
      if (graphics_info_t::particles.empty()) {
         graphics_info_t::do_tick_particles = false;
      } else {
         gtk_gl_area_attach_buffers(GTK_GL_AREA(graphics_info_t::glareas[0])); // needed?
         // std::cout << "glarea_tick_func() calls update_particles() " << std::endl;
         graphics_info_t::particles.update_particles();
         graphics_info_t::mesh_for_particles.update_instancing_buffer_data_for_particles(particles);
      }
   }

   if (do_tick_gone_diegos) {
      if (meshed_particles_for_gone_diegos.empty()) {
         do_tick_gone_diegos = false;
      } else {

         for (unsigned int ip=0; ip<meshed_particles_for_gone_diegos.size(); ip++) {
            // give back the GL buffers for meshes that will be removed
            auto &particles = meshed_particles_for_gone_diegos[ip].particle_container;
            auto &mesh      = meshed_particles_for_gone_diegos[ip].mesh;
            if (particles.have_particles_with_life()) {
            } else {
               mesh.delete_gl_buffers();
            }
         }

         auto remover = [] (const meshed_particle_container_t &mp) {
            bool still_alive = mp.particle_container.have_particles_with_life();
            return ! still_alive;
         };

         meshed_particles_for_gone_diegos.erase(std::remove_if(meshed_particles_for_gone_diegos.begin(),
                                                                meshed_particles_for_gone_diegos.end(), remover),
                                                meshed_particles_for_gone_diegos.end());

         for (unsigned int ip=0; ip<meshed_particles_for_gone_diegos.size(); ip++) {
            auto &particles = meshed_particles_for_gone_diegos[ip].particle_container;
            auto &mesh      = meshed_particles_for_gone_diegos[ip].mesh;
            particles.update_gone_diego_particles();
            mesh.update_instancing_buffer_data_for_particles(particles);
         }
      }
   }

   if (do_tick_gone_diff_map_peaks) {
      // this could be tidied up
      auto &particles = meshed_particles_for_gone_diff_map_peaks.particle_container;
      auto &mesh      = meshed_particles_for_gone_diff_map_peaks.mesh;
      meshed_particles_for_gone_diff_map_peaks.particle_container.update_gone_diff_map_particles();
      meshed_particles_for_gone_diff_map_peaks.mesh.update_instancing_buffer_data_for_particles(particles);
      if (! particles.have_particles_with_life()) {
         particles.clear();
         mesh.clear();
         do_tick_gone_diff_map_peaks = false;
      }
   }

   if (graphics_info_t::do_tick_spin) {
      float delta = 0.004 * graphics_info_t::idle_function_rotate_angle;
      // delta *= 10.0;
      glm::vec3 EulerAngles(0, delta, 0);
      glm::quat quat_delta(EulerAngles);
      glm::quat normalized_quat_delta(glm::normalize(quat_delta));
      glm::quat product = normalized_quat_delta * graphics_info_t::view_quaternion;
      graphics_info_t::view_quaternion = glm::normalize(product);
   }

   if (graphics_info_t::do_tick_rock) {
      std::chrono::time_point<std::chrono::high_resolution_clock> tp_now = std::chrono::high_resolution_clock::now();
      auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(tp_now - graphics_info_t::time_holder_for_rocking);
      double angle = delta.count() * 0.0007 * graphics_info_t::idle_function_rock_freq_scale_factor;
      double theta = 0.008 * graphics_info_t::idle_function_rock_amplitude_scale_factor * sin(angle);
      glm::vec3 EulerAngles(0, theta, 0);
      glm::quat quat_delta(EulerAngles);
      glm::quat normalized_quat_delta(glm::normalize(quat_delta));
      glm::quat product = normalized_quat_delta * graphics_info_t::view_quaternion;
      graphics_info_t::view_quaternion = glm::normalize(product);
   }

   if (graphics_info_t::do_tick_outline_for_active_residue > 0) {
      graphics_info_t::outline_for_active_residue_frame_count--;
      if (graphics_info_t::outline_for_active_residue_frame_count == 0) {
         graphics_info_t::do_tick_outline_for_active_residue = false;
      }
   }

   if (graphics_info_t::do_tick_constant_draw) {
      // don't change anything - I just want to remind you (well myself, I suppose) that it's here
   }

   if (graphics_info_t::do_tick_hydrogen_bonds_mesh) {
      // 20211210-PE  the rotation is done in instanced-object.shader now
   }

   if (graphics_info_t::do_tick_boids) {
      graphics_info_t::boids.update();
      std::vector<glm::mat4> mats(graphics_info_t::boids.size());
      for (unsigned int ii=0; ii<graphics_info_t::boids.size(); ii++)
         mats[ii] = graphics_info_t::boids[ii].make_mat();
      graphics_info_t::mesh_for_boids.update_instancing_buffer_data_standard(mats);
   }

   if (false)
      std::cout << "### in the glarea_tick_func() "
                << graphics_info_t::do_tick_happy_face_residue_markers << "  "
                << graphics_info_t::draw_count_for_happy_face_residue_markers << std::endl;

   if (graphics_info_t::do_tick_happy_face_residue_markers) {
      // this is a texture mesh, currently direct access to the draw flag.
      if (graphics_info_t::tmesh_for_happy_face_residues_markers.draw_this_mesh) {
         graphics_info_t::draw_count_for_happy_face_residue_markers += 1;
         graphics_info_t g;
         if (g.draw_count_for_happy_face_residue_markers >= g.draw_count_max_for_happy_face_residue_markers) {
            graphics_info_t::do_tick_happy_face_residue_markers = false;
            graphics_info_t::draw_count_for_happy_face_residue_markers = 0;

            graphics_info_t::tmesh_for_happy_face_residues_markers.draw_this_mesh = false;
         }

         // repeating code in setup_draw_for_happy_face_residue_markers()

         const std::vector<glm::vec3> &positions = graphics_info_t::happy_face_residue_marker_starting_positions;
         glm::vec3 up_uv = g.get_screen_y_uv();
         unsigned int draw_count = g.draw_count_for_happy_face_residue_markers;
         unsigned int draw_count_max = g.draw_count_max_for_happy_face_residue_markers;
         g.tmesh_for_happy_face_residues_markers.update_instancing_buffer_data_for_happy_faces(positions,
                                                                                               draw_count,
                                                                                               draw_count_max,
                                                                                               up_uv);
      }
   }

   gtk_widget_queue_draw(widget); // needed? 20210904-PE yeah... I  think so

   return graphics_info_t::tick_function_is_active();
}
