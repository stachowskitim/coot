/*
 * src/fit-loop.cc
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

#include "positioned-widgets.h"
#include "widget-from-builder.hh"
#include "c-interface-gtk-widgets.h"

#include "graphics-info.h"

// 20221010-PE this file is not compiled

void
new_fill_combobox_with_coordinates_options(GtkWidget *combobox_molecule, GCallback callback_func, int imol_active) {

   auto get_molecule_indices = [] () {
                                  std::vector<int> molecule_indices;
                                  for (int i=0; i<graphics_info_t::n_molecules(); i++) {
                                     if (graphics_info_t::molecules[i].has_model()) {
                                        molecule_indices.push_back(i);
                                     }
                                  }
                                  return molecule_indices;
                               };

   std::vector<int> molecule_indices = get_molecule_indices();

   // gtk_list_store_clear(GTK_TREE(gtk_combo_box_get_model(GTK_COMBO_BOX(combobox_molecule))));

   GtkTreeModel *model_1 = gtk_combo_box_get_model(GTK_COMBO_BOX(combobox_molecule));
   if (GTK_IS_TREE_STORE(model_1))
      gtk_tree_store_clear(GTK_TREE_STORE(model_1));


   GtkListStore *store = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);
   // how do I empty a combobox? model clear.
   GtkTreeIter iter;
   for (unsigned int ii=0; ii<molecule_indices.size(); ii++) {
      const auto &imol = molecule_indices[ii];
      const molecule_class_info_t &m = graphics_info_t::molecules[imol];
      std::string ss = std::to_string(imol) + " " + m.name_for_display_manager();
      gtk_list_store_append(store, &iter);
      gtk_list_store_set(store, &iter, 0, imol, 1, ss.c_str(), -1);
      if (imol == imol_active) {
         gtk_combo_box_set_active(GTK_COMBO_BOX(combobox_molecule), ii);
      }
   }
   GtkTreeModel *model = GTK_TREE_MODEL(store);
   GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
   gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combobox_molecule), renderer, true);
   gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combobox_molecule), renderer, "text", 1, NULL);
   gtk_combo_box_set_model(GTK_COMBO_BOX(combobox_molecule), model);

   if (callback_func)
      g_signal_connect(combobox_molecule, "changed", callback_func, NULL);

}

void
fill_mutate_sequence_dialog_gtkbuilder_version(GtkWidget *dialog) {
   // c.f. wrapped_create_mutate_sequence_dialog()

   auto get_active_molecule_index = [] () {
                                int imol = -1;
                                for (int i=0; i<graphics_info_t::n_molecules(); i++) {
                                   if (graphics_info_t::molecules[i].has_model()) {
                                      imol = i;
                                      break;
                                   }
                                }
                                return imol;
                             };

   set_transient_and_position(COOT_MUTATE_RESIDUE_RANGE_WINDOW, dialog);

   GtkWidget *combobox_molecule = widget_from_builder("mutate_molecule_combobox");
   GtkWidget *combobox_chain    = widget_from_builder("mutate_molecule_chain_combobox");
   GCallback callback_func      = G_CALLBACK(mutate_sequence_molecule_combobox_changed);

   graphics_info_t g;
   int imol = get_active_molecule_index();
   g.mutate_sequence_imol = imol;
   new_fill_combobox_with_coordinates_options(combobox_molecule, callback_func, imol);

   GCallback chain_callback_func = G_CALLBACK(mutate_sequence_chain_combobox_changed); // why do I need this callback?
   g.fill_combobox_with_chain_options(combobox_chain, imol, chain_callback_func);

}

GtkWidget *
create_fit_loop_rama_search_dialog_gtkbuilder_version() {

   GtkWidget *dialog             = widget_from_builder("mutate_sequence_dialog");
   GtkWidget *label              = widget_from_builder("function_for_molecule_label");
   GtkWidget *method_frame       = widget_from_builder("loop_fit_method_frame");
   GtkWidget *mutate_ok_button   = widget_from_builder("mutate_sequence_ok_button");
   GtkWidget *fit_loop_ok_button = widget_from_builder("fit_loop_ok_button");
   GtkWidget *checkbutton        = widget_from_builder("mutate_sequence_do_autofit_checkbutton");
   GtkWidget *rama_checkbutton   = widget_from_builder("mutate_sequence_use_ramachandran_restraints_checkbutton");

   fill_mutate_sequence_dialog_gtkbuilder_version(dialog);

   gtk_label_set_text(GTK_LABEL(label), "\nFit loop in Molecule:\n");
   gtk_widget_set_visible(mutate_ok_button, FALSE);
   gtk_widget_set_visible(checkbutton, FALSE);
   gtk_widget_set_visible(fit_loop_ok_button, TRUE);
   gtk_widget_set_visible(rama_checkbutton, TRUE);
   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rama_checkbutton), TRUE);

   gtk_widget_set_visible(method_frame, TRUE);

   
   return dialog;
}
