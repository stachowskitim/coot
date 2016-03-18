/* src/graphics-info-preferences.cc
 * 
 * Copyright 2008 The University of York
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.,  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#ifdef USE_PYTHON
#include "Python.h"  // before system includes to stop "POSIX_C_SOURCE" redefined problems
#endif

#include "compat/coot-sysdep.h"


#if defined _MSC_VER
#include <windows.h>
#endif

#include <fstream>

#include <gtk/gtk.h>
#include "interface.h"
#include "graphics-info.h"
#include "c-interface.h"
#include "c-interface-preferences.h"
#include "cc-interface.hh"
#include "c-interface-scm.hh"
#include "coot-preferences.h"
#include "utils/coot-utils.hh"


// Return success status
//
// il - interface language
//  2 - python
//  1 - guile
short int
graphics_info_t::save_preference_file(const std::string &filename, short int il) {

   short int istat = 1;
   //std::cout << "DEBUG:: writing preferences" << std::endl;
   std::vector<std::string> commands;

   std::string comment_str;
   if (il == 1) { 
      comment_str = "; These commands are the preferences of coot.  You can evaluate them\n";
      comment_str += "; using \"Calculate->Run Script...\".\n";
      comment_str += "; DO NOT ADD COMMANDS TO THIS FILE - it is generated by Coot\n";
      comment_str += "; BUT feel free to edit the setting\n";
   } else { 
      comment_str = "# These commands are the preferences of coot.  You can evaluate them\n";
      comment_str += "# using \"Calculate->Run Script...\".\n";
      comment_str += "# DO NOT ADD COMMANDS TO THIS FILE - it is generated by Coot\n";
      comment_str += "# BUT feel free to edit the setting\n";
   }
   commands.push_back(comment_str);
   
   unsigned short int v = 4;
   int preference_type;
   float fval1;
   float fval2;
   float fval3;
   std::vector<int> ivector;
   graphics_info_t g;
   for (unsigned int i=0; i<g.preferences_internal.size(); i++) {
     preference_type = g.preferences_internal[i].preference_type;
     switch (preference_type) {

     case PREFERENCES_FILE_CHOOSER:
       commands.push_back(state_command("set-file-chooser-selector",
					g.preferences_internal[i].ivalue1, il));
       break;

     case PREFERENCES_FILE_OVERWRITE:
       commands.push_back(state_command("set-file-chooser-overwrite",
					g.preferences_internal[i].ivalue1, il));
       break;
       
     case PREFERENCES_FILE_FILTER:
       commands.push_back(state_command("set-filter-fileselection-filenames", 
					g.preferences_internal[i].ivalue1, il));
       break;
       
     case PREFERENCES_FILE_SORT_DATE:
       if (g.preferences_internal[i].ivalue1 == 1) {
	 commands.push_back(state_command("set-sticky-sort-by-date", il));
       } else {
	 commands.push_back(state_command("unset-sticky-sort-by-date", il));
       }
       break;
       
     case PREFERENCES_ACCEPT_DIALOG_DOCKED:
       commands.push_back(state_command("set-accept-reject-dialog-docked",
					g.preferences_internal[i].ivalue1, il));
       break;

     case PREFERENCES_ACCEPT_DIALOG_DOCKED_SHOW:
       commands.push_back(state_command("set-accept-reject-dialog-docked-show",
					g.preferences_internal[i].ivalue1, il));
       break;
       
     case PREFERENCES_IMMEDIATE_REPLACEMENT:
       commands.push_back(state_command("set-refinement-immediate-replacement",
					g.preferences_internal[i].ivalue1, il));
      break;
      
     case PREFERENCES_VT_SURFACE:
       commands.push_back(state_command("vt-surface",
					g.preferences_internal[i].ivalue1, il));
       break;

     case PREFERENCES_RECENTRE_PDB:
       commands.push_back(state_command("set-recentre-on-read-pdb",
					g.preferences_internal[i].ivalue1, il));
       break;

     case PREFERENCES_BONDS_THICKNESS:
       commands.push_back(state_command("set-default-bond-thickness",
					g.preferences_internal[i].ivalue1, il));
       break;
       
     case PREFERENCES_BOND_COLOURS_MAP_ROTATION:
       commands.push_back(state_command("set-colour-map-rotation-on-read-pdb",
					g.preferences_internal[i].fvalue1, il));
       break;
       
     case PREFERENCES_BOND_COLOUR_ROTATION_C_ONLY:
       commands.push_back(state_command("set-colour-map-rotation-on-read-pdb-c-only-flag",
					g.preferences_internal[i].ivalue1, il));
       break;
       
     case PREFERENCES_MAP_RADIUS:
       commands.push_back(state_command("set-map-radius",
					 g.preferences_internal[i].fvalue1, il));
       break;
       
     case PREFERENCES_MAP_ISOLEVEL_INCREMENT:
       commands.push_back(state_command("set-iso-level-increment",
					 g.preferences_internal[i].fvalue1, il, v));
       break;

     case PREFERENCES_DIFF_MAP_ISOLEVEL_INCREMENT:
       commands.push_back(state_command("set-diff-map-iso-level-increment",
					 g.preferences_internal[i].fvalue1, il, v));
       break;

     case PREFERENCES_MAP_SAMPLING_RATE:
       commands.push_back(state_command("set-map-sampling-rate",
					 g.preferences_internal[i].fvalue1, il, v));
       break;

     case PREFERENCES_DYNAMIC_MAP_SAMPLING:
       if (g.preferences_internal[i].ivalue1 == 1) {
	 commands.push_back(state_command("set-dynamic-map-sampling-on", il));
       } else {
	 commands.push_back(state_command("set-dynamic-map-sampling-off", il));
       }
       break;
      
     case PREFERENCES_DYNAMIC_MAP_SIZE_DISPLAY:
       if (g.preferences_internal[i].ivalue1 == 1) {
	 commands.push_back(state_command("set-dynamic-map-size-display-on", il));
       } else {
	 commands.push_back(state_command("set-dynamic-map-size-display-off", il));
       }
       break;

     case PREFERENCES_SWAP_DIFF_MAP_COLOURS:
       commands.push_back(state_command("set-swap-difference-map-colours",
					g.preferences_internal[i].ivalue1, il));
       break;

     case PREFERENCES_MAP_COLOURS_MAP_ROTATION:
       commands.push_back(state_command("set-colour-map-rotation-for-map",
					g.preferences_internal[i].fvalue1, il));
       break;

     case PREFERENCES_SMOOTH_SCROLL:
       commands.push_back(state_command("set-smooth-scroll-flag",
					g.preferences_internal[i].ivalue1, il));
       break;

     case PREFERENCES_SMOOTH_SCROLL_STEPS:
       commands.push_back(state_command("set-smooth-scroll-steps",
					g.preferences_internal[i].ivalue1, il));
       break;

     case PREFERENCES_SMOOTH_SCROLL_LIMIT:
       commands.push_back(state_command("set-smooth-scroll-limit",
					g.preferences_internal[i].fvalue1, il));
       break;

     case PREFERENCES_MAP_DRAG:
       commands.push_back(state_command("set-active-map-drag-flag",
					g.preferences_internal[i].ivalue1, il));
       break;

     case PREFERENCES_MARK_CIS_BAD:
       commands.push_back(state_command("set-mark-cis-peptides-as-bad",
					g.preferences_internal[i].ivalue1, il));     
       break;

     case PREFERENCES_BG_COLOUR:      
       fval1 = g.preferences_internal[i].fvalue1;  // red
       fval2 = g.preferences_internal[i].fvalue2;  // green
       fval3 = g.preferences_internal[i].fvalue3;  // blue
       if (fval1 < 0.01 && fval2 < 0.01 && fval3 < 0.01) {
	 // black
	 commands.push_back(state_command("set-background-colour", 0, 0, 0, il));
       } else if (fval1 > 0.99 && fval2 > 0.99 && fval3 > 0.99) {
	 // white
	 commands.push_back(state_command("set-background-colour", 1, 1, 1, il));
       } else {
	  // other colour
	  std::cout << "on save other colour: " << fval1 << " " << fval2 << " " << fval3
		    << std::endl;
	  commands.push_back(state_command("set-background-colour", fval1, fval2, fval3, il));
       }
       break;

     case PREFERENCES_ANTIALIAS:
       commands.push_back(state_command("set-do-anti-aliasing",
					g.preferences_internal[i].ivalue1, il));
       break;

     case PREFERENCES_CONSOLE_COMMANDS:
       commands.push_back(state_command("set-console-display-commands-state",
					g.preferences_internal[i].ivalue1, il));
       break;

     case PREFERENCES_REFINEMENT_SPEED:
       commands.push_back(state_command("set-dragged-refinement-steps-per-frame",
					g.preferences_internal[i].ivalue1, il));
       break;

     case PREFERENCES_TIPS:
	if (g.preferences_internal[i].ivalue1 == 0) {
	   if (il == 1) { // guile
#ifdef USE_GUILE_GTK	   
	      commands.push_back(state_command("no-coot-tips", il));
#endif
	   } else {
	      commands.push_back(state_command("no-coot-tips", il));
	   }
	}
	break;
      
     case PREFERENCES_SPIN_SPEED:
       commands.push_back(state_command("set-idle-function-rotate-angle",
					g.preferences_internal[i].fvalue1, il));
       break;
      
     case PREFERENCES_FONT_SIZE:
       commands.push_back(state_command("set-font-size",
					g.preferences_internal[i].ivalue1, il));
       break;
      
     case PREFERENCES_FONT_COLOUR:      
       fval1 = g.preferences_internal[i].fvalue1;  // red
       fval2 = g.preferences_internal[i].fvalue2;  // green
       fval3 = g.preferences_internal[i].fvalue3;  // blue
       if (fval1 >= 0.999 && 
	  fval2 >= 0.799 && fval2 <= 0.801 &&
	  fval3 >= 0.799 && fval3 <= 0.801) {
	  commands.push_back(state_command("set-font-colour", 1.0, 0.8, 0.8, il));
       } else {
	  commands.push_back(state_command("set-font-colour", fval1, fval2, fval3, il));
       }
       break;

     case PREFERENCES_PINK_POINTER:
       commands.push_back(state_command("set-rotation-centre-size",
					g.preferences_internal[i].fvalue1, il));
       break;
      
     case PREFERENCES_MODEL_TOOLBAR_SHOW:
       if (g.preferences_internal[i].ivalue1 == 0) {
	 commands.push_back(state_command("hide-modelling-toolbar", il));
       } else {
	 commands.push_back(state_command("show-modelling-toolbar", il));
       }
       break;

     case PREFERENCES_MODEL_TOOLBAR_POSITION:
       commands.push_back(state_command("set-model-toolbar-docked-position",
					g.preferences_internal[i].ivalue1, il));
       break;

     case PREFERENCES_MODEL_TOOLBAR_STYLE:
       commands.push_back(state_command("set-model-toolbar-style",
					g.preferences_internal[i].ivalue1, il));
       break;

     case PREFERENCES_MODEL_TOOLBAR_ICONS:
       if (g.preferences_internal[i].ivalue2 == 1) {
	 commands.push_back(state_command("show-model-toolbar-icon",
					  g.preferences_internal[i].ivalue1, il));
       } else {
	 commands.push_back(state_command("hide-model-toolbar-icon",
					  g.preferences_internal[i].ivalue1, il));
       }
       break;

     case PREFERENCES_MAIN_TOOLBAR_SHOW:
       if (g.preferences_internal[i].ivalue1 == 0) {
	 commands.push_back(state_command("hide-main-toolbar", il));
       } else {
	 commands.push_back(state_command("show-main-toolbar", il));
       }
       break;

       //case PREFERENCES_MAIN_TOOLBAR_POSITION:
       //commands.push_back(state_command("set-main-toolbar-docked-position",
       //				g.preferences_internal[i].ivalue1, il));
       //break;

     case PREFERENCES_MAIN_TOOLBAR_STYLE:
       commands.push_back(state_command("set-main-toolbar-style",
					g.preferences_internal[i].ivalue1, il));
       break;

     case PREFERENCES_MAIN_TOOLBAR_ICONS:
         if (g.preferences_internal[i].ivalue2 == 1) {
             commands.push_back(state_command("show-main-toolbar-icon",
                                              g.preferences_internal[i].ivalue1, il));
         } else {
             commands.push_back(state_command("hide-main-toolbar-icon",
                                              g.preferences_internal[i].ivalue1, il));
         }
         break;

     }
   }

   istat = write_state(commands, filename);
   return istat;
}

// makes a vector of preference_info_t (graphics_info_t::preferences_internal)
// based on current status of coot
//
void
graphics_info_t::make_preferences_internal() {

  coot::preference_info_t p;
  std::vector<coot::preference_info_t> ret;
  
  int on;
  float fvalue;
  // General preference settings
  // file chooser

  on = file_chooser_selector_state();
  
  p.preference_type = PREFERENCES_FILE_CHOOSER;
  p.ivalue1 = on;
  ret.push_back(p);
  
  on = file_chooser_overwrite_state();

  p.preference_type = PREFERENCES_FILE_OVERWRITE;
  p.ivalue1 = on;
  ret.push_back(p);
 
  on = filter_fileselection_filenames_state();
  p.preference_type = PREFERENCES_FILE_FILTER;
  p.ivalue1 = on;
  ret.push_back(p);
 
  on = graphics_info_t::sticky_sort_by_date;
  p.preference_type = PREFERENCES_FILE_SORT_DATE;
  p.ivalue1 = on;
  ret.push_back(p);
 
  // dialogs
  on = accept_reject_dialog_docked_state();
  p.preference_type = PREFERENCES_ACCEPT_DIALOG_DOCKED;
  p.ivalue1 = on;
  ret.push_back(p);

  on = accept_reject_dialog_docked_show_state();
  p.preference_type = PREFERENCES_ACCEPT_DIALOG_DOCKED_SHOW;
  p.ivalue1 = on;
  ret.push_back(p);

  on = refinement_immediate_replacement_state();
  p.preference_type = PREFERENCES_IMMEDIATE_REPLACEMENT;
  p.ivalue1 = on;
  ret.push_back(p);

  // hid
  if (trackball_size > 5) {
    on = 1;
  } else {
    on = 2;
  }
  //  on = graphics_info_t::vt_surface_status();
  p.preference_type = PREFERENCES_VT_SURFACE;
  p.ivalue1 = on;
  ret.push_back(p);

  // recentre pdb
  on = recentre_on_read_pdb;
  p.preference_type = PREFERENCES_RECENTRE_PDB;
  p.ivalue1 = on;
  ret.push_back(p);

  // refinement toolbar show/hide
  on = model_toolbar_show_hide_state;
  p.preference_type = PREFERENCES_MODEL_TOOLBAR_SHOW;
  p.ivalue1 = on;
  ret.push_back(p);

  // refinement toolbar right/left
  on = model_toolbar_position_state;
  p.preference_type = PREFERENCES_MODEL_TOOLBAR_POSITION;
  p.ivalue1 = on;
  ret.push_back(p);

  // refinement toolbar style
  on = model_toolbar_style_state;
  p.preference_type = PREFERENCES_MODEL_TOOLBAR_STYLE;
  p.ivalue1 = on;
  ret.push_back(p);

  // refinement toolbar icons
  std::vector<coot::preferences_icon_info_t> all_items =*model_toolbar_icons;
  for (unsigned int i=0; i<all_items.size(); i++) {
    coot::preferences_icon_info_t item = all_items[i];
    p.preference_type = PREFERENCES_MODEL_TOOLBAR_ICONS;
    // ivalue1 is icon_pos
    p.ivalue1 = item.icon_pos;
    // ivalue 2 is show/hide
    p.ivalue2 = item.show_hide_flag;
    ret.push_back(p);
  }

  // main toolbar
  // main toolbar show/hide
  on = main_toolbar_show_hide_state;
  p.preference_type = PREFERENCES_MAIN_TOOLBAR_SHOW;
  p.ivalue1 = on;
  ret.push_back(p);

  // main toolbar top/bottom/left...
  // not yet
  //on = main_toolbar_position_state;
  //p.preference_type = PREFERENCES_MAIN_TOOLBAR_POSITION;
  //p.ivalue1 = on;
  //ret.push_back(p);

  // main toolbar style
  on = main_toolbar_style_state;
  p.preference_type = PREFERENCES_MAIN_TOOLBAR_STYLE;
  p.ivalue1 = on;
  ret.push_back(p);

  // refinement toolbar icons
  all_items =*main_toolbar_icons;
  for (unsigned int i=0; i<all_items.size(); i++) {
      coot::preferences_icon_info_t item = all_items[i];
      p.preference_type = PREFERENCES_MAIN_TOOLBAR_ICONS;
      // ivalue1 is icon_pos
      p.ivalue1 = item.icon_pos;
      // ivalue 2 is show/hide
      p.ivalue2 = item.show_hide_flag;
      ret.push_back(p);
  }
 
  // Bond preference settings
  // Bond parameters
  on = get_default_bond_thickness();
  p.preference_type = PREFERENCES_BONDS_THICKNESS;
  p.ivalue1 = on;
  ret.push_back(p);

  // Bond colours
  fvalue = graphics_info_t::rotate_colour_map_on_read_pdb;
  p.preference_type = PREFERENCES_BOND_COLOURS_MAP_ROTATION;
  p.fvalue1 = fvalue;
  ret.push_back(p);
   
  on = get_colour_map_rotation_on_read_pdb_c_only_flag();
  p.preference_type = PREFERENCES_BOND_COLOUR_ROTATION_C_ONLY;
  p.ivalue1 = on;
  ret.push_back(p);
   
  // Map preference settings
  // Map parameters
  fvalue = get_map_radius();
  p.preference_type = PREFERENCES_MAP_RADIUS;
  p.fvalue1 = fvalue;
  ret.push_back(p);

  fvalue = get_iso_level_increment();
  p.preference_type = PREFERENCES_MAP_ISOLEVEL_INCREMENT;
  p.fvalue1 = fvalue;
  ret.push_back(p);

  fvalue = get_diff_map_iso_level_increment();
  p.preference_type = PREFERENCES_DIFF_MAP_ISOLEVEL_INCREMENT;
  p.fvalue1 = fvalue;
  ret.push_back(p);

  fvalue = get_map_sampling_rate(); 
  p.preference_type = PREFERENCES_MAP_SAMPLING_RATE;
  p.fvalue1 = fvalue;
  ret.push_back(p);

  on = get_dynamic_map_sampling();
  p.preference_type = PREFERENCES_DYNAMIC_MAP_SAMPLING;
  p.ivalue1 = on;
  ret.push_back(p);
  
  on = get_dynamic_map_size_display();
  p.preference_type = PREFERENCES_DYNAMIC_MAP_SIZE_DISPLAY;
  p.ivalue1 = on;
  ret.push_back(p);

  // Map colours
  on = swap_difference_map_colours_state();
  p.preference_type = PREFERENCES_SWAP_DIFF_MAP_COLOURS;
  p.ivalue1 = on;
  ret.push_back(p);

  fvalue = graphics_info_t::rotate_colour_map_for_map;
  p.preference_type = PREFERENCES_MAP_COLOURS_MAP_ROTATION;
  p.fvalue1 = fvalue;
  ret.push_back(p);
   
  // Map smooth scroll
  on = get_smooth_scroll();
  p.preference_type = PREFERENCES_SMOOTH_SCROLL;
  p.ivalue1 = on;
  ret.push_back(p);

  on = graphics_info_t::smooth_scroll_steps;
  p.preference_type = PREFERENCES_SMOOTH_SCROLL_STEPS;
  p.ivalue1 = on;
  ret.push_back(p);

  fvalue = graphics_info_t::smooth_scroll_limit;
  p.preference_type = PREFERENCES_SMOOTH_SCROLL_LIMIT;
  p.fvalue1 = fvalue;
  ret.push_back(p);

  // Dragged map
  on = get_active_map_drag_flag();
  p.preference_type = PREFERENCES_MAP_DRAG;
  p.ivalue1 = on;
  ret.push_back(p);

  // Geometry preference settings
  // Cis peptides
  on = show_mark_cis_peptides_as_bad_state();
  p.preference_type = PREFERENCES_MARK_CIS_BAD;
  p.ivalue1 = on;
  ret.push_back(p);
  
  // Colour preference settings
  // Background colour
  p.preference_type = PREFERENCES_BG_COLOUR;
  p.fvalue1 = graphics_info_t::background_colour[0];
  p.fvalue2 = graphics_info_t::background_colour[1];
  p.fvalue3 = graphics_info_t::background_colour[2];
  ret.push_back(p);

  // Others preference settings
  // Antialias
  p.preference_type = PREFERENCES_ANTIALIAS;
  p.ivalue1 = do_anti_aliasing_state();
  ret.push_back(p);

  // Console
  p.preference_type = PREFERENCES_CONSOLE_COMMANDS;
  p.ivalue1 = graphics_info_t::console_display_commands.display_commands_flag;
  ret.push_back(p);

  // Tips
  p.preference_type = PREFERENCES_TIPS;
  p.ivalue1 = graphics_info_t::do_tip_of_the_day_flag;
  ret.push_back(p);

  // Speed
  on = graphics_info_t::dragged_refinement_steps_per_frame;
  p.preference_type = PREFERENCES_REFINEMENT_SPEED;
  p.ivalue1 = on;
  ret.push_back(p);

  fvalue = graphics_info_t::idle_function_rotate_angle;
  p.preference_type = PREFERENCES_SPIN_SPEED;
  p.fvalue1 = fvalue;
  ret.push_back(p);

  // Fonts
  // Font size
  on = graphics_info_t::atom_label_font_size;
  p.preference_type = PREFERENCES_FONT_SIZE;
  p.ivalue1 = on;
  ret.push_back(p);

  // Font colour
  p.preference_type = PREFERENCES_FONT_COLOUR;
  p.fvalue1 = graphics_info_t::font_colour.red;
  p.fvalue2 = graphics_info_t::font_colour.green;
  p.fvalue3 = graphics_info_t::font_colour.blue;
  ret.push_back(p);

  p.preference_type = PREFERENCES_FONT_OWN_COLOUR_FLAG;
  p.ivalue1 = -1; // unset
  ret.push_back(p);

  // Pink pointer size
  fvalue = graphics_info_t::rotation_centre_cube_size;
  p.preference_type = PREFERENCES_PINK_POINTER;
  p.fvalue1 = fvalue;
  ret.push_back(p);


  graphics_info_t::preferences_internal = ret;

}

void
graphics_info_t::preferences_internal_change_value(int preference_type, int ivalue){

  for (unsigned int i=0; i<preferences_internal.size(); i++) {
    if (preferences_internal[i].preference_type == preference_type) {
      preferences_internal[i].ivalue1 = ivalue;
      break;
    }
  }
}

void
graphics_info_t::preferences_internal_change_value(int preference_type, int ivalue1, int ivalue2){

  for (unsigned int i=0; i<preferences_internal.size(); i++) {
    if (preferences_internal[i].preference_type == preference_type) {
      if (preferences_internal[i].ivalue1 == ivalue1) {
	preferences_internal[i].ivalue2 = ivalue2;
	break;
      }
    }
  }
}

void
graphics_info_t::preferences_internal_change_value(int preference_type, float fvalue){

  for (unsigned int i=0; i<preferences_internal.size(); i++) {
    if (preferences_internal[i].preference_type == preference_type) {
      preferences_internal[i].fvalue1 = fvalue;
      break;
    }
  }
}

void
graphics_info_t::preferences_internal_change_value(int preference_type, 
						   float fvalue1, float fvalue2, float fvalue3){

  for (unsigned int i=0; i<preferences_internal.size(); i++) {
    if (preferences_internal[i].preference_type == preference_type) {
      preferences_internal[i].fvalue1 = fvalue1;
      preferences_internal[i].fvalue2 = fvalue2;
      preferences_internal[i].fvalue3 = fvalue3;
      break;
    }
  }
}


#if (GTK_MAJOR_VERSION >1)
void
graphics_info_t::preferences_model_toolbar_icon_toggled(GtkCellRendererToggle *button,
                                                        gchar *path_string,
                                                        gpointer data) {

    preferences_toolbar_icon_toggled(button, path_string, data, MODEL_TOOLBAR);

}

void
graphics_info_t::preferences_main_toolbar_icon_toggled(GtkCellRendererToggle *button,
                                                       gchar *path_string,
                                                       gpointer data) {

    preferences_toolbar_icon_toggled(button, path_string, data, MAIN_TOOLBAR);

}

void
graphics_info_t::preferences_toolbar_icon_toggled(GtkCellRendererToggle *button,
                                                  gchar *path_string,
                                                  gpointer data,
                                                  int toolbar_index) {

  GtkTreeModel *model = GTK_TREE_MODEL (data);
  GtkTreeIter iter;
  GtkTreePath *path = gtk_tree_path_new_from_string (path_string);
  gboolean value;
  gint icon_pos;

  gtk_tree_model_get_iter(model, &iter, path);
  gtk_tree_model_get(model, &iter,
		     0, &value,
		     3, &icon_pos, -1);

  value = !value;
  gtk_list_store_set (GTK_LIST_STORE (model), &iter, 0, value, -1);
  graphics_info_t g;
  if (value) {
      g.show_hide_toolbar_icon_pos(icon_pos, 1, toolbar_index);
  } else {
      g.show_hide_toolbar_icon_pos(icon_pos, 0, toolbar_index);
  }

  gtk_tree_path_free (path);
}
#endif // GTK_MAJOR_VERSION

enum {
  BUTTON_COL,
  ICON_COL,
  TEXT_COL,
  INDEX_COL
};

#if (GTK_MAJOR_VERSION >1)
void
graphics_info_t::fill_preferences_model_toolbar_icons(GtkWidget *preferences,
						      GtkWidget *scrolled_window) {

    fill_preferences_toolbar_icons(preferences, scrolled_window, MODEL_TOOLBAR);

}

void
graphics_info_t::fill_preferences_main_toolbar_icons(GtkWidget *preferences,
                                                     GtkWidget *scrolled_window) {

    fill_preferences_toolbar_icons(preferences, scrolled_window, MAIN_TOOLBAR);

}

void
graphics_info_t::fill_preferences_toolbar_icons(GtkWidget *preferences,
                                                GtkWidget *scrolled_window,
                                                int toolbar_index) {

    const gchar *tree_name;
    std::vector<coot::preferences_icon_info_t>* pall_items;
    if (toolbar_index == MODEL_TOOLBAR) {
        tree_name = "preferences_model_toolbar_icon_tree";
        pall_items = model_toolbar_icons;
    } else {
        tree_name = "preferences_main_toolbar_icon_tree";
        pall_items = main_toolbar_icons;
    }
  GtkWidget *icons_tree = gtk_tree_view_new();
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(icons_tree), FALSE);
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window),
					icons_tree);
  gtk_widget_ref(icons_tree);
  g_object_set_data_full(G_OBJECT(preferences),
                         tree_name,
                         icons_tree,
                         (GtkDestroyNotify) gtk_widget_unref);

  // maybe clear tree and model first?!
 
  // now fill the tree
  //  GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(icons_tree));
  //  GtkListStore *list_store = gtk_list_store_new(3, G_TYPE_BOOLEAN, GDK_TYPE_PIXBUF, G_TYPE_STRING);
  GtkTreeModel *model = GTK_TREE_MODEL(gtk_list_store_new(4, 
                        G_TYPE_BOOLEAN, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_INT));
  GtkTreeIter toplevel;
  GdkPixbuf *icon;
  GtkCellRenderer *icon_renderer;
  GtkCellRenderer *button_renderer;
  GtkCellRenderer *text_renderer;
  GtkTreeViewColumn *button_col;
  GtkTreeViewColumn *icon_col;
  GtkTreeViewColumn *text_col;
  GError *error = NULL;

  gtk_tree_view_set_model(GTK_TREE_VIEW(icons_tree), model);

  button_col = gtk_tree_view_column_new();
  icon_col = gtk_tree_view_column_new();
  text_col = gtk_tree_view_column_new();

  for (unsigned int i = 0; i < (*pall_items).size(); i++) {
      coot::preferences_icon_info_t item = (*pall_items)[i];
    gtk_list_store_append(GTK_LIST_STORE(model), &toplevel);

    // for icons
    if (item.icon_filename != "") {
      icon = gtk_widget_render_icon(icons_tree, item.icon_filename.c_str(),
                                    GTK_ICON_SIZE_SMALL_TOOLBAR, NULL);
      if (icon == NULL) {
        g_print("BL ERROR:: something went wrong, icon is NULL\n");
        // try to read as filename (although then should be registered and
        // read in already, but let's try
        std::string splash_screen_pixmap_dir = PKGDATADIR;  
        splash_screen_pixmap_dir += "/";
        splash_screen_pixmap_dir += "pixmaps";
        
        // over-ridden by user?
        char *s = getenv("COOT_PIXMAPS_DIR");
        if (s) {
          splash_screen_pixmap_dir = s;
        }

        // now add the icon
        std::string icon_path =
          coot::util::append_dir_file(splash_screen_pixmap_dir,
                                      item.icon_filename);
        icon = gdk_pixbuf_new_from_file(icon_path.c_str(), &error);
        if (error) {
          g_warning ("Could not load icon: %s\n", error->message);
          g_error_free(error);
          error = NULL;
        }
      } 
    } else {
      icon = NULL;
    }
    gtk_list_store_set(GTK_LIST_STORE(model), &toplevel,
		       BUTTON_COL, "",
		       ICON_COL, icon,
		       TEXT_COL, item.icon_text.c_str(),
		       INDEX_COL, item.icon_pos,
		       -1);
  }

  // button column
  button_col = gtk_tree_view_column_new();
  gtk_tree_view_append_column(GTK_TREE_VIEW(icons_tree), button_col);
  button_renderer = gtk_cell_renderer_toggle_new();
  gtk_tree_view_column_pack_start(button_col, GTK_CELL_RENDERER(button_renderer), TRUE);
  g_object_set(button_renderer, "activatable", TRUE, NULL);
  gtk_tree_view_column_set_attributes(button_col, button_renderer,
				      "active", BUTTON_COL,
				      NULL);

  // this could/should be done generic too, but requires an extra function...
  if (toolbar_index == MODEL_TOOLBAR) {
      g_signal_connect(button_renderer, "toggled", 
                       G_CALLBACK (preferences_model_toolbar_icon_toggled), model);
  } else {
      g_signal_connect(button_renderer, "toggled", 
                       G_CALLBACK (preferences_main_toolbar_icon_toggled), model);
  }

  // icon column
  icon_col = gtk_tree_view_column_new();
  gtk_tree_view_append_column(GTK_TREE_VIEW(icons_tree), icon_col);
  icon_renderer = gtk_cell_renderer_pixbuf_new();
  gtk_tree_view_column_pack_start(icon_col, icon_renderer, TRUE);
  gtk_tree_view_column_set_attributes(icon_col, icon_renderer,
				      "pixbuf", ICON_COL,
				      NULL);

  // text column
  text_col = gtk_tree_view_column_new();
  gtk_tree_view_append_column(GTK_TREE_VIEW(icons_tree), text_col);
  text_renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(text_col, text_renderer, TRUE);
  gtk_tree_view_column_set_attributes(text_col, text_renderer,
				      "text", TEXT_COL,
				      NULL);

  // update the tree
  update_toolbar_icons(model, toolbar_index);

  gtk_widget_show(icons_tree);
}
#endif // GTK_MAJOR_VERSION


void
graphics_info_t::show_hide_toolbar_icon_pos(int pos, int show_hide_flag, int toolbar_index) {

   if (use_graphics_interface_flag) {
      GtkWidget *icon_button = NULL;
      int preferences_index;

      std::vector<coot::preferences_icon_info_t>* pall_items;
      std::string widget_name;

      if (toolbar_index == MODEL_TOOLBAR) {
          pall_items = model_toolbar_icons;
          preferences_index = PREFERENCES_MODEL_TOOLBAR_ICONS;
      } else {
          pall_items = main_toolbar_icons;
          preferences_index = PREFERENCES_MAIN_TOOLBAR_ICONS;
      }
      coot::preferences_icon_info_t item = (*pall_items)[pos];
      widget_name = item.icon_widget;
      icon_button = lookup_widget(graphics_info_t::glarea, widget_name.c_str());

      if (icon_button) { 

	 if (show_hide_flag == 1) {
	    preferences_internal_change_value_int2(preferences_index, pos, 1);
	    (*pall_items)[pos].show();
	    gtk_widget_show(icon_button);
	 } else {
	    preferences_internal_change_value_int2(preferences_index, pos, 0);
	    (*pall_items)[pos].hide();
	    gtk_widget_hide(icon_button);
	 }
         coot::preferences_icon_info_t item = (*pall_items)[pos];
         
      }
   }
}


std::vector<int>
graphics_info_t::get_model_toolbar_icons_list() {

  std::vector<int> ivector;
  std::vector<coot::preferences_icon_info_t> all_items =*model_toolbar_icons;
  int icon_pos;

  for (unsigned int i=0; i<all_items.size(); i++) {
    coot::preferences_icon_info_t item = all_items[i];
    if (item.show_hide_flag == 1) {
      icon_pos = item.icon_pos;
      ivector.push_back(icon_pos);
    }

  }
  
  return ivector;
}


void
graphics_info_t::update_toolbar_icons(GtkTreeModel *model, int toolbar_index) {

  gint button_value;
  gint col_index;
  GtkTreeIter iter;

  std::vector<int> ivector;
  std::vector<coot::preferences_icon_info_t>* pall_items;

  if (toolbar_index == MODEL_TOOLBAR) {
      pall_items = model_toolbar_icons;
  } else {
      pall_items = main_toolbar_icons;
  }

  if (gtk_tree_model_get_iter_first(model, &iter)) {
    do {
      gtk_tree_model_get(model, &iter,
			 0, &button_value,
			 3, &col_index, -1);

      coot::preferences_icon_info_t item = (*pall_items)[col_index];
      
      if (item.show_hide_flag) {
	// show icon
	gtk_list_store_set (GTK_LIST_STORE (model), &iter, 0, 1, -1);
      } else {
	// hide icon
	gtk_list_store_set (GTK_LIST_STORE (model), &iter, 0, 0, -1);
      }

    } while (gtk_tree_model_iter_next(model, &iter));
    
  }
}


void
graphics_info_t::update_model_toolbar_icons(GtkTreeModel *model) {

    update_toolbar_icons(model, MODEL_TOOLBAR);

}

void
graphics_info_t::update_main_toolbar_icons(GtkTreeModel *model) {

    update_toolbar_icons(model, MAIN_TOOLBAR);

}

