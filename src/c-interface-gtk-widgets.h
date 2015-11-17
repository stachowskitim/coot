/* src/c-interface-gtk-widgets.h
 * 
 * Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007 The University of York
 * Copyright 2007 by Paul Emsley
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 by The University of Oxford
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

/* svn $Id: c-interface.h 1458 2007-01-26 20:20:18Z emsley $ */

/*! \file 
  \brief Coot Scripting Interface

  Here is a list of all the scripting interface functions. They are
  described/formatted in c/python format.

  Usually coot is compiled with the guile interpreter, and in this
  case these function names and usage are changed a little, e.g.:

  c-format:
  chain_n_residues("A", 1)

  scheme format:
  (chain-n-residues "A" 1)

  Note the prefix usage of the parenthesis and the lack of comma to
  separate the arguments.

*/

#ifndef C_INTERFACE_GTK_WIDGETS_H
#define C_INTERFACE_GTK_WIDGETS_H

/*
  The following extern stuff here because we want to return the
  filename from the file entry box.  That code (e.g.) 
  on_ok_button_coordinates_clicked (callback.c), is written and
  compiled in c.
 
  But, we need that function to set the filename in mol_info, which 
  is a c++ class.
 
  So we need to have this function external for c++ linking.
 
*/

/* Francois says move this up here so that things don't get wrapped
   twice in C-declarations inside gmp library. Hmm! */
#ifdef __cplusplus
#ifdef USE_GUILE
#include <cstdio> /* for std::FILE in gmp.h for libguile.h */
#include <libguile.h>		/* for SCM type (returned by safe_scheme_command) */
#else
#include <string> /* for std::string; included (sic!) in above for guile */
#endif /*  USE_GUILE */
#endif /* c++ */

#ifndef BEGIN_C_DECLS

#ifdef __cplusplus
#define BEGIN_C_DECLS extern "C" {
#define END_C_DECLS }

#else
#define BEGIN_C_DECLS extern
#define END_C_DECLS
#endif
#endif /* BEGIN_C_DECLS */

BEGIN_C_DECLS

#define COOT_SCHEME_DIR "COOT_SCHEME_DIR"
#define COOT_PYTHON_DIR "COOT_PYTHON_DIR"

void on_filename_filter_toggle_button_toggled (GtkButton       *button,
					      gpointer         user_data);

void add_file_dialog_action_area_vbox(GtkWidget *fileselection);
void add_filename_filter(GtkWidget *fileselection);
void add_save_coordinates_include_hydrogens_and_aniso_checkbutton(GtkWidget *fileselection);

/* where data type:
 0 coords
 1 mtz etc
 2 maps
 (return the button)
*/
GtkWidget *add_filename_filter_button(GtkWidget *fileselection, 
				      short int type);

void add_filechooser_filter_button(GtkWidget *fileselection, 
				      short int data_type);

void add_filechooser_extra_filter_button(GtkWidget *fileselection, 
				      const gchar *name,
                                      const gchar *name2);

gboolean on_filename_filter_key_press_event (GtkWidget       *widget,
					     GdkEventKey     *event,
					     gpointer         user_data);

/* a c callable wrapper to the graphics_info_t function */
void fill_option_menu_with_coordinates_options(GtkWidget *option_menu, 
					       GtkSignalFunc signal_func,
					       int imol_active_position);
void fill_option_menu_with_coordinates_options_unsaved_first(GtkWidget *option_menu, 
							     GtkSignalFunc signal_func,
							     int imol_active_position);
GtkWidget *coot_file_chooser();

GtkWidget *coot_dataset_chooser();

GtkWidget *coot_map_name_chooser();

GtkWidget *coot_save_coords_chooser();

GtkWidget *coot_cif_dictionary_chooser();

GtkWidget *coot_run_script_chooser();

GtkWidget *coot_save_state_chooser();

GtkWidget *coot_save_symmetry_chooser();

GtkWidget *coot_screendump_chooser();

void set_directory_for_coot_file_chooser(GtkWidget *w);

const char *coot_file_chooser_file_name(GtkWidget *widget);

void set_filename_for_filechooserselection(GtkWidget *widget, const gchar *name);

void on_export_map_dialog_ok_button_clicked_cc(GtkButton *button);

/*  ---------------------------------------------------------------------- */
/*                     widget utilities                                    */
/*  ---------------------------------------------------------------------- */
/* section Widget Utilities */
/*! \name Widget Utilities */
/* \{ */
/* return negative if fail */
float get_positive_float_from_entry(GtkEntry *w); 

void handle_filename_filter_gtk2(GtkWidget *widget);

void set_transient_and_position(int window_type, GtkWidget *window);


GtkWidget *main_menubar();
GtkWidget *main_statusbar();
GtkWidget *main_toolbar();

/* entry_info_t entry_to_number(GtkWidget *entry);  */

void manage_refmac_column_selection(GtkWidget *w);
void fill_f_optionmenu_with_expert_options(GtkWidget *f_optionmenu);
void handle_column_label_make_fourier(GtkWidget *column_label_window);
void wrapped_create_run_refmac_dialog();


void on_remarks_dialog_close_button_clicked     (GtkButton *button,
						 gpointer         user_data);
GdkColor remark_number_to_colour(int remark_number); 


GtkWidget *wrapped_create_remarks_browser_molecule_chooser_dialog();
void fill_remarks_browswer_chooser(GtkWidget *w);
void remarks_browswer_molecule_item_select(GtkWidget *item, GtkPositionType pos);

void fill_about_window(GtkWidget *widget);
void add_coot_references_button(GtkWidget *widget);
GtkWidget *wrapped_create_coot_references_dialog();

void fill_references_notebook(GtkToolButton *toolbutton, int reference_id);

 void add_is_difference_map_checkbutton(GtkWidget *fileselection); 
/* the callback for the above: */
void
on_read_map_difference_map_toggle_button_toggled (GtkButton       *button,
						  gpointer         user_data);

void add_recentre_on_read_pdb_checkbutton(GtkWidget *fileselection); 
/* the callback for the above: */
void
on_recentre_on_read_pdb_toggle_button_toggled (GtkButton       *button,
					       gpointer         user_data);

void toolbar_popup_menu(GtkToolbar *toolbar, 
		    GdkEventButton *event_button,
		    gpointer user_data);

void set_model_toolbar_docked_position_callback(GtkWidget *w, gpointer user_data);

 
/* return the dialog if it exists, else null */
GtkWidget *close_model_fit_dialog(GtkWidget *dialog_hbox);
/* use this from the scripting layer to say something to the user (popup). */
GtkWidget *popup_window(const char *s);

 void save_accept_reject_dialog_window_position(GtkWidget *acc_reg_dialog);
void set_accept_reject_dialog(GtkWidget *w); /* used by callbacks to unset the widget */


GtkWidget *wrapped_create_model_fit_refine_dialog(); 

void update_model_fit_refine_dialog_menu(GtkWidget *widget);

void update_model_fit_refine_dialog_buttons(GtkWidget *widget);
/* other tools */
GtkWidget *wrapped_create_other_model_tools_dialog();
void unset_other_modelling_tools_dialog();

GtkWidget *wrapped_create_fast_ss_search_dialog();
  
/* called by above */
GtkWidget *wrapped_create_undo_molecule_chooser_dialog();

void execute_recover_session(GtkWidget *w);

void add_on_map_colour_choices(GtkWidget *w);

/* the callback set on the submenu items in the above function */
void map_colour_mol_selector_activate (GtkMenuItem     *menuitem,
				       gpointer         user_data);
void my_delete_menu_items(GtkWidget *widget, void *data);

/* similarly for the scrollwheel */
void add_on_map_scroll_whell_choices(GtkWidget *menu);
void map_scroll_wheel_mol_selector_activate (GtkMenuItem     *menuitem,
					     gpointer         user_data);

void handle_map_colour_change     (int map_no, gdouble[4]);
void handle_symmetry_colour_change(int mol,    gdouble[4]);
void fill_single_map_properties_dialog(GtkWidget *window, int imol);
void set_contour_sigma_button_and_entry(GtkWidget *window, int imol);
void set_contour_by_sigma_step_maybe(GtkWidget *window, int imol);
void single_map_properties_apply_contour_level_to_map(GtkWidget *w);
void execute_refmac(GtkWidget *window); /* lookup stuff here. */


 GtkWidget *wrapped_create_show_symmetry_window();
void symmetry_colour_adjustment_changed (GtkAdjustment *adj, 
					 GtkWidget *window); 
GtkWidget *symmetry_molecule_controller_dialog();

/* used by destroy callback, needed because there should only be one of these. */
void set_symmetry_controller_dialog_widget(GtkWidget *w); 

void save_symmetry_coords_from_fileselection(GtkWidget *fileselection);

void set_directory_for_fileselection(GtkWidget *coords_fileselection1); 
void save_directory_from_fileselection(const GtkWidget *fileselection);
void save_directory_for_saving_from_fileselection(const GtkWidget *fileselection);
void set_file_for_save_fileselection(GtkWidget *fileselection);

/*  ------------------------------------------------------------------- */
/*                    file selection                                    */
/*  ------------------------------------------------------------------- */

/* we include thes functions for the chooser here */
void set_directory_for_filechooser(GtkWidget *coords_fileselection1); 
void save_directory_from_filechooser(const GtkWidget *fileselection);
void save_directory_for_saving_from_filechooser(const GtkWidget *fileselection);


/* Eleanor likes to sort her files by date when selecting a file */

/* return the button. */
GtkWidget *add_sort_button_fileselection(GtkWidget *fileselection); 

void add_ccp4i_project_optionmenu(GtkWidget *fileselection, int file_selector_type);
void add_ccp4i_projects_to_optionmenu(GtkWidget *optionmenu, int file_selector_type, GtkSignalFunc func);
void add_ccp4i_project_shortcut(GtkWidget *fileselection);
void option_menu_refmac_ccp4i_project_signal_func(GtkWidget *item, GtkPositionType pos);
void run_refmac_ccp4i_option_menu_signal_func(GtkWidget *item, GtkPositionType pos);
void clear_refmac_ccp4i_project();
GtkWidget *lookup_file_selection_widgets(GtkWidget *item, int file_selector_type);

/* We wrote this button/callback by hand, most of the rest are in
   callbacks.c  */

void fileselection_sort_button_clicked( GtkWidget *sort_button,
					GtkWidget *file_list); 

void push_the_buttons_on_fileselection(GtkWidget *filter_button, 
				       GtkWidget *sort_button,
				       GtkWidget *fileselection);
GtkWidget *wrapped_create_run_state_file_dialog();
#ifdef USE_PYTHON
GtkWidget *wrapped_create_run_state_file_dialog_py();
#endif /* USE_PYTHON */

 void do_clipping1_activate();
void clipping_adjustment_changed (GtkAdjustment *adj, GtkWidget *window);

/* widget work */
GtkWidget *wrapped_create_coords_colour_control_dialog();

/* a general purpose version of the above, where we pass a widget flag */
void store_window_position(int window_type, GtkWidget *w);
void store_window_size(int window_type, GtkWidget *w);

void skeletonize_map_by_optionmenu(GtkWidget *optionmenu);
void skeletonize_map_single_map_maybe(GtkWidget *window, int imol); 

GtkWidget *wrapped_create_skeleton_dialog();


 /* set the radio buttons in the frame to the be on or off for the map
   that is displayed in the optionmenu (those menu items "active"
   callbacks (graphics_info::skeleton_map_select change
   g.map_for_skeletonize).  */
void set_on_off_skeleton_radio_buttons(GtkWidget *skeleton_frame);
void set_on_off_single_map_skeleton_radio_buttons(GtkWidget *skeleton_frame, int imol);

void save_coordinates_using_widget(GtkWidget *widget); /* do a get_user_data for
					     the molecule and a lookup
					     of the entry? to find the
					     filename in c-interface,
					     not in the callback.c  */
 
/* not really a button select, its a menu item select */
/* not productive */
void save_molecule_coords_button_select(GtkWidget *item, GtkPositionType pos); 

GtkWidget *wrapped_create_goto_atom_window();
void fill_go_to_atom_window(GtkWidget *widget);

int goto_next_atom_maybe_new(GtkWidget *window);
int goto_previous_atom_maybe_new(GtkWidget *window);

/* used by keypress (return) callbacks */

/*  read the widget values and apply them to the graphics */
 
int apply_go_to_atom_values(GtkWidget * window);

int go_to_atom_molecule_optionmenu_active_molecule(GtkWidget *widget); 

void clear_atom_list(GtkWidget *atom_gtklist);

void apply_go_to_atom_from_widget(GtkWidget *widget);

void
on_go_to_atom_residue_list_select_child (GtkList         *list,
					 GtkWidget       *widget,
					 gpointer         user_data);


void
on_go_to_atom_residue_list_unselect_child (GtkList         *list,
					   GtkWidget       *widget,
					   gpointer         user_data);

void save_display_control_widget_in_graphics(GtkWidget *widget); 

GtkWidget *wrapped_create_display_control_window();

GtkWidget *wrapped_create_merge_molecules_dialog();
void do_merge_molecules_gui();
void do_merge_molecules(GtkWidget *dialog);
void fill_vbox_with_coordinates_options(GtkWidget *vbox,
					GtkSignalFunc checkbox_callback_func);
void merge_molecules_menu_item_activate(GtkWidget *item, 
					GtkPositionType pos);
void on_merge_molecules_check_button_toggled (GtkToggleButton *togglebutton,
					      gpointer         user_data);

/*  ----------------------------------------------------------------------- */
/*                         Mutate Sequence and Loops GUI                    */
/*  ----------------------------------------------------------------------- */
/* section Mutate Sequence and Loops GUI */

GtkWidget *wrapped_create_mutate_sequence_dialog();
void do_mutate_sequence(GtkWidget *dialog); 
void mutate_sequence_molecule_menu_item_activate(GtkWidget *item, 
						 GtkPositionType pos);
/* void fill_chain_option_menu(GtkWidget *chain_option_menu, int imol); */
/* the generic form of the above - also used by superpose chain optionmenu */
/* void fill_chain_option_menu_with_callback(GtkWidget *chain_option_menu, 
					  int imol,
					  GtkSignalFunc callback); */
void mutate_sequence_chain_option_menu_item_activate (GtkWidget *item,
						      GtkPositionType pos);

/* My Rama-Search Loop fit */
GtkWidget *wrapped_fit_loop_rama_search_dialog();
void fit_loop_from_widget(GtkWidget *w);

/* wrapper for Kevin's DB Loop (called from callbacks.c) */
void wrapped_fit_loop_db_loop_dialog();


 
GtkWidget *wrapped_create_align_and_mutate_dialog();
/* return the handled_state, so that we know if we should kill the dialog or not */
int do_align_mutate_sequence(GtkWidget *w);
void align_and_mutate_molecule_menu_item_activate(GtkWidget *item, 
						  GtkPositionType pos);
void align_and_mutate_chain_option_menu_item_activate (GtkWidget *item,
						       GtkPositionType pos);
GtkWidget *wrapped_create_renumber_residue_range_dialog();
void renumber_residues_from_widget(GtkWidget *window);

GtkWidget *wrapped_create_change_chain_id_dialog();
void change_chain_id_by_widget(GtkWidget *w);
void change_chain_ids_mol_option_menu_item_activate(GtkWidget *item,
						    GtkPositionType pos);
void change_chain_ids_chain_menu_item_activate(GtkWidget *item,
					       GtkPositionType pos);
void setup_guile_window_entry(GtkWidget *entry); 
void setup_python_window_entry(GtkWidget *entry); 

/*  Check if this is needed still, I think not. */
#ifdef USE_GUILE
void guile_window_enter_callback( GtkWidget *widget,
				  GtkWidget *entry ); 
#endif /* USE_GUILE */

#ifdef USE_PYTHON
void python_window_enter_callback( GtkWidget *widget,
				   GtkWidget *entry ); 
#endif /* USE_PYTHON */

/*  ----------------------------------------------------------------------- */
/*                  Monomer                                                 */
/*  ----------------------------------------------------------------------- */

/* Accession code */
void handle_get_accession_code(GtkWidget *widget); 
 
/* in here we check if libcheck is available (if scripting is available) */
GtkWidget *wrapped_create_libcheck_monomer_dialog();

/* Libcheck monomer code */
void handle_get_libcheck_monomer_code(GtkWidget *widget); 

int
handle_make_monomer_search(const char *text, GtkWidget *viewport);

/* now we use */
void fill_option_menu_with_refine_options(GtkWidget *option_menu);

GtkWidget *wrapped_create_refine_params_dialog(); 

void do_torsions_toggle(GtkWidget *button);

void set_refine_params_toggle_buttons(GtkWidget *button);

/* Now the refinement weight can be set from an entry in the refine_params_dialog. */
void set_refinemenent_weight_from_entry(GtkWidget *entry);

void check_chiral_volumes_from_widget(GtkWidget *window); 
void fill_chiral_volume_molecule_option_menu(GtkWidget *w);
void chiral_volume_molecule_option_menu_item_select(GtkWidget *item, GtkPositionType pos);


/*  ----------------------------------------------------------------------- */
/*                  Restraints editor                                       */
/*  ----------------------------------------------------------------------- */

GtkWidget *wrapped_create_residue_editor_select_monomer_type_dialog();
void clear_restraints_editor_by_dialog(GtkWidget *dialog); /* close button pressed */


/*  ----------------------------------------------------------------------- */
/*               Move Molecule Here                                        */
/*  ----------------------------------------------------------------------- */
/*! \name move molecule here (wrapper to scheme function) */
/* { */
GtkWidget *wrapped_create_move_molecule_here_dialog();
void move_molecule_here_by_widget(GtkWidget *w);
int move_molecule_to_screen_centre_internal(int imol);
void fill_move_molecule_here_dialog(GtkWidget *w);

/* } */

void apply_residue_info_changes(GtkWidget *widget);
GtkWidget *wrapped_create_residue_info_dialog();
void residue_info_release_memory(GtkWidget *widget); 
GtkWidget *wrapped_create_geometry_dialog();
void store_geometry_dialog(GtkWidget *w);

void fill_environment_widget(GtkWidget *widget);
void execute_environment_settings(GtkWidget *widget);
void toggle_environment_show_distances(GtkToggleButton *button); 
 
void fill_pointer_distances_widget(GtkWidget *widget);
void execute_pointer_distances_settings(GtkWidget *widget);
void toggle_pointer_distances_show_distances(GtkToggleButton *button); 

void set_zoom_adjustment(GtkWidget *w);

void free_geometry_graph(GtkWidget *dialog); /* free the lines in the widget  */
void unset_geometry_graph(GtkWidget *dialog); /* set the graphics info
						 static to NULL, so
						 that we on longer try
						 to update the
						 widget*/

void add_on_validation_graph_mol_options(GtkWidget *menu, const char *type_in);
void my_delete_validaton_graph_mol_option(GtkWidget *widget, void *);
void validation_graph_b_factor_mol_selector_activate (GtkMenuItem     *menuitem,
						      gpointer         user_data);
////B 
void validation_graph_calc_b_factor_mol_selector_activate (GtkMenuItem     *menuitem,
						      gpointer         user_data);
////E 
void validation_graph_geometry_mol_selector_activate (GtkMenuItem     *menuitem,
						      gpointer         user_data);
void validation_graph_omega_mol_selector_activate (GtkMenuItem     *menuitem,
						   gpointer         user_data);
void validation_graph_rotamer_mol_selector_activate (GtkMenuItem     *menuitem,
						   gpointer         user_data);
void validation_graph_density_fit_mol_selector_activate (GtkMenuItem     *menuitem,
						   gpointer         user_data);
void gln_and_asn_b_factor_outlier_mol_selector_activate (GtkMenuItem     *menuitem,
							 gpointer         user_data);
void validation_graph_ncs_diffs_mol_selector_activate (GtkMenuItem     *menuitem,
						       gpointer         user_data);

void probe_mol_selector_activate (GtkMenuItem     *menuitem,
				  gpointer         user_data);

/* These are called right at the beginning (main) */
/* old style not-generic menu initialization */
/* void create_initial_validation_graph_b_factor_submenu(GtkWidget *window1); */
/* void create_initial_validation_graph_geometry_submenu(GtkWidget *window1); */
/* void create_initial_validation_graph_omega_submenu(GtkWidget *window1); */
 
void difference_map_peaks_by_widget(GtkWidget *dialog);
void set_difference_map_peaks_widget(GtkWidget *w);

GtkWidget *wrapped_create_generate_diff_map_peaks_dialog();


/* call with value non-zero for on, 0 for off/not. */

/* This should not be used for scripting. */
 
/*  If called with 0, it checks to see if it was previously non-zero, */
/*  if so, then it does a get_user_data to find the pointer to the */
/*  object and deletes it. */
void my_delete_ramachandran_mol_option(GtkWidget *widget, void *);
void set_dynarama_is_displayed(GtkWidget *dynarama_widget, int imol);
GtkWidget *dynarama_is_displayed_state(int imol);

/*  return -1 on error. */
int get_mol_from_dynarama(GtkWidget *window);


/* FIXME before release! */
/* gcc ... -O2 -MT callbacks.o -MD -MP -MF .deps/callbacks.Tpo -c -o callbacks.o ../../coot/src/callbacks.c */
/* ../../coot/src/callbacks.c: In function ‘on_dynarama_window_configure_event’: */
/* ../../coot/src/callbacks.c:9682:4: warning: passing argument 2 of ‘resize_rama_canvas’ from incompatible pointer type [enabled by default] */
/* ../../coot/src/c-interface.h:4083:6: note: expected ‘union GdkEvent *’ but argument is of type ‘struct GdkEventConfigure *’ */
void resize_rama_canvas(GtkWidget *widget, GdkEventConfigure *event);


GtkWidget *wrapped_ramachandran_plot_differences_dialog();
int  do_ramachandran_plot_differences_by_widget(GtkWidget *w); /* return status */
void fill_ramachandran_plot_differences_option_menu_with_chain_options(GtkWidget *chain_optionmenu, 
								       int is_first_mol_flag);
void ramachandran_plot_differences_mol_option_menu_activate_first(GtkWidget *item, GtkPositionType pos);
void ramachandran_plot_differences_mol_option_menu_activate_second(GtkWidget *item, GtkPositionType pos);
void ramachandran_plot_differences_chain_option_menu_activate_first(GtkWidget *item, GtkPositionType pos);
void ramachandran_plot_differences_chain_option_menu_activate_second(GtkWidget *item, GtkPositionType pos);
 
void set_sequence_view_is_displayed(GtkWidget *widget, int imol); 

void set_ligand_cluster_sigma_level_from_widget(GtkWidget *button);

void free_ligand_search_user_data(GtkWidget *button); 

/* Widget functions */

int fill_ligands_dialog(GtkWidget *dialog);
int fill_ligands_dialog_map_bits(GtkWidget *dialog, short int diff_maps_only_flag);	
int fill_ligands_dialog_protein_bits(GtkWidget *dialog);	
int fill_ligands_dialog_ligands_bits(GtkWidget *dialog);	
/*  we need to delete the find_ligand_dialog when we are done, so  */
/*  add this pointer as user data. */
void do_find_ligand_many_atoms_in_ligands(GtkWidget *find_ligand_dialog); 
/* these I factored out, they can be used for the waters dialog too */
int fill_ligands_dialog_map_bits_by_dialog_name(GtkWidget *find_ligand_dialog,
						const char *dialog_name, 
						short int diff_maps_only_flag); 
int fill_ligands_dialog_protein_bits_by_dialog_name(GtkWidget *find_ligand_dialog,
						    const char *dialog_name); 
int fill_vbox_with_coords_options_by_dialog_name(GtkWidget *find_ligand_dialog,
						 const char *dialog_name,
						 short int have_ncs_flag);
void fill_ligands_sigma_level_entry(GtkWidget *dialog);
void fill_ligands_expert_options(GtkWidget *find_ligand_dialog);
void set_ligand_expert_options_from_widget(GtkWidget *button);

 
/*  info is stored in graphics_info_t beforehand */
void execute_get_mols_ligand_search(GtkWidget *button); 

/* This has pointers to Coord_orths poked into it, let's clear them
   up. */
void  free_blob_dialog_memory(GtkWidget *w);

void wrapped_create_find_waters_dialog();
void fill_find_waters_dialog(GtkWidget *find_ligand_dialog);
/* interface fluff */
void execute_find_waters(GtkWidget *ok_button);  

void on_big_blob_button_clicked(GtkButton *button, gpointer user_data);

void execute_find_blobs_from_widget(GtkWidget *dialog);

GtkWidget *wrapped_create_unmodelled_blobs_dialog();

GtkWidget *wrapped_create_bond_parameters_dialog();
void apply_bond_parameters(GtkWidget *w);

GtkWidget *wrapped_create_add_additional_representation_gui();
void add_additional_representation_by_widget(GtkWidget *w);
void add_reps_molecule_option_menu_item_select(GtkWidget *item, GtkPositionType pos);

void   set_map_dynamic_map_sampling_checkbutton(GtkWidget *checkbutton);
void   set_map_dynamic_map_display_size_checkbutton(GtkWidget *checkbutton);
void store_delete_item_widget(GtkWidget *widget);
/* We need to set the pending delete flag and that can't be done in
   callback, so this wrapper does it */
GtkWidget *wrapped_create_delete_item_dialog();

void do_rot_trans_adjustments(GtkWidget *dialog);

/* get the molecule to delete from the optionmenu */
void close_molecule_by_widget(GtkWidget *optionmenu);
void fill_close_option_menu_with_all_molecule_options(GtkWidget *optionmenu);
/* The callback for the above menuitems */
void close_molecule_item_select(GtkWidget *item, GtkPositionType pos); 

/* New version of close molecule */
void new_close_molecules(GtkWidget *window);
GtkWidget *wrapped_create_new_close_molecules_dialog();

/* Used for unsetting the rotamer dialog when it gets destroyed. */
void
set_graphics_rotamer_dialog(GtkWidget *w);

void setup_alt_conf_with_dialog(GtkWidget *dialog); 

void fill_place_atom_molecule_option_menu(GtkWidget *optionmenu);
 
void set_baton_build_params_from_widget(GtkWidget *params_dialog);
void baton_mode_calculate_skeleton(GtkWidget *window);

GtkWidget *wrapped_create_add_OXT_dialog();
void apply_add_OXT_from_widget(GtkWidget *w);
 

/* not for user consumption, this finds (from itself) the residue type
   and calls the graphics_info_t function. */
void fill_chi_angles_vbox(GtkWidget *vbox);
 GtkWidget *wrapped_create_check_waters_dialog();
void check_waters_molecule_menu_item_activate(GtkWidget *item, 
					      GtkPositionType pos);
void check_water_by_difference_maps_option_menu_item_select(GtkWidget *item, 
							    GtkPositionType pos);
void do_check_waters_by_widget(GtkWidget *dialog);
void store_checked_waters_baddies_dialog(GtkWidget *dialog);

GtkWidget *wrapped_checked_waters_baddies_dialog(int imol, float b_factor_lim, 
						 float map_sigma_lim, 
						 float min_dist, float max_dist,
						 short int part_occ_contact_flag,
						 short int zero_occ_flag,
						 short int logical_operator_and_or_flag);
GtkWidget *wrapped_create_lsq_plane_dialog();
GtkWidget *wrapped_create_least_squares_dialog();
int apply_lsq_matches_by_widget(GtkWidget *lsq_dialog); /* return 1 for good fit */
void lsq_ref_mol_option_menu_changed(GtkWidget *item, GtkPositionType pos);
void lsq_mov_mol_option_menu_changed(GtkWidget *item, GtkPositionType pos);
void lsq_reference_chain_option_menu_item_activate(GtkWidget *item,
						   GtkPositionType pos);
void lsq_moving_chain_option_menu_item_activate(GtkWidget *item,
						GtkPositionType pos);
void fill_lsq_option_menu_with_chain_options(GtkWidget *chain_optionmenu, 
					     int is_reference_structure_flag,
					     const char *active_chain_id);

void execute_superpose(GtkWidget *w);
GtkWidget *wrapped_create_superpose_dialog(); /* used by callback */
void fill_superpose_option_menu_with_chain_options(GtkWidget *chain_optionmenu, 
 						   int is_reference_structure_flag);
void update_lsq_dialog_store_values(GtkWidget *w);

 
int make_dynamically_transformed_ncs_maps_by_widget(GtkWidget *dialog);
GtkWidget *wrapped_create_ncs_maps_dialog();

GtkWidget *wrapped_create_ncs_control_dialog();	
void ncs_control_change_ncs_master_to_chain_update_widget(GtkWidget *w, int imol, int ichain); 

GtkWidget *wrapped_nucleotide_builder_dialog();
void ideal_nucleic_acid_by_widget(GtkWidget *builder_dialog);

int coot_socket_listener_idle_func(GtkWidget *w);

GtkWidget *wrapped_create_map_sharpening_dialog();
void map_sharpening_map_select(GtkWidget *item, GtkPositionType pos);
void map_sharpening_value_changed (GtkAdjustment *adj, GtkWidget *window);
int fill_option_menu_with_map_options(GtkWidget *option_menu, GtkSignalFunc signalfunc);
int fill_option_menu_with_map_mtz_options(GtkWidget *option_menu, GtkSignalFunc signalfunc);

void calc_and_set_optimal_b_factor ( GtkWidget *w ) ;

void store_fixed_atom_dialog(GtkWidget *w);
GtkWidget *wrapped_create_fixed_atom_dialog();

void start_ligand_builder_gui_internal(GtkMenuItem     *menuitem,
				       gpointer         user_data);
 
void set_file_selection_dialog_size(GtkWidget *w);

GtkWidget *main_window(); 
void save_go_to_atom_widget(GtkWidget *widget); /* store in a static */


 
/*  this is the option menu callback - does nothing. */
void refmac_molecule_button_select(GtkWidget *item, GtkPositionType pos); 
int set_refmac_molecule(int imol); /* used by callback.c */
void fill_option_menu_with_refmac_options(GtkWidget *optionmenu);
void fill_option_menu_with_refmac_methods_options(GtkWidget *optionmenu);
void fill_option_menu_with_refmac_phase_input_options(GtkWidget *optionmenu);
void fill_option_menu_with_refmac_labels_options(GtkWidget *optionmenu);
void fill_option_menu_with_refmac_file_labels_options(GtkWidget *optionmenu);
void fill_option_menu_with_refmac_ncycle_options(GtkWidget *optionmenu);

void update_refmac_column_labels_frame(GtkWidget *optionmenu, 
				       GtkWidget *fobs_menu, GtkWidget *fiobs_menu, GtkWidget *fpm_menu,
				       GtkWidget *f_free_menu,
				       GtkWidget *phases_menu, GtkWidget *fom_menu, GtkWidget *hl_menu);


void free_memory_run_refmac(GtkWidget *window); 

/* some methods to get refmac run parameters */
int get_refmac_refinement_method(void);
void set_refmac_refinement_method(int method);
int get_refmac_phase_input(void);
void set_refmac_phase_input(int phase_flag);
void set_refmac_use_tls(int state);
int refmac_use_tls_state(void);
void set_refmac_use_twin(int state);
int refmac_use_twin_state(void);
void set_refmac_use_sad(int state);
int refmac_use_sad_state(void);
int get_refmac_ncycles(void);
void set_refmac_ncycles(int no_cycles);
void add_refmac_ncycle_no(int cycle);
void set_refmac_use_ncs(int state);
int refmac_use_ncs_state(void);
void set_refmac_use_intensities(int state);
int refmac_use_intensities_state(void);
int refmac_imol_coords(void);
void add_refmac_sad_atom(const char *atom_name, float fp, float fpp, float lambda);
void add_refmac_sad_atom_fp(const char *atom_name, float fp, float fpp);
void add_refmac_sad_atom_lambda(const char *atom_name, float lambda);
void clear_refmac_sad_atoms();
void store_refmac_mtz_file_label(GtkWidget *label);
GtkWidget *get_refmac_mtz_file_label(void);
void fill_refmac_sad_atom_entry(GtkWidget *widget);
short int get_refmac_used_mtz_file_state();
void set_refmac_used_mtz_file(int state);
const gchar *get_saved_refmac_file_filename(void);
void set_stored_refmac_file_mtz_filename(int imol, const char *mtz_filename);
void save_refmac_params_to_map(int imol_map,
			       const char *mtz_filename,
			       const char *fobs_col,
			       const char *sigfobs_col,
			       const char *r_free_col,
			       int r_free_flag_sensible);
void save_refmac_phase_params_to_map(int imol_map,
			     	     const char *phi,
				     const char *fom,
				     const char *hla,
				     const char *hlb,
				     const char *hlc,
				     const char *hld);


#endif /* C_INTERFACE_GTK_WIDGETS_H */
END_C_DECLS
