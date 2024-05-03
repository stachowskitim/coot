/* src/glade-callbacks.cc
 *
 * Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007 The University of York
 * Author: Paul Emsley
 * Copyright 2008 The University of Oxford
 * Copyright 2015, 2016 by Medical Research Council
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
 * You should have received a copy of the GNU General Public License and
 * the GNU Lesser General Public License along with this program; if not,
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA, 02110-1301, USA.
 */

#include "cc-interface.hh" // maybe these callback/settigns for shader/lighting should have its own .hh file?
#include "widget-from-builder.hh"


// 20220222-PE add new callback for shader settings

extern "C" G_MODULE_EXPORT
void
on_shader_settings_dialog_response(GtkDialog       *dialog,
                                   gint             response_id,
                                   gpointer         user_data) {

   if (response_id == GTK_RESPONSE_CLOSE) {
      gtk_widget_set_visible(GTK_WIDGET(dialog), FALSE);
   }
}

extern "C" G_MODULE_EXPORT
void
on_shader_settings_do_depth_fog_checkbutton_toggled(GtkCheckButton *checkbutton,
                                                    gpointer        user_data) {

   if (gtk_check_button_get_active(checkbutton)) {
      set_use_fog(1);
   } else {
      set_use_fog(0);
   }
}

extern "C" G_MODULE_EXPORT
void
on_shader_settings_do_depth_blur_checkbutton_toggled(GtkCheckButton *checkbutton,
                                                     gpointer        user_data) {
   
   if (gtk_check_button_get_active(checkbutton)) {
      set_use_depth_blur(1);
   } else {
      set_use_depth_blur(0);
   }
}


extern "C" G_MODULE_EXPORT
void
on_shader_settings_depth_blur_outline_off_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                              gpointer        user_data) {

   if (gtk_check_button_get_active(checkbutton)) {
      std::cout << "off activated " << std::endl;
      set_use_outline(0);
      set_use_depth_blur(0);
   }
}


extern "C" G_MODULE_EXPORT
void
on_shader_settings_depth_blur_outline_depth_blur_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                                     gpointer        user_data) {

   if (gtk_check_button_get_active(checkbutton)) {
      std::cout << "blur activated " << std::endl;
      set_use_outline(0);
      set_use_depth_blur(1);
   }
}


extern "C" G_MODULE_EXPORT
void
on_shader_settings_depth_blur_outline_outline_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                                  gpointer        user_data) {

   if (gtk_check_button_get_active(checkbutton)) {
      std::cout << "on_shader_settings_depth_blur_outline_outline_radiobutton_toggled() outline toggled " << std::endl;
      set_use_outline(1);
      set_use_depth_blur(0);
   }
}



extern "C" G_MODULE_EXPORT
void
on_shader_settings_depth_blur_strength_scale_value_changed(GtkRange        *range,
                                                           gpointer         user_data) {

   gdouble f = gtk_range_get_value(range);
   set_focus_blur_strength(f);
}

extern "C" G_MODULE_EXPORT
void
on_shader_settings_depth_blur_focus_depth_scale_value_changed(GtkRange        *range,
                                                              gpointer         user_data) {
   gdouble f = gtk_range_get_value(range);
   set_focus_blur_z_depth(f);
}

extern "C" G_MODULE_EXPORT
void
on_shader_settings_shadow_texture_resolution_multiplier_6_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                                              gpointer         user_data) {
   if (gtk_check_button_get_active(checkbutton)) {
      set_shadow_texture_resolution_multiplier(6);
   }
}

extern "C" G_MODULE_EXPORT
void
on_shader_settings_shadow_texture_resolution_multiplier_5_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                                              gpointer         user_data) {
   if (gtk_check_button_get_active(checkbutton)) {
      set_shadow_texture_resolution_multiplier(5);
   }
}

extern "C" G_MODULE_EXPORT
void
on_shader_settings_shadow_texture_resolution_multiplier_4_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                                              gpointer         user_data) {
   if (gtk_check_button_get_active(checkbutton)) {
      set_shadow_texture_resolution_multiplier(4);
   }
}

extern "C" G_MODULE_EXPORT
void
on_shader_settings_shadow_texture_resolution_multiplier_3_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                                              gpointer         user_data) {
   if (gtk_check_button_get_active(checkbutton)) {
      set_shadow_texture_resolution_multiplier(3);
   }
}


extern "C" G_MODULE_EXPORT
void
on_shader_settings_shadow_texture_resolution_multiplier_2_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                                              gpointer         user_data) {
   if (gtk_check_button_get_active(checkbutton)) {
      set_shadow_texture_resolution_multiplier(2);
   }
}

extern "C" G_MODULE_EXPORT
void
on_shader_settings_shadow_texture_resolution_multiplier_1_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                                              gpointer         user_data) {
   if (gtk_check_button_get_active(checkbutton)) {
      set_shadow_texture_resolution_multiplier(1);
   }
}


extern "C" G_MODULE_EXPORT
void
on_shader_settings_shadow_softness_3_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                         gpointer         user_data) {
   if (gtk_check_button_get_active(checkbutton)) {
      set_shadow_softness(3);
   }
}


extern "C" G_MODULE_EXPORT
void
on_shader_settings_shadow_softness_2_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                         gpointer         user_data) {
   if (gtk_check_button_get_active(checkbutton)) {
      set_shadow_softness(2);
   }
}

extern "C" G_MODULE_EXPORT
void
on_shader_settings_shadow_softness_1_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                         gpointer         user_data) {
   if (gtk_check_button_get_active(checkbutton)) {
      set_shadow_softness(1);
   }
}


extern "C" G_MODULE_EXPORT
void
on_shader_settings_shadow_softness_0_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                         gpointer         user_data) {
   if (gtk_check_button_get_active(checkbutton)) {
      set_shadow_softness(0);
   }
}


extern "C" G_MODULE_EXPORT
void
on_shader_settings_shadow_strength_scale_value_changed(GtkRange        *range,
                                                       gpointer         user_data) {
   gdouble f = gtk_range_get_value(range);
   set_shadow_strength(f);
}


extern "C" G_MODULE_EXPORT
void
on_shader_settings_ssao_smoothing_blur_size_2_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                                  gpointer         user_data) {
   if (gtk_check_button_get_active(checkbutton)) {
      set_ssao_blur_size(2);
   }
}


extern "C" G_MODULE_EXPORT
void
on_shader_settings_ssao_smoothing_blur_size_1_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                                  gpointer         user_data) {

   if (gtk_check_button_get_active(checkbutton)) {
      set_ssao_blur_size(1);
   }
}


extern "C" G_MODULE_EXPORT
void
on_shader_settings_ssao_smoothing_blur_size_0_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                                  gpointer         user_data) {
   if (gtk_check_button_get_active(checkbutton)) {
      set_ssao_blur_size(0);
   }
}


extern "C" G_MODULE_EXPORT
void
on_shader_settings_ssao_n_kernel_samples_scale_value_changed(GtkRange        *range,
                                                             gpointer         user_data) {
   gdouble f = gtk_range_get_value(range);
   unsigned int n = static_cast<int>(f);
   set_ssao_kernel_n_samples(n);
}


extern "C" G_MODULE_EXPORT
void
on_shader_settings_ssao_radius_scale_value_changed(GtkRange        *range,
                                                                       gpointer         user_data) {
   gdouble f = gtk_range_get_value(range);
   set_ssao_radius(f);
}

extern "C" G_MODULE_EXPORT
void
on_shader_settings_ssao_bias_scale_value_changed(GtkRange        *range,
                                                                     gpointer         user_data) {
   gdouble f = gtk_range_get_value(range);
   set_ssao_bias(f);
}


extern "C" G_MODULE_EXPORT
void
on_shader_settings_ssao_strength_scale_value_changed(GtkRange        *range,
                                                                         gpointer         user_data) {
   gdouble f = gtk_range_get_value(range);
   set_ssao_strength(f);
}


extern "C" G_MODULE_EXPORT
void
on_shader_settings_fancy_mode_togglebutton_toggled(GtkToggleButton *togglebutton,
                                                   gpointer         user_data) {

   std::cout << "on_shader_settings_fancy_mode_togglebutton_toggled() " << std::endl;

   if (gtk_toggle_button_get_active(togglebutton)) {
      set_use_simple_lines_for_model_molecules(0);
      set_use_fancy_lighting(1);
      GtkWidget *fancy_vbox1 = widget_from_builder("shader_settings_fancy_vbox1");
      GtkWidget *fancy_vbox2 = widget_from_builder("shader_settings_fancy_vbox2");
      gtk_widget_set_sensitive(fancy_vbox1, TRUE);
      gtk_widget_set_sensitive(fancy_vbox2, TRUE);
   } else {
      std::cout << "in on_shader_settings_fancy_mode_radiobutton_toggled() button was not active" << std::endl;
   }
}


extern "C" G_MODULE_EXPORT
void
on_shader_settings_standard_mode_togglebutton_toggled(GtkToggleButton *togglebutton,
                                                      gpointer         user_data) {

   std::cout << "in on_shader_settings_standard_mode_radiobutton_toggled()"
             << " is-toggle " << GTK_IS_TOGGLE_BUTTON(togglebutton)
             << " is-check " << GTK_IS_CHECK_BUTTON(togglebutton)
             << std::endl;

   if (gtk_toggle_button_get_active(togglebutton)) {
      set_use_simple_lines_for_model_molecules(0);
      set_use_fancy_lighting(0);
      GtkWidget *fancy_vbox1 = widget_from_builder("shader_settings_fancy_vbox1");
      GtkWidget *fancy_vbox2 = widget_from_builder("shader_settings_fancy_vbox2");
      gtk_widget_set_sensitive(fancy_vbox1, FALSE);
      gtk_widget_set_sensitive(fancy_vbox2, FALSE);
   } else {
      GtkWidget *basic_mode_togglebutton = widget_from_builder("shader_settings_basic_mode_togglebutton");
      GtkWidget *fancy_vbox1 = widget_from_builder("shader_settings_fancy_vbox1");
      GtkWidget *fancy_vbox2 = widget_from_builder("shader_settings_fancy_vbox2");
      bool is_fancy_mode = true;
      if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(basic_mode_togglebutton))) is_fancy_mode = false;
      if (! is_fancy_mode) {
         gtk_widget_set_sensitive(fancy_vbox1, FALSE);
         gtk_widget_set_sensitive(fancy_vbox2, FALSE);
      }
   }
}

extern "C" G_MODULE_EXPORT
void
on_shader_settings_brightness_scale_value_changed(GtkRange        *range,
                                                  gpointer         user_data) {
   gdouble f = gtk_range_get_value(range);
   set_effects_shader_brightness(f);
}

extern "C" G_MODULE_EXPORT
void
on_shader_settings_gamma_scale_value_changed(GtkRange        *range,
                                             gpointer         user_data) {
   gdouble f = gtk_range_get_value(range);
   set_effects_shader_gamma(f);
}

extern "C" G_MODULE_EXPORT
void
on_shader_settings_basic_mode_togglebutton_toggled(GtkToggleButton *togglebutton,
                                                   gpointer         user_data) {

   if (gtk_toggle_button_get_active(togglebutton)) {
      set_use_simple_lines_for_model_molecules(1);
      set_use_fancy_lighting(0);
      GtkWidget *fancy_vbox1 = widget_from_builder("shader_settings_fancy_vbox1");
      GtkWidget *fancy_vbox2 = widget_from_builder("shader_settings_fancy_vbox2");
      gtk_widget_set_sensitive(fancy_vbox1, FALSE);
      gtk_widget_set_sensitive(fancy_vbox2, FALSE);
   } else {
      GtkWidget *standard_mode_togglebutton = widget_from_builder("shader_settings_standard_mode_togglebutton");
      GtkWidget *fancy_vbox1 = widget_from_builder("shader_settings_fancy_vbox1");
      GtkWidget *fancy_vbox2 = widget_from_builder("shader_settings_fancy_vbox2");
      bool is_fancy_mode = true;
      if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(standard_mode_togglebutton))) is_fancy_mode = false;
      if (! is_fancy_mode) {
         gtk_widget_set_sensitive(fancy_vbox1, FALSE);
         gtk_widget_set_sensitive(fancy_vbox2, FALSE);
      }
   }
}
