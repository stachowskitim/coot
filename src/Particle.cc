/*
 * src/Particle.cc
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

#include <algorithm>
#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL // # for norm things
#include <glm/gtx/string_cast.hpp>  // to_string()

#include "Particle.hh"
#include "utils/coot-utils.hh"

void
Particle::update() {
   float v_scale = 0.009;
   v_scale = 0.015;
   glm::vec3 delta = v_scale * velocity;
   position += delta;
   velocity *= 0.992f;
   // colour.w *= 0.99;
   colour.g += colour_change_rate * 0.02;
   colour.r -= colour_change_rate * 0.02;
   colour.b += colour_change_rate * 0.001;
   life -= 0.18;
   // float r = 0.5 * (1.0 + random());
   // rotation += 0.01 * r; // currently a uniform is used, not this (which means they all spin at the same rate)
   rotation = 0.0;
}

void
Particle::update_gone_diego_particle() {

   float v_scale = 0.08;
   glm::vec3 delta = v_scale * velocity;
   position += delta;
   // colour.w *= 0.9;  I don't like fading, it make it look as if it's moving into the background
   life -= 0.08; // starts around 10 or so
   rotation += 1.4;
}

void
Particle::update_gone_diff_map_particle() {

   float v_scale = 0.18;
   glm::vec3 delta = v_scale * velocity;
   position += delta;
   life -= 0.03; // starts around 10 or so
   rotation += 0.01; // this number is not used in draw_particles(). Instead the
                     // rotation is based on the number of times the particle
                     // has been drawn. It *should* use this value.
}

void
particle_container_t::remove_old_particles() {

   auto remover = [] (const Particle &p) {
                     return (p.life <= 0.0);
                  };

   particles.erase(std::remove_if(particles.begin(), particles.end(), remover), particles.end());

   if (! particles.empty()) {
      if (particles[0].life <= 0.0)
         particles.clear();
   }
}

float
particle_container_t::random() const {

   const float d = static_cast<float>(RAND_MAX);
   long int r = coot::util::random();
   return static_cast<float>(r)/d;
}

void
particle_container_t::make_particles(unsigned int n_particles_per_burst,
                                     const std::vector<glm::vec3> &positions) {

   particles.clear();
   particles.reserve(n_particles_per_burst * positions.size());

   for (unsigned int ipos=0; ipos<positions.size(); ipos++) {
      const glm::vec3 &atom_position = positions[ipos];
      for (unsigned int i=0; i<n_particles_per_burst; i++) {
         float p0 = 2.0 * random() - 1.0;
         float p1 = 2.0 * random() - 1.0;
         float p2 = 2.0 * random() - 1.0;
         while((p0*p0 + p1*p1 + p2*p2) > 1.1) { // don't be "boxy"
            p0 = 2.0 * random() - 1.0;
            p1 = 2.0 * random() - 1.0;
            p2 = 2.0 * random() - 1.0;
         }
         glm::vec3 pp(p0,  p1, p2);
         glm::vec3 n = glm::normalize(pp);
         pp = n;
         float sc_pos = 0.1f;
         float sc_vel = 6.1f;
         glm::vec3 pos = sc_pos * pp;
         glm::vec3 vel = sc_vel * pp;
         glm::vec4 col(0.96, 0.26, 0.4, 1.0);
         if (false)
            std::cout << "Position-idx " << ipos << " Particle " << i << " " << glm::to_string(pos)
                      << "\tvelocity " << glm::to_string(vel) << " \t col: " << glm::to_string(col)
                      << std::endl;
         Particle p(pos + atom_position, vel, col, 10.0f - 9.0f * random());
         float ccr = 0.2 + 0.9 * random();
         p.colour_change_rate = ccr;
         particles.push_back(p);
      }
   }
}

void
particle_container_t::make_gone_diego_particles(unsigned int n_particles_per_burst,
                                                const std::vector<glm::vec3> &positions,
                                                const glm::vec3 &screen_x_uv,
                                                const glm::vec3 &screen_y_uv) {

   // pass screen x and y uvs for better positions

   for (unsigned int ipos=0; ipos<positions.size(); ipos++) {
      const glm::vec3 &gone_diego_position = positions[ipos];
      for (unsigned int i=0; i<n_particles_per_burst; i++) {
         double pi = 3.1415926;
         double alpha = 2.0 * pi * static_cast<double>(i)/static_cast<double>(n_particles_per_burst);
         double x = sin(alpha);
         double y = cos(alpha);
         // x and y should be aligned with the scren x and y axes
         float sf = 0.5f; // so that the ring of particles is about the same size as a diego.
         glm::vec3 pos = sf * static_cast<float>(x) * screen_x_uv + sf * static_cast<float>(y) * screen_y_uv;
         glm::vec3 vel = -2.0f * pos;
         glm::vec4 col(0.91, 0.78, 0.48, 1.0);
         float life = 1.5;
         Particle p(pos + gone_diego_position, vel, col, life);
         particles.push_back(p);
      }
   }
}

void
particle_container_t::make_gone_diff_map_peaks_particles(unsigned int n_particles_per_burst,
                                                      const std::vector<std::pair<glm::vec3, float> > &positions,
                                                      const glm::vec3 &screen_x_uv,
                                                      const glm::vec3 &screen_y_uv) { // usually just 1 or 2

   // std::cout << "make_gone_diff_map_peaks_particles() n_particles_per_burst " << n_particles_per_burst
   //           << std::endl;
   for (unsigned int ipos=0; ipos<positions.size(); ipos++) {
      const float &f = positions[ipos].second;
      const glm::vec3 &pos_base = positions[ipos].first;
      glm::vec4 col(0.2, 0.58, 0.2, 1.0);
      if (f > 0.0) col = glm::vec4(0.6, 0.2, 0.2, 1.0);
      for (unsigned int i=0; i<n_particles_per_burst; i++) {
         double pi = 3.1415926;
         double alpha = 2.0 * pi * static_cast<double>(i)/static_cast<double>(n_particles_per_burst);
         double x = sin(alpha);
         double y = cos(alpha);
         float sf = 0.5f;
         glm::vec3 pos = sf * static_cast<float>(x) * screen_x_uv + sf * static_cast<float>(y) * screen_y_uv;
         glm::vec3 vel = -0.3f * pos;
         float life = 0.5;
         Particle p(pos + pos_base, vel, col, life);
         // std::cout << "i " << i << " alpha " << alpha << " col " << glm::to_string(col) << std::endl;
         particles.push_back(p);
      }
   }
}

// pass the time?
void
particle_container_t::update_particles() {

   for (unsigned int i=0; i<particles.size(); i++)
      particles[i].update();

   remove_old_particles();
}

void
particle_container_t::update_gone_diff_map_particles() {

   for (unsigned int i=0; i<particles.size(); i++)
      particles[i].update_gone_diff_map_particle();
}

void
particle_container_t::update_gone_diego_particles() {

   for (unsigned int i=0; i<particles.size(); i++)
      particles[i].update_gone_diego_particle();

   // We don't call remove_old_particles(); here because we need to remove the mesh at the same time.

}

bool
particle_container_t::have_particles_with_life() const {

   bool status = false;
   for (unsigned int i=0; i<particles.size(); i++) {
      if (particles[i].life > 0.0) {
         status = true;
         break;
      }
   }
   return status;
}
