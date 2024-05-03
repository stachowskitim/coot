/* src/molecule-class-info-widget-work.cc
 *
 * Copyright 2005, 2006 by The University of York
 * Author: Paul Emsley
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
 * write to the Free Software Foundation, Inc., 51 Franklin Street, 02110-1335, USA.
 */


#ifdef _MSC_VER
#include <windows.h>
#endif

#include <vector>
#include "molecule-class-info.h"

#include <mmdb2/mmdb_manager.h>
#include "coords/Cartesian.h"
#include "coords/mmdb-extras.h"
#include "coords/mmdb-crystal.h"

#include "utils/coot-utils.hh"
// #include "globjects.h" // for rotate_rgb.  That should be a utility
  			// function, not in globjects.hh

#include "widget-from-builder.hh"
#include "c-interface.h"
#include "c-interface-gtk-widgets.h"

void
molecule_class_info_t::update_map_colour_menu_maybe(int imol) {
   // or maybe not.
}




// extern "C" G_MODULE_EXPORT
void
on_ncs_controller_molecule_n_display_ncs_checkbutton_toggled_gtkbuilder_callback
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

  int imol = GPOINTER_TO_INT(user_data);
  int state = 0;
  if (gtk_toggle_button_get_active(togglebutton)) {
    state = 1;
    make_ncs_ghosts_maybe(imol);
  }
  /*    printf("NCS_controller Display NCS ghosts for imol %d %d\n", imol, state); */
  set_draw_ncs_ghosts(imol, state);
}


void
on_ncs_controller_molecule_n_display_chain_ich_checkbutton_toggled_gtkbuilder_callback
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
   int imol_chain = GPOINTER_TO_INT(user_data);
   int imol = imol_chain/1000;
   int ich = imol_chain - imol*1000;
   int state = 0;
   if (gtk_toggle_button_get_active(togglebutton)) {
     state = 1;
   }
   printf("\nNCS_controller display chain toggled for imol %d chain %d state %d\n",
	  imol, ich, state);
   ncs_control_display_chain(imol, ich, state);
}


void
on_ncs_controller_ncs_master_chain_ich_radiobutton_toggled_gtkbuilder_callback
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
   GtkWidget *w = widget_from_builder("ncs_control_dialog");
   int imol_chain = GPOINTER_TO_INT(user_data);
   int imol = imol_chain/1000;
   int ich = imol_chain - imol*1000;
/*    printf("==== DEBUG:: chain raiobutton toggled: imol %d ich %d active-state: %d \n",  */
/* 	  imol, gtk_toggle_button_get_active(ich, togglebutton)); */
   if (gtk_toggle_button_get_active(togglebutton)) {
/*      printf("NCS_controller_ncs_master_chain_ich_radiobutton_toggled on for imol %d %d %d\n",  */
/* 	    imol_chain, imol, ich); */

/*      ncs_control_change_ncs_master_to_chain(imol, ich); (done in the following function) */

     ncs_control_change_ncs_master_to_chain_update_widget(w, imol, ich);
   }
}


void
on_molecule_0_checkbutton_toggled_gtkbuilder_callback      (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

  int imol = GPOINTER_TO_INT(user_data);
  if (gtk_toggle_button_get_active(togglebutton))
    set_show_symmetry_molecule(imol, 1);
  else
    set_show_symmetry_molecule(imol, 0);

}

void
on_colour_symm_std_molecule_0_toggled_gtkbuilder_callback  (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

  int imol = GPOINTER_TO_INT(user_data);
  if (gtk_toggle_button_get_active(togglebutton)) {
    set_symmetry_colour_by_symop(imol, 0);
    set_symmetry_molecule_rotate_colour_map(imol, 0);
  }
}

void
on_display_sphere_radiobutton_molecule_0_toggled_gtkbuilder_callback
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

  int imol = GPOINTER_TO_INT(user_data);
  if (gtk_toggle_button_get_active(togglebutton)) {
    set_symmetry_whole_chain(imol, 0);
    symmetry_as_calphas(imol, 0); /* does an update_symmetry() */
  }
}


void
on_display_all_radiobutton_molecule_0_toggled_gtkbuilder_callback
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

  int imol = GPOINTER_TO_INT(user_data);
  if (gtk_toggle_button_get_active(togglebutton)) {
    symmetry_as_calphas(imol, 0);
    set_symmetry_whole_chain(imol, 1);
/*   } else { */
/*     symmetry_as_calphas(imol, 1); */
/*     printf("DEBUG:: all for molecule %d CA state 1\n", imol); */
   }
}

void
on_colour_symm_by_symop_molecule_0_toggled_gtkbuilder_callback
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

  int imol = GPOINTER_TO_INT(user_data);
  if (gtk_toggle_button_get_active(togglebutton)) {
    set_symmetry_molecule_rotate_colour_map(imol, 1); /* yes, I mean this */
    set_symmetry_colour_by_symop(imol, 1);
  }
}

void
on_display_CA_radiobutton_molecule_0_toggled_gtkbuilder_callback
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

  int imol = GPOINTER_TO_INT(user_data);
  if (gtk_toggle_button_get_active(togglebutton)) {
     symmetry_as_calphas(imol, 1);
  }
}

void
on_colour_symm_by_molecule_molecule_0_toggled_gtkbuilder_callback(GtkToggleButton *togglebutton,
                                                                  gpointer         user_data) {

  int imol = GPOINTER_TO_INT(user_data);
  if (gtk_toggle_button_get_active(togglebutton)) {
    set_symmetry_colour_by_symop(imol, 0);
    set_symmetry_molecule_rotate_colour_map(imol, 1);
  }
}









void
molecule_class_info_t::handle_map_colour_change_rotate_difference_map(bool swap_difference_map_colours_flag) {

   // Usually (by default) the colours for the difference map are  green and red. Some people like red and
   // green. set_last_map_colour() calls this function and it is here that we decide on the second (negative
   // level) colour.
   float rotation_size = rotate_colour_map_for_difference_map/360.0;
   if (swap_difference_map_colours_flag)
      rotation_size = (360.0 - rotate_colour_map_for_difference_map)/360.0;

   // std::vector<float> rgb_new = rotate_rgb(orig_colours, rotation_size);
   // map_colour_negative_level.red   = rgb_new[0];
   // map_colour_negative_level.green = rgb_new[1];
   // map_colour_negative_level.blue  = rgb_new[2];

   coot::colour_holder ch(map_colour.red, map_colour.green, map_colour.blue);
   ch.rotate_by(rotation_size);
   map_colour_negative_level.red   = ch.red;
   map_colour_negative_level.green = ch.green;
   map_colour_negative_level.blue  = ch.blue;
}

void
molecule_class_info_t::handle_map_colour_change(GdkRGBA map_col_in,
                                                bool swap_difference_map_colours_flag,
                                                bool main_or_secondary,
                                                clipper::Coord_orth centre,
                                                float radius) {

   if (false)
      std::cout << "debug:: handle_map_colour_change() handle change to map colour "
                << map_col_in.red << " "
                << map_col_in.green << " "
                << map_col_in.blue << std::endl;

   map_colour = map_col_in;

   // input map colours are now in the range 0 to 1
   // map_colour.red   = map_col_in.red  /65535.0;
   // map_colour.green = map_col_in.green/65535.0;
   // map_colour.blue  = map_col_in.blue /65535.0;

   if (xmap_is_diff_map) {
      // std::cout << "calling handle_map_colour_change_rotate_difference_map() map colour for " << imol_no
      // << " with swap flag " << swap_difference_map_colours_flag << std::endl;
      handle_map_colour_change_rotate_difference_map(swap_difference_map_colours_flag);
   }

   // ideally, just change the colour buffer, but this will do for now.

   // clipper::Coord_orth centre = graphics_info_t::get_rotation_centre_co();
   // float radius = graphics_info_t::box_radius_xray;
   setup_glsl_map_rendering(centre, radius);

   // main 0: secondary: 1
   // compile_density_map_display_list(main_or_secondary);
}

// symmetry control
//
// We create a frame and add it to the viewport that's passed.  It is used to fill the
// symmetry control widget (requested by Frank von Delft)
//
// Perhaps this should be in a file molecule-class-info-widget-work.cc
void
molecule_class_info_t::fill_symmetry_control_frame(GtkWidget *symmetry_controller_dialog) const {

   std::string s = "Molecule ";
   std::string imol_str = coot::util::int_to_string(imol_no);
   std::string molecule_n = "molecule_";
   molecule_n += imol_str;
   s          += imol_str;
   s          += " ";
   s          += name_for_display_manager();

   GtkWidget *molecule_0_frame;
   GtkWidget *vbox168;
   GtkWidget *molecule_0_checkbutton;
   GtkWidget *frame162;
   GtkWidget *table4;
   GSList *symm_display_mol_0_gr_group = NULL;
   GtkWidget *display_sphere_radiobutton_molecule_0 = nullptr;
   GtkWidget *display_all_radiobutton_molecule_0 = nullptr;
   GtkWidget *display_CA_radiobutton_molecule_0 = nullptr;
   GSList *symm_colour_mol_0_gr_group = NULL;
   GtkWidget *colour_symm_std_molecule_0 = nullptr;
   GtkWidget *colour_symm_by_symop_molecule_0 = nullptr;
   GtkWidget *colour_symm_by_molecule_molecule_0 = nullptr;
   GtkWidget *symmetry_control_vbox = nullptr;

   // symmetry_control_vbox = lookup_widget(symmetry_controller_dialog, "symmetry_controller_vbox");
   symmetry_control_vbox = widget_from_builder("symmetry_controller_vbox");

   molecule_0_frame = gtk_frame_new (s.c_str());
   // gtk_widget_ref (molecule_0_frame);
   std::string t = molecule_n + "_frame";
   g_object_set_data_full (G_OBJECT (symmetry_controller_dialog),
			     t.c_str(),
			     molecule_0_frame, NULL);
   gtk_box_append(GTK_BOX(symmetry_control_vbox), molecule_0_frame);

   vbox168 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
   // gtk_widget_ref (vbox168);
   g_object_set_data_full (G_OBJECT (symmetry_controller_dialog), "vbox168", vbox168, NULL);
   gtk_widget_set_visible (vbox168, TRUE);
   gtk_frame_set_child(GTK_FRAME(molecule_0_frame), vbox168);

   molecule_0_checkbutton = gtk_check_button_new_with_label (" Show Symmetry?");
   // gtk_widget_ref (molecule_0_checkbutton);
   std::string molecule_n_checkbutton = molecule_n + "_checkbutton";
   g_object_set_data_full (G_OBJECT (symmetry_controller_dialog),
			     molecule_n_checkbutton.c_str(),
			     molecule_0_checkbutton, NULL);
   gtk_widget_set_visible (molecule_0_checkbutton, TRUE);

   gtk_box_append (GTK_BOX (vbox168), molecule_0_checkbutton);
   if (show_symmetry)
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(molecule_0_checkbutton), TRUE);

   frame162 = gtk_frame_new ("Display Options");
   // gtk_widget_ref (frame162);
   g_object_set_data_full (G_OBJECT (symmetry_controller_dialog), "frame162", frame162, NULL);
   gtk_widget_set_visible (frame162, TRUE);
   gtk_box_append (GTK_BOX (vbox168), frame162);

#if (GTK_MAJOR_VERSION == 3 && GTK_MINOR_VERSION == 94) || (GTK_MAJOR_VERSION == 4)
      // 20220528-PE FIXME table to grid
#else
   // table4 = gtk_table_new (3, 2, FALSE);
   table4 = gtk_grid_new ();
   // gtk_widget_ref (table4);
   g_object_set_data_full (G_OBJECT (symmetry_controller_dialog), "table4", table4, NULL);
   gtk_widget_set_visible (table4, TRUE);
   gtk_container_add (GTK_CONTAINER (frame162), table4);

   display_sphere_radiobutton_molecule_0 = gtk_radio_button_new_with_label (symm_display_mol_0_gr_group, "Display Sphere");
   symm_display_mol_0_gr_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (display_sphere_radiobutton_molecule_0));
   // gtk_widget_ref (display_sphere_radiobutton_molecule_0);
   s = "display_sphere_radiobutton_" + molecule_n;
   g_object_set_data_full (G_OBJECT (symmetry_controller_dialog),
			     s.c_str(),
			     display_sphere_radiobutton_molecule_0, NULL);
   gtk_widget_set_visible (display_sphere_radiobutton_molecule_0, TRUE);

   gtk_table_attach (GTK_TABLE (table4), display_sphere_radiobutton_molecule_0, 0, 1, 0, 1,
		     (GtkAttachOptions) (GTK_FILL),
		     (GtkAttachOptions) (0), 0, 0);
#endif

#if (GTK_MAJOR_VERSION >= 4)
   // 20220602-PE FIXME radio buttons
#else
   display_all_radiobutton_molecule_0 = gtk_radio_button_new_with_label (symm_display_mol_0_gr_group,
									 "Display Near Chains");
   symm_display_mol_0_gr_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (display_all_radiobutton_molecule_0));
#endif
   // gtk_widget_ref (display_all_radiobutton_molecule_0);

   // set display_all_radiobutton_
   if (symmetry_whole_chain_flag)
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(display_all_radiobutton_molecule_0), TRUE);

   s = "display_all_radiobutton_" + molecule_n;
   g_object_set_data_full (G_OBJECT (symmetry_controller_dialog),
			     s.c_str(),
			     display_all_radiobutton_molecule_0, NULL);
   gtk_widget_set_visible (display_all_radiobutton_molecule_0, TRUE);

#if (GTK_MAJOR_VERSION == 3 && GTK_MINOR_VERSION == 94) || (GTK_MAJOR_VERSION == 4)

#else
   gtk_table_attach (GTK_TABLE (table4), display_all_radiobutton_molecule_0, 0, 1, 1, 2,
		     (GtkAttachOptions) (GTK_FILL),
		     (GtkAttachOptions) (0), 0, 0);
#endif

#if (GTK_MAJOR_VERSION >= 4)
   // 20220602-PE FIXME radio buttons
#else
   display_CA_radiobutton_molecule_0 = gtk_radio_button_new_with_label (symm_display_mol_0_gr_group, "Display as CAs");
   symm_display_mol_0_gr_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (display_CA_radiobutton_molecule_0));
#endif

   // gtk_widget_ref (display_CA_radiobutton_molecule_0);

   // set display_CA_radiobutton_
   if (symmetry_as_calphas)
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(display_CA_radiobutton_molecule_0), TRUE);

   s = "display_CA_radiobutton_" + molecule_n;
   g_object_set_data_full (G_OBJECT (symmetry_controller_dialog),
			     s.c_str(),
			     display_CA_radiobutton_molecule_0, NULL);
   gtk_widget_set_visible (display_CA_radiobutton_molecule_0, TRUE);

#if (GTK_MAJOR_VERSION == 3 && GTK_MINOR_VERSION == 94) || (GTK_MAJOR_VERSION == 4)

#else
   gtk_table_attach (GTK_TABLE (table4), display_CA_radiobutton_molecule_0, 0, 1, 2, 3,
		     (GtkAttachOptions) (GTK_FILL),
		     (GtkAttachOptions) (0), 0, 0);
#endif

#if (GTK_MAJOR_VERSION >= 4)
   // 20220602-PE FIXME radio buttons
#else
   colour_symm_std_molecule_0 = gtk_radio_button_new_with_label (symm_colour_mol_0_gr_group, "Standard Colouring");
   symm_colour_mol_0_gr_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (colour_symm_std_molecule_0));
#endif
   // gtk_widget_ref (colour_symm_std_molecule_0);

   s = "colour_symm_std_" + molecule_n;
   g_object_set_data_full (G_OBJECT (symmetry_controller_dialog),
			     s.c_str(),
			     colour_symm_std_molecule_0, NULL);
   gtk_widget_set_visible (colour_symm_std_molecule_0, TRUE);

#if (GTK_MAJOR_VERSION == 3 && GTK_MINOR_VERSION == 94) || (GTK_MAJOR_VERSION == 4)

#else
   gtk_table_attach (GTK_TABLE (table4), colour_symm_std_molecule_0, 1, 2, 0, 1,
		     (GtkAttachOptions) (GTK_FILL),
		     (GtkAttachOptions) (0), 0, 0);
#endif

   // set the colour radiobutton
   if (symmetry_colour_by_symop_flag == 0 && symmetry_rotate_colour_map_flag == 0)
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(colour_symm_std_molecule_0), TRUE);

#if (GTK_MAJOR_VERSION >= 4)
   // 20220602-PE FIXME radio buttons
#else
   colour_symm_by_symop_molecule_0 = gtk_radio_button_new_with_label (symm_colour_mol_0_gr_group,
								      "Colour by Symop");
   symm_colour_mol_0_gr_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (colour_symm_by_symop_molecule_0));
#endif
   // gtk_widget_ref (colour_symm_by_symop_molecule_0);

   s = "colour_symm_by_symop_" + molecule_n;
   g_object_set_data_full (G_OBJECT (symmetry_controller_dialog),
			     s.c_str(),
			     colour_symm_by_symop_molecule_0, NULL);
   gtk_widget_set_visible (colour_symm_by_symop_molecule_0, TRUE);

#if (GTK_MAJOR_VERSION == 3 && GTK_MINOR_VERSION == 94) || (GTK_MAJOR_VERSION == 4)

#else
   gtk_table_attach (GTK_TABLE (table4), colour_symm_by_symop_molecule_0, 1, 2, 1, 2,
		     (GtkAttachOptions) (GTK_FILL),
		     (GtkAttachOptions) (0), 0, 0);
#endif

   // set the colour radiobutton
   if (symmetry_colour_by_symop_flag)
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(colour_symm_by_symop_molecule_0), TRUE);


#if (GTK_MAJOR_VERSION >= 4)
   // 20220602-PE FIXME radio buttons
#else
   colour_symm_by_molecule_molecule_0 = gtk_radio_button_new_with_label (symm_colour_mol_0_gr_group, "Colour by Molecule");
   symm_colour_mol_0_gr_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (colour_symm_by_molecule_molecule_0));
#endif

   // gtk_widget_ref (colour_symm_by_molecule_molecule_0);
   s = "colour_symm_by_molecule_" + molecule_n;
   g_object_set_data_full (G_OBJECT (symmetry_controller_dialog),
			     s.c_str(),
			     colour_symm_by_molecule_molecule_0, NULL);
   gtk_widget_set_visible (colour_symm_by_molecule_molecule_0, TRUE);

#if (GTK_MAJOR_VERSION == 3 && GTK_MINOR_VERSION == 94) || (GTK_MAJOR_VERSION == 4)

#else
   gtk_table_attach (GTK_TABLE (table4), colour_symm_by_molecule_molecule_0, 1, 2, 2, 3,
		     (GtkAttachOptions) (GTK_FILL),
		     (GtkAttachOptions) (0), 0, 0);
#endif

   // set the colour radiobutton
   if (symmetry_rotate_colour_map_flag)
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(colour_symm_by_molecule_molecule_0), TRUE);

   // 20220405-PE Porting to GTK4:
   // these callbacks are in glade-callbacks.cc - that's unusual and required glade-callbacks.hh

   g_signal_connect (G_OBJECT (molecule_0_checkbutton), "toggled",
		       G_CALLBACK (on_molecule_0_checkbutton_toggled_gtkbuilder_callback),
		       GINT_TO_POINTER(imol_no));
   g_signal_connect (G_OBJECT (display_sphere_radiobutton_molecule_0), "toggled",
		       G_CALLBACK (on_display_sphere_radiobutton_molecule_0_toggled_gtkbuilder_callback),
		       GINT_TO_POINTER(imol_no));
   g_signal_connect (G_OBJECT (display_all_radiobutton_molecule_0), "toggled",
		       G_CALLBACK (on_display_all_radiobutton_molecule_0_toggled_gtkbuilder_callback),
		       GINT_TO_POINTER(imol_no));
   g_signal_connect (G_OBJECT (display_CA_radiobutton_molecule_0), "toggled",
		       G_CALLBACK (on_display_CA_radiobutton_molecule_0_toggled_gtkbuilder_callback),
		       GINT_TO_POINTER(imol_no));
   g_signal_connect (G_OBJECT (colour_symm_std_molecule_0), "toggled",
		       G_CALLBACK (on_colour_symm_std_molecule_0_toggled_gtkbuilder_callback),
		       GINT_TO_POINTER(imol_no));
   g_signal_connect (G_OBJECT (colour_symm_by_symop_molecule_0), "toggled",
		       G_CALLBACK (on_colour_symm_by_symop_molecule_0_toggled_gtkbuilder_callback),
		       GINT_TO_POINTER(imol_no));
   g_signal_connect (G_OBJECT (colour_symm_by_molecule_molecule_0), "toggled",
		       G_CALLBACK (on_colour_symm_by_molecule_molecule_0_toggled_gtkbuilder_callback),
		       GINT_TO_POINTER(imol_no));

   gtk_widget_set_visible(molecule_0_frame, TRUE);
}

// NCS control
void
molecule_class_info_t::fill_ncs_control_frame(GtkWidget *ncs_control_dialog) const {


   if (atom_sel.n_selected_atoms > 0) {
      if (ncs_ghosts.size() > 0) {
	 fill_ncs_control_frame_internal(ncs_control_dialog);
      }
   }
}

#include "graphics-info.h" // 20220315-PE becaause we want clear_out_container().
                           // (no, it's not a good arangement)

// NCS control
void
molecule_class_info_t::fill_ncs_control_frame_internal(GtkWidget *ncs_control_dialog) const {

   // GtkWidget *ncs_control_vbox = lookup_widget(ncs_control_dialog, "ncs_control_vbox");
   GtkWidget *ncs_control_vbox = widget_from_builder("ncs_control_vbox");
   GtkWidget *frame_molecule_N;
   GtkWidget *vbox176;
   GtkWidget *ncs_controller_molecule_n_display_ncs_checkbutton;
   GtkWidget *hseparator11;
   GtkWidget *hbox134;
   GtkWidget *vbox172;
   GtkWidget *label264;
   GtkWidget *ncs_controller_molecule_n_display_chain_vbox;
   GtkWidget *ncs_controller_molecule_n_display_chain_ich_checkbutton = nullptr;
   GtkWidget *vbox174;
   GtkWidget *label265;
   GtkWidget *ncs_controller_molecule_n_vbox;
   GSList *molecule_n_ncs_master_chain_gr_group = NULL;
   GtkWidget *ncs_controller_ncs_master_chain_ich_radiobutton = nullptr;
   // GtkTooltips *tooltips;

   if (! ncs_control_dialog) return;

   // funny place to put this function...
   graphics_info_t::clear_out_container(ncs_control_vbox);

   std::string m("Molecule ");
   std::string imol_str = coot::util::int_to_string(imol_no);
   m += imol_str;
   m += " ";
   m += dotted_chopped_name();
   // tooltips = gtk_tooltips_new ();

   frame_molecule_N = gtk_frame_new (m.c_str());
   // gtk_widget_ref (frame_molecule_N);
   g_object_set_data_full (G_OBJECT (ncs_control_dialog),
			     "frame_molecule_N",
			     frame_molecule_N, NULL);

   gtk_box_append(GTK_BOX (ncs_control_vbox), frame_molecule_N);

   vbox176 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
   // gtk_widget_ref (vbox176);
   g_object_set_data_full (G_OBJECT (ncs_control_dialog), "vbox176", vbox176, NULL);
   gtk_widget_set_visible (vbox176, TRUE);
   gtk_frame_set_child(GTK_FRAME(frame_molecule_N), vbox176);

   ncs_controller_molecule_n_display_ncs_checkbutton =
      gtk_check_button_new_with_label ("Display Non-cystallographic Ghosts");
   // gtk_widget_ref (ncs_controller_molecule_n_display_ncs_checkbutton);

   std::string label_str_stub = "ncs_controller_molecule_";
   std::string label_str = label_str_stub + imol_str;
   label_str += "_display_ncs_checkbutton";
   g_object_set_data_full (G_OBJECT (ncs_control_dialog),
			     label_str.c_str(),
			     ncs_controller_molecule_n_display_ncs_checkbutton, NULL);
   gtk_widget_set_visible (ncs_controller_molecule_n_display_ncs_checkbutton, TRUE);

   gtk_box_append(GTK_BOX (vbox176), ncs_controller_molecule_n_display_ncs_checkbutton);
   if (show_ghosts_flag)
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ncs_controller_molecule_n_display_ncs_checkbutton), TRUE);


   //
   hseparator11 = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
   // gtk_widget_ref (hseparator11);
   g_object_set_data_full (G_OBJECT (ncs_control_dialog), "hseparator11", hseparator11, NULL);
   gtk_widget_set_visible (hseparator11, TRUE);
   gtk_box_append(GTK_BOX (vbox176), hseparator11);

   hbox134 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
   // gtk_widget_ref (hbox134);
   g_object_set_data_full (G_OBJECT (ncs_control_dialog), "hbox134", hbox134, NULL);
   gtk_widget_set_visible (hbox134, TRUE);
   gtk_box_append(GTK_BOX (vbox176), hbox134);

   vbox172 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
   // gtk_widget_ref (vbox172);
   g_object_set_data_full (G_OBJECT (ncs_control_dialog), "vbox172", vbox172, NULL);
   gtk_widget_set_visible (vbox172, TRUE);
   gtk_box_append(GTK_BOX (hbox134), vbox172);

   label264 = gtk_label_new("Displayed Chains");
   // gtk_widget_ref (label264);
   g_object_set_data_full (G_OBJECT (ncs_control_dialog), "label264", label264, NULL);
   gtk_widget_set_visible (label264, TRUE);
   gtk_box_append(GTK_BOX(vbox172), label264);

   ncs_controller_molecule_n_display_chain_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
   // gtk_widget_ref (ncs_controller_molecule_n_display_chain_vbox);
   g_object_set_data_full (G_OBJECT (ncs_control_dialog),
			     "ncs_controller_molecule_n_display_chain_vbox",
			     ncs_controller_molecule_n_display_chain_vbox, NULL);
   gtk_widget_set_visible (ncs_controller_molecule_n_display_chain_vbox, TRUE);

   gtk_box_append(GTK_BOX (vbox172), ncs_controller_molecule_n_display_chain_vbox);


   // for each [displayed?] chain:
   std::vector<std::string> v = coot::util::chains_in_molecule(atom_sel.mol);
   int n_chains = v.size();
   int ighost = 0;

   // is ncs_ghosts filled right now?

   std::string master = ncs_ghosts[ighost].target_chain_id;
   for (int ich=0; ich<n_chains; ich++) {
      std::string label = "Chain ";
      label += v[ich];
      ncs_controller_molecule_n_display_chain_ich_checkbutton =
	 gtk_check_button_new_with_label (label.c_str());
      // gtk_widget_ref (ncs_controller_molecule_n_display_chain_ich_checkbutton);
      std::string name = "ncs_controller_molecule_";
      name += imol_str;
      name += "_display_chain_";
      name += coot::util::int_to_string(ich);
      name += "_checkbutton";
      g_object_set_data_full (G_OBJECT (ncs_control_dialog),
				name.c_str(),
				ncs_controller_molecule_n_display_chain_ich_checkbutton, NULL);
      gtk_widget_set_visible (ncs_controller_molecule_n_display_chain_ich_checkbutton, TRUE);

      gtk_box_append(GTK_BOX(ncs_controller_molecule_n_display_chain_vbox),
                     ncs_controller_molecule_n_display_chain_ich_checkbutton);

      std::cout << "Fix the tip B" << std::endl;

      // g_tooltips_set_tip (tooltips, ncs_controller_molecule_n_display_chain_ich_checkbutton,
	// 		    _("Show this chain as a ghost chain?"), NULL);
      g_signal_connect (G_OBJECT (ncs_controller_molecule_n_display_chain_ich_checkbutton),
			  "toggled",
			  G_CALLBACK (on_ncs_controller_molecule_n_display_chain_ich_checkbutton_toggled_gtkbuilder_callback),
			  GINT_TO_POINTER(imol_no*1000 + ich));

      // we set the Displayed ghost chain to insensitive if it is the
      // master (which doesn't make sense)
      //
      // We also make it insensitive if this chain is not a ghost of
      // the master.
      //
      // e.g. conside the case: B matches A, D matches C.  If A is the
      // master, then we should not be seeing C or D ghosts.
      //
      if (v[ich] == master) {
	 gtk_widget_set_sensitive(ncs_controller_molecule_n_display_chain_ich_checkbutton, FALSE);
      }


      ighost = -1;
      for (unsigned int jghost=0; jghost<ncs_ghosts.size(); jghost++) {
	 if ( v[ich] == ncs_ghosts[jghost].chain_id) {
	    ighost = jghost;
	    break;
	 }
      }
      if (ighost != -1)
	 // 	 if (ncs_ghosts[ighost].display_it_flag)
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ncs_controller_molecule_n_display_chain_ich_checkbutton), TRUE);
   }

   vbox174 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
   // gtk_widget_ref (vbox174);
   g_object_set_data_full (G_OBJECT (ncs_control_dialog), "vbox174", vbox174, NULL);
   gtk_widget_set_visible (vbox174, TRUE);
   gtk_box_append(GTK_BOX (hbox134), vbox174);

   label265 = gtk_label_new ("NCS Master Chain");
   // gtk_widget_ref (label265);
   g_object_set_data_full (G_OBJECT (ncs_control_dialog), "label265", label265, NULL);
   gtk_widget_set_visible (label265, TRUE);
   // 20220528-PE FIXME set alignment
   gtk_box_append(GTK_BOX (vbox174), label265);

   ncs_controller_molecule_n_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
   // gtk_widget_ref (ncs_controller_molecule_n_vbox);
   g_object_set_data_full (G_OBJECT (ncs_control_dialog),
			     "ncs_controller_molecule_n_vbox", ncs_controller_molecule_n_vbox, NULL);
   gtk_widget_set_visible (ncs_controller_molecule_n_vbox, TRUE);

   gtk_box_append(GTK_BOX (vbox174), ncs_controller_molecule_n_vbox);

   // ncs master
   for (int ich=0; ich<n_chains; ich++) {
      std::string chain_str = v[ich];
      std::string label = "Chain ";
      label += chain_str;
#if (GTK_MAJOR_VERSION >= 4)
   // 20220602-PE FIXME radio buttons
#else
      ncs_controller_ncs_master_chain_ich_radiobutton =
	 gtk_radio_button_new_with_label (molecule_n_ncs_master_chain_gr_group, label.c_str());
      molecule_n_ncs_master_chain_gr_group =
	 gtk_radio_button_get_group (GTK_RADIO_BUTTON (ncs_controller_ncs_master_chain_ich_radiobutton));
#endif
      // gtk_widget_ref (ncs_controller_ncs_master_chain_ich_radiobutton);
      std::string name = "ncs_controller_ncs_master_chain_";
      name += imol_str;
      name += "_radiobutton";
      g_object_set_data_full (G_OBJECT (ncs_control_dialog),
				name.c_str(),
				ncs_controller_ncs_master_chain_ich_radiobutton, NULL);
      gtk_widget_set_visible (ncs_controller_ncs_master_chain_ich_radiobutton, TRUE);
      gtk_box_append(GTK_BOX (ncs_controller_molecule_n_vbox),
                     ncs_controller_ncs_master_chain_ich_radiobutton);
      std::cout << "Fix the tip BB" << std::endl;
      // gtk_tooltips_set_tip (tooltips, ncs_controller_ncs_master_chain_ich_radiobutton,
	// 		    _("The chain to which operators and ghosts are generated from other chains"), NULL);
      g_signal_connect (G_OBJECT (ncs_controller_ncs_master_chain_ich_radiobutton),
			  "toggled",
			  G_CALLBACK (on_ncs_controller_ncs_master_chain_ich_radiobutton_toggled_gtkbuilder_callback),
			  GINT_TO_POINTER(imol_no*1000 + ich));

      if (chain_str == master) {
	 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ncs_controller_ncs_master_chain_ich_radiobutton), TRUE);
      }
   }

   g_signal_connect (G_OBJECT (ncs_controller_molecule_n_display_ncs_checkbutton),
		       "toggled",
		       G_CALLBACK (on_ncs_controller_molecule_n_display_ncs_checkbutton_toggled_gtkbuilder_callback),
		       GINT_TO_POINTER(imol_no));

   gtk_widget_set_visible(frame_molecule_N, TRUE);

}


void
molecule_class_info_t::ncs_control_change_ncs_master_to_chain_update_widget(GtkWidget *w, int imaster) const {

   // Now we want to update the widget.  We need to change the sensitivity of
   // all the Chain check boxes in the dispaly ncs chain vbox.
   //
   // We need to change to desensitve the chain that matches ichain.
   //

   // First find imaster
   std::vector<std::string> chain_ids = coot::util::chains_in_molecule(atom_sel.mol);

   if (w) {
      if (imaster != -1) {
	 // GtkWidget *vbox = lookup_widget(w, "ncs_controller_molecule_n_display_chain_vbox");
	 GtkWidget *vbox = widget_from_builder("ncs_controller_molecule_n_display_chain_vbox");
	 std::string imol_str = coot::util::int_to_string(imol_no);
	 for (unsigned int i=0; i<chain_ids.size(); i++) {
	    std::string name = "ncs_controller_molecule_";
	    name += imol_str;
	    name += "_display_chain_";
	    name += coot::util::int_to_string(i);
	    name += "_checkbutton";
	    // GtkWidget *checkbutton = lookup_widget(vbox, name.c_str());
	    GtkWidget *checkbutton = 0;
            std::cout << "in ncs_control_change_ncs_master_to_chain_update_widget() set the checkbutton correctly" << std::endl;
	    if (checkbutton) {
	       if (int(i) == imaster) {
		  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton), FALSE);
		  gtk_widget_set_sensitive(checkbutton, FALSE);
	       } else {
		  gtk_widget_set_sensitive(checkbutton, TRUE);
		  // ncs control turns on all chains when we change the master
		  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton), TRUE);
	       }
	    }
	 }
      }
   }
}
