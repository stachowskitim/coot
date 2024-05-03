/*
 * src/interface.h
 *
 * Copyright 2007 by University of York
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
#ifndef BEGIN_C_DECLS
#ifdef __cplusplus
#define BEGIN_C_DECLS extern "C" {
#define END_C_DECLS }
#else
#define BEGIN_C_DECLS
#define END_C_DECLS
#endif
#endif

BEGIN_C_DECLS
/*
 * Todo: remove this file? I guess
 */

// GtkWidget* create_window1 (void);
// GtkWidget* create_column_label_window (void);
// GtkWidget* create_clipping_window (void);
// GtkWidget* create_global_map_properties_window (void);
// GtkWidget* create_fps_window (void);
// GtkWidget* create_active_map_window (void);
// GtkWidget* create_show_symmetry_window (void);
// GtkWidget* create_about_window (void);
// GtkWidget* create_aniso_window (void);
// GtkWidget* create_smooth_scroll_window (void);
// GtkWidget* create_font_size_window (void);
// GtkWidget* create_phs_info_box (void);
// GtkWidget* create_display_control_window_glade (void);
// GtkWidget* create_goto_atom_window (void);
// GtkWidget* create_skeletonization_box_radius_window (void);
// GtkWidget* create_skeletonization_level_window (void);
// GtkWidget* create_rotation_centre_cube_size_window (void);
// GtkWidget* create_phs_cell_choice_window (void);
// GtkWidget* create_accession_code_window (void);
// GtkWidget* create_dynarama_window (void);
// GtkWidget* create_dynarama_properties_window (void);
// GtkWidget* create_find_ligand_dialog (void);
// GtkWidget* create_find_ligand_many_atoms_dialog (void);
// GtkWidget* create_model_refine_dialog (void);
// GtkWidget* create_rotate_translate_obj_dialog (void);
// GtkWidget* create_save_coords_dialog (void);
// GtkWidget* create_refine_params_dialog (void);
// GtkWidget* create_accept_reject_refinement_dialog (void);
// GtkWidget* create_find_waters_dialog (void);
// GtkWidget* create_environment_distance_dialog (void);
// GtkWidget* create_residue_type_chooser_window (void);
// GtkWidget* create_delete_item_dialog (void);
// GtkWidget* create_residue_info_dialog (void);
// GtkWidget* create_hints_dialog (void);
// GtkWidget* create_rotamer_selection_dialog (void);
// GtkWidget* create_unsaved_changes_dialog (void);
// GtkWidget* create_baton_dialog (void);
// GtkWidget* create_read_pdb_recentre_dialog (void);
// GtkWidget* create_pointer_atom_type_dialog (void);
// GtkWidget* create_run_refmac_dialog (void);
// GtkWidget* create_single_map_properties_dialog (void);
// GtkWidget* create_undo_molecule_chooser_dialog (void);
// GtkWidget* create_skeleton_dialog (void);
// GtkWidget* create_splash_screen_window (void);
// GtkWidget* create_crosshairs_dialog (void);
// GtkWidget* create_add_alt_conf_dialog (void);
// GtkWidget* create_validation_dialog (void);
// GtkWidget* create_run_state_file_dialog (void);
// GtkWidget* create_edit_backbone_torsions_dialog (void);
// GtkWidget* create_start_workflow_dialog (void);
// GtkWidget* create_select_fitting_map_dialog (void);
// GtkWidget* create_run_refmac_help_dialog (void);
// GtkWidget* create_no_restraints_info_dialog (void);
// GtkWidget* create_no_cif_dictionary_bonds_dialog (void);
// GtkWidget* create_ligand_big_blob_dialog (void);
// GtkWidget* create_edit_chi_angles_dialog (void);
// GtkWidget* create_check_waters_dialog (void);
// GtkWidget* create_geometry_dialog (void);
// GtkWidget* create_new_ligands_info_dialog (void);
// GtkWidget* create_no_new_ligands_info_dialog (void);
// GtkWidget* create_zoom_dialog (void);
// GtkWidget* create_libcheck_monomer_dialog (void);
// GtkWidget* create_recover_coordinates_dialog (void);
// GtkWidget* create_centre_atom_label_dialog (void);
// GtkWidget* create_chi_angle_help_dialog (void);
// GtkWidget* create_no_symmetry_warning_dialog (void);
// GtkWidget* create_nothing_to_recover_dialog (void);
// GtkWidget* create_superpose_dialog (void);
// GtkWidget* create_superpose_nonsense_dialog (void);
// GtkWidget* create_add_terminal_residue_finds_none_dialog (void);
// GtkWidget* create_no_bad_chiral_volumes_dialog (void);
// GtkWidget* create_check_chiral_volumes_dialog (void);
// GtkWidget* create_bad_chiral_volumes_dialog (void);
// GtkWidget* create_rigid_body_refinement_failed_dialog (void);
// GtkWidget* create_baton_mode_make_skeleton_dialog (void);
// GtkWidget* create_merge_molecules_dialog (void);
// GtkWidget* create_mutate_sequence_dialog (void);
// GtkWidget* create_bond_parameters_dialog (void);
// GtkWidget* create_renumber_residue_range_dialog (void);
// GtkWidget* create_add_OXT_dialog (void);
// GtkWidget* create_ligand_no_blobs_dialog (void);
// GtkWidget* create_new_close_molecules_dialog (void);
// GtkWidget* create_unmodelled_blobs_dialog (void);
// GtkWidget* create_chiral_restraints_problem_dialog (void);
// GtkWidget* create_check_waters_diff_map_dialog (void);
// GtkWidget* create_interesting_waters_by_difference_map_check_dialog (void);
// GtkWidget* create_nothing_bad_dialog (void);
// GtkWidget* create_skeletonize_map_dialog (void);
// GtkWidget* create_antialiasing_dialog (void);
// GtkWidget* create_geometry_graphs_dialog (void);
// GtkWidget* create_stereo_dialog (void);
// GtkWidget* create_preferences (void);
// GtkWidget* create_diff_map_peaks_dialog (void);
// GtkWidget* create_generate_diff_map_peaks_dialog (void);
// GtkWidget* create_ncs_maps_dialog (void);
// GtkWidget* create_popup_info_window (void);
// GtkWidget* create_pointer_distances_dialog (void);
// GtkWidget* create_align_and_mutate_dialog (void);
// GtkWidget* create_ramachandran_plot_differences_dialog (void);
// GtkWidget* create_checked_waters_baddies_dialog (void);
// GtkWidget* create_nucleic_acid_base_chooser_dialog (void);
// GtkWidget* create_change_chain_id_dialog (void);
// GtkWidget* create_doc_urls_dialog (void);
// GtkWidget* create_other_model_tools_dialog (void);
// GtkWidget* create_symmetry_controller_dialog (void);
// GtkWidget* create_ncs_control_dialog (void);
// GtkWidget* create_lsq_plane_dialog (void);
// GtkWidget* create_coords_colour_control_dialog (void);
// GtkWidget* create_nucleotide_builder_dialog (void);
// GtkWidget* create_move_molecule_here_dialog (void);
// GtkWidget* create_monomer_search_dialog (void);
// GtkWidget* create_least_squares_dialog (void);
// GtkWidget* create_sft_dialog (void);
// GtkWidget* create_aboutdialog (void);
// GtkWidget* create_globularize_dialog (void);
// GtkWidget* create_splash_screen_alternate_1_window (void);
// GtkWidget* create_coords_filechooserdialog1 (void);
// GtkWidget* create_dataset_filechooserdialog1 (void);
// GtkWidget* create_map_name_filechooserdialog1 (void);
// GtkWidget* create_phs_coordinates_filechooserdialog1 (void);
// GtkWidget* create_save_coords_filechooserdialog1 (void);
// GtkWidget* create_cif_dictionary_filechooserdialog1 (void);
// GtkWidget* create_run_script_filechooserdialog1 (void);
// GtkWidget* create_save_symmetry_coords_filechooserdialog1 (void);
// GtkWidget* create_save_state_filechooserdialog1 (void);
// GtkWidget* create_screendump_filechooserdialog1 (void);
// GtkWidget* create_fixed_atom_dialog (void);
// GtkWidget* create_add_reps_dialog (void);
// GtkWidget* create_display_additional_representations_dialog (void);
// GtkWidget* create_restraints_editor_dialog (void);
// GtkWidget* create_residue_editor_select_monomer_type_dialog (void);
// GtkWidget* create_save_restraint_chooserdialog (void);
// GtkWidget* create_run_refmac_nolabels_help_dialog (void);
// GtkWidget* create_coot_references_dialog (void);
// GtkWidget* create_run_refmac_file_help_dialog (void);
// GtkWidget* create_run_refmac_sad_help_dialog (void);
// GtkWidget* create_run_refmac_mtz_filechooserdialog (void);
// GtkWidget* create_fast_ss_search_dialog (void);
// GtkWidget* create_map_sharpening_dialog (void);
// GtkWidget* create_baton_build_params_dialog (void);
// GtkWidget* create_pisa_interfaces_dialog (void);
// GtkWidget* create_scheme_window (void);
// GtkWidget* create_remarks_browser_molecule_chooser_dialog (void);
// GtkWidget* create_fix_nomenclature_errors_dialog (void);
// GtkWidget* create_multi_residue_torsion_dialog (void);
// GtkWidget* create_multi_residue_torsion_pick_dialog (void);
// GtkWidget* create_keyboard_goto_residue_window (void);
// GtkWidget* create_mogul_geometry_results_table_dialog (void);
// GtkWidget* create_build_na_dialog (void);
// GtkWidget* create_ligand_check_dialog (void);
// GtkWidget* create_generic_objects_dialog (void);
// GtkWidget* create_export_map_dialog (void);
// GtkWidget* create_export_map_filechooserdialog (void);
// GtkWidget* create_cfc_dialog (void);
// GtkWidget* create_curlew_dialog (void);
// GtkWidget* create_refinement_restraints_sliders_dialog (void);
// GtkWidget* create_simple_refmac_dialog (void);
// GtkWidget* create_simple_refmac_filechooserdialog (void);
END_C_DECLS
