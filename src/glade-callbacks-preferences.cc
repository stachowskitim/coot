/* src/glade-callbacks-preferences.cc
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


#include "Python.h"

#include <iostream>
#include <gtk/gtk.h>

#include "c-interface.h"
#include "c-interface-gtk-widgets.h"
#include "coot-fileselections.h"
#include "positioned-widgets.h"
#include "interface.h"
#include "coot-references.h"

// put preferences functions into their own file, not here.
#include "coot-preferences.h"
#include "c-interface-preferences.h"
#include "rotate-translate-modes.hh"
#include "restraints-editor-c.h"
#include "generic-display-objects-c.h"
#include "c-interface-refmac.h"
#include "gtk-widget-conversion-utils.h"
#include "curlew.h"
#include "read-phs.h"
#include "gtk-manual.h"
#include "c-interface-refine.h"
#include "utils/coot-utils.hh"

#include "widget-from-builder.hh"

// this from callbacks.h (which I don't want to include here)
typedef const char entry_char_type;


extern "C" G_MODULE_EXPORT
void
on_preferences_general_radiotoolbutton_toggled
                                        (GtkToggleButton *toggletoolbutton,
                                         gpointer         user_data) {
  show_hide_preferences_tabs(toggletoolbutton, COOT_GENERAL_PREFERENCES);
}


extern "C" G_MODULE_EXPORT
void
on_preferences_bond_radiotoolbutton_toggled
                                        (GtkToggleButton *toggletoolbutton,
                                        gpointer         user_data)
{
  show_hide_preferences_tabs(toggletoolbutton, COOT_BOND_PREFERENCES);
}


extern "C" G_MODULE_EXPORT
void
on_preferences_map_radiotoolbutton_toggled
                                        (GtkToggleButton *toggletoolbutton,
                                        gpointer         user_data)
{
  show_hide_preferences_tabs(toggletoolbutton, COOT_MAP_PREFERENCES);
}



extern "C" G_MODULE_EXPORT
void
on_preferences_geometry_radiotoolbutton_toggled
                                        (GtkToggleButton *toggletoolbutton,
                                        gpointer         user_data)
{
  show_hide_preferences_tabs(toggletoolbutton, COOT_GEOMETRY_PREFERENCES);
}




extern "C" G_MODULE_EXPORT
void
on_preferences_colour_radiotoolbutton_toggled
                                        (GtkToggleButton *toggletoolbutton,
                                        gpointer         user_data)
{
  show_hide_preferences_tabs(toggletoolbutton, COOT_COLOUR_PREFERENCES);
}




extern "C" G_MODULE_EXPORT
void
on_preferences_other_radiotoolbutton_toggled
                                        (GtkToggleButton *toggletoolbutton,
                                        gpointer         user_data)
{
  show_hide_preferences_tabs(toggletoolbutton, COOT_OTHER_PREFERENCES);
}


extern "C" G_MODULE_EXPORT
void
on_preferences_ok_button_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
   // GtkWidget *w = lookup_widget(GTK_WIDGET(button), "preferences");
  GtkWidget *w = widget_from_preferences_builder("preferences_dialog");
  save_preferences();
  gtk_widget_set_visible(w, FALSE);
  clear_preferences();
}

extern "C" G_MODULE_EXPORT
void
on_preferences_reset_button_clicked    (GtkButton       *button,
                                                            gpointer         user_data)
{
  reset_preferences();
}

extern "C" G_MODULE_EXPORT
void
on_preferences_destroy                 (GtkWidget       *object,
                                        gpointer         user_data)
{
  clear_preferences();
}

void set_use_trackpad(short int state); // or #include cc-interface.hh

extern "C" G_MODULE_EXPORT
void
on_preferences_view_rotation_left_mouse_checkbutton_toggled(GtkCheckButton *checkbutton,
                                                            gpointer         user_data) {
   if (gtk_check_button_get_active(checkbutton)) {
      preferences_internal_change_value_int(PREFERENCES_VIEW_ROTATION_MOUSE_BUTTON, 1);
      set_use_trackpad(1);
   } else {
      preferences_internal_change_value_int(PREFERENCES_VIEW_ROTATION_MOUSE_BUTTON, 0);
      set_use_trackpad(0);
   }
}

extern "C" G_MODULE_EXPORT
void
on_preferences_default_b_factor_entry_activate(GtkEntry        *entry,
                                               gpointer         user_data) {

   const gchar *text = gtk_editable_get_text(GTK_EDITABLE(entry));
   try {
      float f = coot::util::string_to_float(std::string(text));
      set_default_temperature_factor_for_new_atoms(f);
   }
   catch (const std::runtime_error &e) {
      std::cout << "WARNING:: in on_preferences_default_b_factor_entry_activate(): " << e.what() << std::endl;
   }
}


extern "C" G_MODULE_EXPORT
void
on_preferences_bond_colours_hscale_value_changed(GtkRange        *range,
                                                 gpointer         user_data) {

   GtkAdjustment *adjustment;
   float fvalue;
   adjustment = gtk_range_get_adjustment(GTK_RANGE(range));
   fvalue = gtk_adjustment_get_value(adjustment);
   preferences_internal_change_value_float(PREFERENCES_BOND_COLOURS_MAP_ROTATION, fvalue);
   set_colour_map_rotation_on_read_pdb(fvalue);
}

extern "C" G_MODULE_EXPORT
void
on_preferences_bond_colours_checkbutton_toggled(GtkCheckButton *checkbutton,
                                                gpointer         user_data) {

   if (gtk_check_button_get_active(checkbutton)) {
      preferences_internal_change_value_int(PREFERENCES_BOND_COLOUR_ROTATION_C_ONLY, 1);
      set_colour_map_rotation_on_read_pdb_c_only_flag(1);
   } else {
      preferences_internal_change_value_int(PREFERENCES_BOND_COLOUR_ROTATION_C_ONLY, 0);
      set_colour_map_rotation_on_read_pdb_c_only_flag(0);
   }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_bg_colour_black_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                   gpointer         user_data) {

   if (gtk_check_button_get_active(checkbutton)) {
      preferences_internal_change_value_float3(PREFERENCES_BG_COLOUR, 0, 0, 0);
      set_background_colour(0, 0, 0);
   }
}


extern "C" G_MODULE_EXPORT
void
on_preferences_bg_colour_white_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                   gpointer         user_data) {
   if (gtk_check_button_get_active(checkbutton)) {
      preferences_internal_change_value_float3(PREFERENCES_BG_COLOUR, 1, 1, 1);
      set_background_colour(1, 1, 1);
   }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_bg_colour_own_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

#if (GTK_MAJOR_VERSION == 3 && GTK_MINOR_VERSION == 94) || (GTK_MAJOR_VERSION == 4)
  GdkRGBA bg_colour;
  // 20220528-PE more here
#else
  GdkColor bg_colour;
  float fval1;
  float fval2;
  float fval3;
  // GtkWidget *w = lookup_widget(GTK_WIDGET(togglebutton), "preferences_bg_colour_colorbutton");
  GtkWidget *w = widget_from_preferences_builder("preferences_bg_colour_colorbutton");
  gtk_color_button_get_color(GTK_COLOR_BUTTON(w), &bg_colour);
  fval1 = (float)bg_colour.red / 65535;
  fval2 = (float)bg_colour.green / 65535;
  fval3 = (float)bg_colour.blue / 65535;

  preferences_internal_change_value_float3(PREFERENCES_BG_COLOUR, fval1, fval2, fval3);
  set_background_colour(fval1, fval2, fval3);
#endif

}


extern "C" G_MODULE_EXPORT
void
on_preferences_bg_colour_colorbutton_color_set(GtkColorButton  *colorbutton,
                                               gpointer         user_data) {

#if (GTK_MAJOR_VERSION == 3 && GTK_MINOR_VERSION == 94) || (GTK_MAJOR_VERSION == 4)
#else
   // GtkWidget *w = lookup_widget(GTK_WIDGET(colorbutton), "preferences_bg_colour_own_radiobutton");
   GtkWidget *w = widget_from_preferences_builder("preferences_bg_colour_own_radiobutton");
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w))) {
      GdkColor bg_colour;
      float fval1;
      float fval2;
      float fval3;
      gtk_color_button_get_color(colorbutton, &bg_colour);
      fval1 = (float)bg_colour.red / 65535;
      fval2 = (float)bg_colour.green / 65535;
      fval3 = (float)bg_colour.blue / 65535;

      preferences_internal_change_value_float3(PREFERENCES_BG_COLOUR, fval1, fval2, fval3);
      set_background_colour(fval1, fval2, fval3);
   }
#endif

}


extern "C" G_MODULE_EXPORT
void
on_preferences_bg_colour_colorbutton_clicked(GtkButton       *button,
                                             gpointer         user_data) {

   // GtkWidget *w = lookup_widget(GTK_WIDGET(button), "preferences_bg_colour_own_radiobutton");
   GtkWidget *w = widget_from_preferences_builder("preferences_bg_colour_own_radiobutton");
   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), TRUE);

}



extern "C" G_MODULE_EXPORT
void
on_preferences_map_radius_entry_activate(GtkEntry        *entry,
					 gpointer         user_data) {

   const gchar *text = gtk_editable_get_text(GTK_EDITABLE(entry));
   float fval = 0;
   fval = atof(text);
   if ((fval > 0) && (fval <1000)) {
      preferences_internal_change_value_float(PREFERENCES_MAP_RADIUS, fval);
      set_map_radius(fval);
   }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_map_radius_entry_changed(GtkEditable     *editable,
                                        gpointer         user_data) {

   //GtkEntry *entry = GTK_ENTRY(lookup_widget(GTK_WIDGET(editable), "preferences_map_radius_entry"));
   GtkEntry *entry = GTK_ENTRY(widget_from_preferences_builder("preferences_map_radius_entry"));
   std::cout << "debug:: on_preferences_map_radius_entry_changed() entry " << entry << std::endl;
   if (entry) {
      const gchar *text = gtk_editable_get_text(GTK_EDITABLE(entry));
      float fval = 0;
      fval = atof(text);
      if ((fval > 0) && (fval <200)) {
         preferences_internal_change_value_float(PREFERENCES_MAP_RADIUS, fval);
         set_map_radius(fval);
      }
   } else {
      std::cout << "ERROR:: in on_preferences_map_radius_entry_changed() failed to lookup "
                << "preferences_map_radius_entry" << std::endl;
   }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_map_increment_size_entry_activate(GtkEntry        *entry,
                                                 gpointer         user_data) {
   const gchar *text = gtk_editable_get_text(GTK_EDITABLE(entry));
   float fval = 0;
   fval = atof(text);
   if (fval > 0) {
      preferences_internal_change_value_float(PREFERENCES_MAP_ISOLEVEL_INCREMENT, fval);
      set_iso_level_increment(fval);
   }
}


extern "C" G_MODULE_EXPORT
void
on_preferences_map_increment_size_entry_changed(GtkEditable     *editable,
                                                gpointer         user_data) {

   GtkEntry *entry = GTK_ENTRY(widget_from_preferences_builder("preferences_map_increment_size_entry"));
   const gchar *text = gtk_editable_get_text(GTK_EDITABLE(entry));
   try {
      float fval = coot::util::string_to_float(text);
      if (fval > 0) {
         preferences_internal_change_value_float(PREFERENCES_MAP_ISOLEVEL_INCREMENT, fval);
         set_iso_level_increment(fval);
      }
   }
   catch (const std::runtime_error &e) {
      std::cout << "WARNING::" << e.what() << std::endl;
   }
}


extern "C" G_MODULE_EXPORT
void
on_preferences_map_diff_increment_entry_activate(GtkEntry        *entry,
                                                 gpointer         user_data) {

   const gchar *text = gtk_editable_get_text(GTK_EDITABLE(entry));
   float fval = 0;
   fval = atof(text);
   if (fval > 0) {
      preferences_internal_change_value_float(PREFERENCES_DIFF_MAP_ISOLEVEL_INCREMENT, fval);
      set_diff_map_iso_level_increment(fval);
   }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_map_diff_increment_entry_changed
                                        (GtkEditable     *editable,
                                        gpointer         user_data)
{
   // GtkEntry *entry = GTK_ENTRY(lookup_widget(GTK_WIDGET(editable), "preferences_map_diff_increment_entry"));
   GtkEntry *entry = GTK_ENTRY(widget_from_preferences_builder("preferences_map_diff_increment_entry"));
   const gchar *text = gtk_editable_get_text(GTK_EDITABLE(entry));
   float fval = 0;
   fval = atof(text);
   if (fval > 0) {
      preferences_internal_change_value_float(PREFERENCES_DIFF_MAP_ISOLEVEL_INCREMENT, fval);
      set_diff_map_iso_level_increment(fval);
   }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_map_sampling_entry_activate
                                        (GtkEntry        *entry,
                                        gpointer         user_data)
{
   const gchar *text = gtk_editable_get_text(GTK_EDITABLE(entry));
  float fval = 0;
  fval = atof(text);
  if ((fval < 100) && (fval > 1)) {
    preferences_internal_change_value_float(PREFERENCES_MAP_SAMPLING_RATE, fval);
    set_map_sampling_rate(fval);
  }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_map_sampling_entry_changed
                                        (GtkEditable     *editable,
                                        gpointer         user_data)
{
   // GtkEntry *entry = GTK_ENTRY(lookup_widget(GTK_WIDGET(editable), "preferences_map_sampling_entry"));
   GtkEntry *entry = GTK_ENTRY(widget_from_preferences_builder("preferences_map_sampling_entry"));
   const gchar *text = gtk_editable_get_text(GTK_EDITABLE(entry));
   float fval = 0;
   fval = atof(text);
   if ((fval < 100) && (fval > 1)) {
      preferences_internal_change_value_float(PREFERENCES_MAP_SAMPLING_RATE, fval);
      set_map_sampling_rate(fval);
   }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_map_dynamic_sampling_checkbutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  if (gtk_toggle_button_get_active(togglebutton)) {
    preferences_internal_change_value_int(PREFERENCES_DYNAMIC_MAP_SAMPLING, 1);
    set_dynamic_map_sampling_on();
  } else {
    preferences_internal_change_value_int(PREFERENCES_DYNAMIC_MAP_SAMPLING, 0);
    set_dynamic_map_sampling_off();
  }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_map_dynamic_size_checkbutton_toggled(GtkCheckButton *checkbutton,
                                                    gpointer         user_data) {
   if (gtk_check_button_get_active(checkbutton)) {
      preferences_internal_change_value_int(PREFERENCES_DYNAMIC_MAP_SIZE_DISPLAY, 1);
      set_dynamic_map_size_display_on();
   } else {
      preferences_internal_change_value_int(PREFERENCES_DYNAMIC_MAP_SIZE_DISPLAY, 0);
      set_dynamic_map_size_display_off();
   }
}


extern "C" G_MODULE_EXPORT
void
on_preferences_diff_map_colours_coot_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                         gpointer        user_data) {
   if (gtk_check_button_get_active(checkbutton)) {
      preferences_internal_change_value_int(PREFERENCES_SWAP_DIFF_MAP_COLOURS, 0);
      set_swap_difference_map_colours(0);
   }
}


extern "C" G_MODULE_EXPORT
void
on_preferences_diff_map_colours_o_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                      gpointer         user_data) {
   if (gtk_check_button_get_active(checkbutton)) {
      preferences_internal_change_value_int(PREFERENCES_SWAP_DIFF_MAP_COLOURS, 1);
      set_swap_difference_map_colours(1);
   }
}


extern "C" G_MODULE_EXPORT
void
on_preferences_map_colours_hscale_value_changed(GtkRange        *range,
                                                gpointer         user_data) {
   GtkAdjustment *adjustment;
   float fvalue;
   adjustment = gtk_range_get_adjustment(GTK_RANGE(range));
   fvalue = gtk_adjustment_get_value(adjustment);
   preferences_internal_change_value_float(PREFERENCES_MAP_COLOURS_MAP_ROTATION, fvalue);
   set_colour_map_rotation_for_map(fvalue);
}


extern "C" G_MODULE_EXPORT
void
on_preferences_smooth_scroll_on_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                    gpointer        user_data) {

   if (gtk_check_button_get_active(checkbutton)) {
      preferences_internal_change_value_int(PREFERENCES_SMOOTH_SCROLL, 1);
      set_smooth_scroll_flag(1);
   }
}


extern "C" G_MODULE_EXPORT
void
on_preferences_smooth_scroll_off_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                     gpointer        user_data) {

   if (gtk_check_button_get_active(checkbutton)) {
      preferences_internal_change_value_int(PREFERENCES_SMOOTH_SCROLL, 0);
      set_smooth_scroll_flag(0);
  }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_smooth_scroll_steps_entry_activate(GtkEntry        *entry,
                                                  gpointer         user_data) {
   const gchar *text = gtk_editable_get_text(GTK_EDITABLE(entry));
   try {
      int ival = coot::util::string_to_int(text);
      if ((ival < 100000) && (ival > 0)) {
         preferences_internal_change_value_int(PREFERENCES_SMOOTH_SCROLL_STEPS, ival);
         std::cout << "EPH set_smooth_scroll " << ival << std::endl;
         set_smooth_scroll_steps(ival);
      }
   }
   catch (const std::runtime_error &e) {
      std::cout << "WARNING::" << e.what() << std::endl;
   }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_smooth_scroll_steps_entry_changed(GtkEditable     *editable,
                                                 gpointer         user_data) {

   const gchar *text = gtk_editable_get_text(editable);
   try {
      int ival = coot::util::string_to_int(text);
      if ((ival < 100000) && (ival > 0)) {
         preferences_internal_change_value_int(PREFERENCES_SMOOTH_SCROLL_STEPS, ival);
         std::cout << "EPH set_smooth_scroll " << ival << std::endl;
         set_smooth_scroll_steps(ival);
      }
   }
   catch (const std::runtime_error &e) {
      std::cout << "WARNING::" << e.what() << std::endl;
   }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_smooth_scroll_limit_entry_activate(GtkEntry        *entry,
                                                  gpointer         user_data) {

   const gchar *text = gtk_editable_get_text(GTK_EDITABLE(entry));

   try {
      float fval = coot::util::string_to_float(std::string(text));
      if ((fval < 1000) && (fval > 0)) {
         preferences_internal_change_value_float(PREFERENCES_SMOOTH_SCROLL_LIMIT, fval);
         std::cout << "EPH set smooth scroll imit " << fval << std::endl;
         set_smooth_scroll_limit(fval);
      }
   }
   catch (const std::runtime_error &e) {
      std::cout << "WARNING::" << e.what() << std::endl;
   }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_smooth_scroll_limit_entry_changed
                                        (GtkEditable     *editable,
                                        gpointer         user_data)
{
   // GtkEntry *entry = GTK_ENTRY(lookup_widget(GTK_WIDGET(editable), "preferences_smooth_scroll_limit_entry"));
   GtkEntry *entry = GTK_ENTRY(widget_from_preferences_builder("preferences_smooth_scroll_limit_entry"));
   const gchar *text = gtk_editable_get_text(GTK_EDITABLE(entry));
   float fval = 0;
   fval = atof(text);
   if ((fval < 1000) && (fval > 0)) {
      preferences_internal_change_value_float(PREFERENCES_SMOOTH_SCROLL_LIMIT, fval);
      set_smooth_scroll_limit(fval);
   }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_map_drag_on_radiobutton_toggled(GtkCheckButton *checkbutton,
                                               gpointer         user_data) {

   if (gtk_check_button_get_active(checkbutton)) {
      preferences_internal_change_value_int(PREFERENCES_MAP_DRAG, 1);
      set_active_map_drag_flag(1);
   }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_map_drag_off_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                gpointer         user_data) {

   if (gtk_check_button_get_active(checkbutton)) {
      preferences_internal_change_value_int(PREFERENCES_MAP_DRAG, 0);
      set_active_map_drag_flag(0);
   }
}



extern "C" G_MODULE_EXPORT
void
on_preferences_hid_spherical_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                 gpointer         user_data) {

   if (gtk_check_button_get_active(checkbutton)) {
      preferences_internal_change_value_int(PREFERENCES_VT_SURFACE, 2);
      vt_surface(2);
   }
}


extern "C" G_MODULE_EXPORT
void
on_preferences_hid_flat_radiobutton_toggled(GtkCheckButton *checkbutton,
                                            gpointer         user_data) {

   if (gtk_check_button_get_active(checkbutton)) {
      preferences_internal_change_value_int(PREFERENCES_VT_SURFACE, 1);
      vt_surface(1);
   }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_filechooser_off_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  if (gtk_toggle_button_get_active(togglebutton)) {
    preferences_internal_change_value_int(PREFERENCES_FILE_CHOOSER, 0);
    set_file_chooser_selector(0);
  }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_filechooser_on_radiobutton_toggled(GtkToggleButton *togglebutton,
                                                  gpointer         user_data) {

   if (gtk_toggle_button_get_active(togglebutton)) {
      preferences_internal_change_value_int(PREFERENCES_FILE_CHOOSER, 1);
      set_file_chooser_selector(1);
  }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_file_overwrite_yes_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

  if (gtk_toggle_button_get_active(togglebutton)) {
    preferences_internal_change_value_int(PREFERENCES_FILE_OVERWRITE, 1);
    set_file_chooser_overwrite(1);
  }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_file_overwrite_no_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

  if (gtk_toggle_button_get_active(togglebutton)) {
    preferences_internal_change_value_int(PREFERENCES_FILE_OVERWRITE, 0);
    set_file_chooser_overwrite(0);
  }

}

extern "C" G_MODULE_EXPORT
void
on_preferences_file_filter_on_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  if (gtk_toggle_button_get_active(togglebutton)) {
    preferences_internal_change_value_int(PREFERENCES_FILE_FILTER, 1);
    set_filter_fileselection_filenames(1);
  }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_file_filter_off_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  if (gtk_toggle_button_get_active(togglebutton)) {
    preferences_internal_change_value_int(PREFERENCES_FILE_FILTER, 0);
    set_filter_fileselection_filenames(0);
  }

}

extern "C" G_MODULE_EXPORT
void
on_preferences_file_sort_by_date_on_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  if (gtk_toggle_button_get_active(togglebutton)) {
    preferences_internal_change_value_int(PREFERENCES_FILE_SORT_DATE, 1);
    set_sticky_sort_by_date();
  }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_file_sort_by_date_off_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  if (gtk_toggle_button_get_active(togglebutton)) {
    preferences_internal_change_value_int(PREFERENCES_FILE_SORT_DATE, 0);
    unset_sticky_sort_by_date();
  }

}

extern "C" G_MODULE_EXPORT
void
on_preferences_dialog_accept_docked_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
   // GtkWidget *hbox             = lookup_widget(GTK_WIDGET(togglebutton), "preferences_dialog_accept_docked_hbox");
   // GtkWidget *show_checkbutton = lookup_widget(GTK_WIDGET(togglebutton), "preferences_dialog_accept_docked_show_radiobutton");
   // GtkWidget *hide_checkbutton = lookup_widget(GTK_WIDGET(togglebutton), "preferences_dialog_accept_docked_hide_radiobutton");
   GtkWidget *hbox             = widget_from_preferences_builder("preferences_dialog_accept_docked_hbox");
   GtkWidget *show_checkbutton = widget_from_preferences_builder("preferences_dialog_accept_docked_show_radiobutton");
   GtkWidget *hide_checkbutton = widget_from_preferences_builder("preferences_dialog_accept_docked_hide_radiobutton");
   if (gtk_toggle_button_get_active(togglebutton)) {
      preferences_internal_change_value_int(PREFERENCES_ACCEPT_DIALOG_DOCKED, 1);
      set_accept_reject_dialog_docked(1);
      /* shall update the hbox */
      if (accept_reject_dialog_docked_show_state() == 1) {
         gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(show_checkbutton), TRUE);
      } else {
         gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(hide_checkbutton), TRUE);
      }
      gtk_widget_set_visible(hbox, TRUE);
   }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_dialog_accept_detouched_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
   // GtkWidget *hbox = lookup_widget(GTK_WIDGET(togglebutton), "preferences_dialog_accept_docked_hbox");
   GtkWidget *hbox = widget_from_preferences_builder("preferences_dialog_accept_docked_hbox");
   if (gtk_toggle_button_get_active(togglebutton)) {
      preferences_internal_change_value_int(PREFERENCES_ACCEPT_DIALOG_DOCKED, 0);
      set_accept_reject_dialog_docked(0);
      /* shall update the hbox */
      if (accept_reject_dialog_docked_show_state() == 1) {
         set_accept_reject_dialog_docked_show(0);
      }
      gtk_widget_set_visible(hbox, FALSE);
   }

}

extern "C" G_MODULE_EXPORT
void
on_preferences_dialog_accept_docked_show_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  if (gtk_toggle_button_get_active(togglebutton)) {
    preferences_internal_change_value_int(PREFERENCES_ACCEPT_DIALOG_DOCKED_SHOW, 1);
    set_accept_reject_dialog_docked_show(1);
  }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_dialog_accept_docked_hide_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  if (gtk_toggle_button_get_active(togglebutton)) {
    preferences_internal_change_value_int(PREFERENCES_ACCEPT_DIALOG_DOCKED_SHOW, 0);
    set_accept_reject_dialog_docked_show(0);
  }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_dialog_accept_on_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  if (gtk_toggle_button_get_active(togglebutton)) {
    preferences_internal_change_value_int(PREFERENCES_IMMEDIATE_REPLACEMENT, 0);
    set_refinement_immediate_replacement(0);
  }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_dialog_accept_off_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  if (gtk_toggle_button_get_active(togglebutton)) {
    preferences_internal_change_value_int(PREFERENCES_IMMEDIATE_REPLACEMENT, 1);
    set_refinement_immediate_replacement(1);
  }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_recentre_pdb_on_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                   gpointer         user_data) {

   if (gtk_check_button_get_active(checkbutton)) {
      preferences_internal_change_value_int(PREFERENCES_RECENTRE_PDB, 1);
      set_recentre_on_read_pdb(1);
   }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_recentre_pdb_off_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                    gpointer         user_data) {

   if (gtk_check_button_get_active(checkbutton)) {
      preferences_internal_change_value_int(PREFERENCES_RECENTRE_PDB, 0);
      set_recentre_on_read_pdb(0);
   }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_console_info_on_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                   gpointer         user_data) {

   if (gtk_check_button_get_active(checkbutton)) {
      preferences_internal_change_value_int(PREFERENCES_CONSOLE_COMMANDS, 1);
      set_console_display_commands_state(1);
   }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_console_info_off_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                    gpointer         user_data) {

   // just do this in the above radio button callback - it's 2-state

   if (gtk_check_button_get_active(checkbutton)) {
      preferences_internal_change_value_int(PREFERENCES_CONSOLE_COMMANDS, 0);
      set_console_display_commands_state(0);
  }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_tips_on_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  if (gtk_toggle_button_get_active(togglebutton)) {
    preferences_internal_change_value_int(PREFERENCES_TIPS, 1);
    set_tip_of_the_day_flag(1);
  }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_tips_off_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  if (gtk_toggle_button_get_active(togglebutton)) {
    preferences_internal_change_value_int(PREFERENCES_TIPS, 0);
    set_tip_of_the_day_flag(0);
  }

}



extern "C" G_MODULE_EXPORT
void
on_preferences_spin_speed_entry_activate
                                        (GtkEntry        *entry,
                                        gpointer         user_data)
{
  float fval;
  const gchar* entry_text = gtk_editable_get_text(GTK_EDITABLE(entry));
  fval = atof(entry_text);
  if ((fval > 360) || (fval < 0)) {
    printf("Cannot interpret: %s Assuming default 1.0 \n", entry_text);
    fval  = 1.0;
    gtk_editable_set_text(GTK_EDITABLE(entry), "1.0");
  }
  preferences_internal_change_value_float(PREFERENCES_SPIN_SPEED, fval);
  set_idle_function_rotate_angle(fval);
}


extern "C" G_MODULE_EXPORT
void
on_preferences_spin_speed_entry_changed
                                        (GtkEditable     *editable,
                                        gpointer         user_data)
{
   float fval;
   GtkWidget *w = widget_from_preferences_builder("preferences_spin_speed_entry");
   const gchar* entry_text = gtk_editable_get_text(GTK_EDITABLE(w));
   fval = atof(entry_text);
   if ((fval > 360) || (fval < 0)) {
      printf("Cannot interpret: %s Assuming default 1.0 \n", entry_text);
      fval  = 1.0;
      gtk_editable_set_text(GTK_EDITABLE(w), "1.0");
   }
   preferences_internal_change_value_float(PREFERENCES_SPIN_SPEED, fval);
   set_idle_function_rotate_angle(fval);
}


extern "C" G_MODULE_EXPORT
void
on_preferences_font_size_small_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  if (gtk_toggle_button_get_active(togglebutton)) {
    preferences_internal_change_value_int(PREFERENCES_FONT_SIZE, 1);
    set_font_size(1);
  }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_font_size_medium_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  if (gtk_toggle_button_get_active(togglebutton)) {
    preferences_internal_change_value_int(PREFERENCES_FONT_SIZE, 2);
    set_font_size(2);
  }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_font_size_large_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  if (gtk_toggle_button_get_active(togglebutton)) {
    preferences_internal_change_value_int(PREFERENCES_FONT_SIZE, 3);
    set_font_size(3);
  }

}

extern "C" G_MODULE_EXPORT
void
on_preferences_font_size_others_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

   if (gtk_toggle_button_get_active(togglebutton)) {
      GtkWidget *w = widget_from_preferences_builder("preferences_font_size_combobox");
      gint ival = gtk_combo_box_get_active(GTK_COMBO_BOX(w));
      ival += 4;
      preferences_internal_change_value_int(PREFERENCES_FONT_SIZE, ival);
      set_font_size(ival);
   }
}

extern "C" G_MODULE_EXPORT
void
on_preferences_font_size_combobox_changed
                                        (GtkComboBox     *combobox,
                                        gpointer         user_data)
{
  GtkWidget *w = widget_from_preferences_builder("preferences_font_size_others_radiobutton");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), TRUE);
  gint ival = gtk_combo_box_get_active(combobox);
  ival += 4;
  preferences_internal_change_value_int(PREFERENCES_FONT_SIZE, ival);
  set_font_size(ival);

}

extern "C" G_MODULE_EXPORT
void
on_preferences_font_colour_default_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                       gpointer         user_data) {

   if (gtk_check_button_get_active(checkbutton)) {
      preferences_internal_change_value_float3(PREFERENCES_FONT_COLOUR, 1.0, 0.8, 0.8);
      set_font_colour(1.0, 0.8, 0.8);
   }
}


extern "C" G_MODULE_EXPORT
void
on_preferences_font_colour_own_radiobutton_toggled(GtkCheckButton *checkbutton,
                                                   gpointer         user_data) {

   // 20230716-PE There is no function to set the colour of the colorbutton if
   // the button is in the .ui file (which it is in this case (we look up the
   // widget from the name)).

   GdkRGBA font_colour;
   float fval1;
   float fval2;
   float fval3;
   int previous_state;

   if (gtk_check_button_get_active(checkbutton)) {
      previous_state = preferences_internal_font_own_colour_flag();
      if (previous_state != -1) { 	/* not unset */
         GtkWidget *colorbutton = widget_from_preferences_builder("preferences_font_colorbutton");
         GdkRGBA col;
         gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(colorbutton), &col);
         set_font_colour(col.red, col.green, col.blue);
      }
   }

}



extern "C" G_MODULE_EXPORT
void
on_preferences_font_colorbutton_color_set (GtkColorButton  *colorbutton,
                                           gpointer         user_data) {

   GdkRGBA col;
   // 20230716-PE change to GtkColorDialogButton when moving to 4.12
   gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(colorbutton), &col);
   // std::cout << "col: " << col.red << " " << col.green << " " << col.blue << " " << col.alpha << std::endl;
   preferences_internal_change_value_int(PREFERENCES_FONT_OWN_COLOUR_FLAG, 1); // checked in above function
   set_font_colour(col.red, col.green, col.blue);
}

extern "C" G_MODULE_EXPORT
void
on_preferences_font_colorbutton_clicked (GtkButton       *button,
                                         gpointer         user_data)
{
  /* actually not doing anything */
}


extern "C" G_MODULE_EXPORT
void
on_preferences_pink_pointer_entry_activate
                                        (GtkEntry        *entry,
                                        gpointer         user_data)
{
  float fval;
  const gchar* entry_text = gtk_editable_get_text(GTK_EDITABLE(entry));
  fval = atof(entry_text);
  if ((fval > 1000) || (fval < 0)) {
    printf("Invalid cube size: %s Assuming default 0.1 A \n", entry_text);
    fval  = 0.1;
    gtk_editable_set_text(GTK_EDITABLE(entry), "0.1");
  }
  preferences_internal_change_value_float(PREFERENCES_PINK_POINTER, fval);
  set_rotation_centre_size(fval);
}


extern "C" G_MODULE_EXPORT
void
on_preferences_pink_pointer_entry_changed
                                        (GtkEditable     *editable,
                                        gpointer         user_data)
{
  float fval;
  GtkWidget *w = widget_from_preferences_builder("preferences_pink_pointer_entry");
  const gchar* entry_text = gtk_editable_get_text(GTK_EDITABLE(w));
  fval = atof(entry_text);
  if ((fval > 1000) || (fval < 0)) {
    printf("Invalid cube size: %s Assuming default 0.1 A \n", entry_text);
    fval  = 0.1;
    gtk_editable_set_text(GTK_EDITABLE(w), "0.1");
  }
  preferences_internal_change_value_float(PREFERENCES_PINK_POINTER, fval);
  set_rotation_centre_size(fval);
}


extern "C" G_MODULE_EXPORT
void
on_preferences_bond_width_combobox_changed
                                        (GtkComboBox     *combobox,
                                        gpointer         user_data)
{
  gint val;
  val = gtk_combo_box_get_active(combobox);
  val += 1;  /* offset */
  preferences_internal_change_value_int(PREFERENCES_BONDS_THICKNESS, val);
  set_default_bond_thickness(val);
}


extern "C" G_MODULE_EXPORT
void
on_preferences_main_toolbar_show_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  if (gtk_toggle_button_get_active(togglebutton)) {
    show_main_toolbar();
    preferences_internal_change_value_int(PREFERENCES_MAIN_TOOLBAR_SHOW, 1);
  }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_main_toolbar_hide_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  if (gtk_toggle_button_get_active(togglebutton)) {
    hide_main_toolbar();
    preferences_internal_change_value_int(PREFERENCES_MAIN_TOOLBAR_SHOW, 0);
  }

}


/* not used currently */
extern "C" G_MODULE_EXPORT
void
on_preferences_main_toolbar_top_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


extern "C" G_MODULE_EXPORT
void
on_preferences_main_toolbar_bottom_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


extern "C" G_MODULE_EXPORT
void
on_preferences_main_toolbar_right_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


extern "C" G_MODULE_EXPORT
void
on_preferences_main_toolbar_left_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


extern "C" G_MODULE_EXPORT
void
on_preferences_main_toolbar_style_icons_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  if (gtk_toggle_button_get_active(togglebutton)) {
    preferences_internal_change_value_int(PREFERENCES_MAIN_TOOLBAR_STYLE, 1);
    set_main_toolbar_style(1);
  }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_main_toolbar_style_both_radiobutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  if (gtk_toggle_button_get_active(togglebutton)) {
    preferences_internal_change_value_int(PREFERENCES_MAIN_TOOLBAR_STYLE, 2);
    set_main_toolbar_style(2);
  }

}


extern "C" G_MODULE_EXPORT
void
on_preferences_main_toolbar_style_text_radiobutton_toggled(GtkToggleButton *togglebutton,
                                                           gpointer         user_data) {
  if (gtk_toggle_button_get_active(togglebutton)) {
    preferences_internal_change_value_int(PREFERENCES_MAIN_TOOLBAR_STYLE, 3);
    set_main_toolbar_style(3);
  }

}

extern "C" G_MODULE_EXPORT
void
noughties_physics_switch_state_set(GtkSwitch *switch_widget,
                                   gboolean   state,
                                   gpointer   user_data) {

   if (state)
      set_show_unit_cells_all(1);
  else
     set_show_unit_cells_all(0);
}

extern "C" G_MODULE_EXPORT
void
on_noughties_physics_checkbutton_toggled(GtkCheckButton *toggletoolbutton,
                                         gpointer         user_data) {

   if (gtk_check_button_get_active(toggletoolbutton))
      set_refine_use_noughties_physics(1);
   else
      set_refine_use_noughties_physics(0);
}
