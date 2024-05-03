/*
 * src/gl-rama-plot.hh
 *
 * Copyright 2021 by Medical Research Council
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

#ifndef GL_RAMA_PLOT_HH
#define GL_RAMA_PLOT_HH

#include <clipper/core/ramachandran.h>
#include "rama-plot-phi-psi.hh"
#include "HUDMesh.hh"
#include "HUDTextureMesh.hh"
#include "Texture.hh"
#include "Shader.hh"

class mouse_over_hit_t {

   // we want to know both if a residue rama-texture was clicked on
   // (and if it was, what was it)
   // and also if we clicked on the plot.
   // We don't want clicks over the plot, even if they don't hit a residue
   // to go through to the atoms behind/below.

public:
   bool residue_was_clicked;
   bool plot_was_clicked;
   coot::residue_spec_t residue_spec;
   mouse_over_hit_t(bool c1, bool c2, const coot::residue_spec_t &rs) : residue_was_clicked(c1),
                                                                        plot_was_clicked(c2),
                                                                        residue_spec(rs) {}
};

class gl_rama_plot_t {

   class canvas_tick_t {
   public:
      double x, y;
      short int axis;
      // i==0 for x axis, i==1 for y axis.
      canvas_tick_t(int i, double a, double b) {
         x = a;
         y = b;
         axis = i;
      }
      double start_x() {return x;};
      double start_y() {return y;};
      double end_x() { if (axis == 0) return x; else return x - 10;};
      double end_y() { if (axis == 1) return y; else return y + 10;}; 
   };
   clipper::Ramachandran rama_all;
   clipper::Ramachandran rama_gly;
   clipper::Ramachandran rama_pro;
   clipper::Ramachandran rama_non_gly_pro;
   clipper::Ramachandran rama_ile_val;
   clipper::Ramachandran rama_pre_pro;
   clipper::Ramachandran rama_non_gly_pro_ile_val_or_pre_pro;
   double rama_threshold_preferred;
   double rama_threshold_allowed;
   std::string colour_for_OK_phi_psi;
   std::string colour_for_OK_phi_psi_pro;
   std::string colour_for_outlier_phi_psi;
   std::map<coot::residue_spec_t, rama_plot::phi_psi_t> phi_psi_map;
   clipper::Ramachandran::TYPE current_background_type;
   std::map<coot::residue_spec_t, rama_plot::phi_psi_t> generate_pseudo_phi_psis();
   std::map<coot::residue_spec_t, rama_plot::phi_psi_t> generate_phi_psis(int imol, const std::string &residue_selection,
                                                                          mmdb::Manager *mol_in);
   bool draw_outliers_only_flag; 
   void init(); // rama things
   HUDTextureMesh hud_tmesh_for_other_normal;
   HUDTextureMesh hud_tmesh_for_other_outlier;
   HUDTextureMesh hud_tmesh_for_pro_normal;
   HUDTextureMesh hud_tmesh_for_pro_outlier;
   HUDTextureMesh hud_tmesh_for_gly_normal;
   HUDTextureMesh hud_tmesh_for_gly_outlier;
   Texture texture_for_other_normal;
   Texture texture_for_other_outlier;
   Texture texture_for_pro_normal;
   Texture texture_for_pro_outlier;
   Texture texture_for_gly_normal;
   Texture texture_for_gly_outlier;
   HUDTextureMesh hud_tmesh_for_global_distribution_non_gly_pro;
   HUDTextureMesh hud_tmesh_for_global_distribution_pro;
   HUDTextureMesh hud_tmesh_for_global_distribution_gly;
   Texture texture_for_global_distribution_non_gly_pro;
   Texture texture_for_global_distribution_gly;
   Texture texture_for_global_distribution_pro;
   HUDMesh hud_mesh_for_axes_and_ticks;

   // probably these should be in a lower-lever library.
   enum screen_position_origins_t { TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT};

   std::pair<glm::vec2, glm::vec2>
    get_munged_offset_and_scale(screen_position_origins_t spo,
                                const glm::vec2 &offset_natural,
                                float scale_x_natural, float scale_y_natural,
                                int glarea_width, int glarea_height) const;
   
   void update_hud_tmeshes(const std::map<coot::residue_spec_t, rama_plot::phi_psi_t> &phi_psi_map);
   float rama_plot_scale;

public:
   gl_rama_plot_t() :
      hud_tmesh_for_other_normal("hud_tmesh_for_other_normal"),
      hud_tmesh_for_other_outlier("hud_tmesh_for_other_outlier"),
      hud_tmesh_for_pro_normal("hud_tmesh_for_pro_normal"),
      hud_tmesh_for_pro_outlier("hud_tmesh_for_pro_outlier"),
      hud_tmesh_for_gly_normal("hud_tmesh_for_gly_normal"),
      hud_tmesh_for_gly_outlier("hud_tmesh_for_gly_outlier"),
      hud_tmesh_for_global_distribution_non_gly_pro("hud_tmesh_for_global_distribution_non_gly_pro"),
      hud_tmesh_for_global_distribution_pro("hud_tmesh_for_global_distribution_pro"),
      hud_tmesh_for_global_distribution_gly("hud_tmesh_for_global_distribution_gly") { init(); }
   void setup_buffers(float rama_plot_scale); // setup OpenGL things - must be done after OpenGL realize()
   void setup_from(int imol, mmdb::Manager *mol, const std::string &residue_selection);
   void update_phi_psis_on_moved_atoms();
   // void background_to_type(GtkWidget *canvas, clipper::Ramachandran::TYPE); // don't change it if we are already there of course.
   void draw(Shader *shader_for_axes_and_tick,
             Shader *shader_for_rama_plot_phi_psis_markers_p,  // instanced
             Shader *shader_for_hud_textures_p,
             int glarea_height_at_hud_start,
             int glarea_width_at_hud_start,
             int glarea_current_width, int glarea_current_height,
             bool clear_needed_flag);
   std::string residue_selection;
   float position_hash; // updated and tested in setup_from() so that we don't recalculate if we don't need to
   void clear(); // empty the residue phi,psi map and delete the gl buffers (keep the vao)
   bool is_active() const; // for mouse-overing
   mouse_over_hit_t get_mouse_over_hit(double x_mouse, double y_mouse,
                                       int widget_width, int widget_height) const;
   // lightweight, don't draw,
   void set_outliers_only(bool state) { draw_outliers_only_flag = state; }
};

#endif // GL_RAMA_PLOT_HH
