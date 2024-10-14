/* src/c-interface-build.cc
 *
 * Copyright 2002, 2003, 2004, 2005, 2006, 2007 The University of York
 * Author: Paul Emsley
 * Copyright 2007 by Paul Emsley
 * Copyright 2007 by the University of Oxford
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


#ifdef USE_PYTHON
#include "Python.h"  // before system includes to stop "POSIX_C_SOURCE" redefined problems
#endif

#include "compat/coot-sysdep.h"

#include <string>
#include <vector>

#include <gtk/gtk.h>
#include "coords/mmdb.hh"
#include "graphics-info.h"
// Including python needs to come after graphics-info.h, because
// something in Python.h (2.4 - chihiro) is redefining FF1 (in
// ssm_superpose.h) to be 0x00004000 (Grrr).
// BL says:: and (2.3 - dewinter), i.e. is a Mac - Python issue
// since the follwing two include python graphics-info.h is moved up
#include "c-interface.h"
#include "c-interface-gtk-widgets.h"

#include "generic-display-objects-c.h"

#include "widget-from-builder.hh"

GMenuModel *main_menumodel() {

   GtkApplication *app = graphics_info_t::application;
   GMenuModel *menubar = gtk_application_get_menubar(app);
   // std::cout << "@@@@@@@ in main_menumodel() returning " << menubar << std::endl;
   return menubar;
}

GtkWidget *main_statusbar() {

   return graphics_info_t::statusbar; // special case, is set when builder is read

}

GtkWidget *main_toolbar() {

   GtkWidget *w = 0;
   if (graphics_info_t::gui_from_gtkbuilder()) { // 20220310-PE no other choice now!
      w = graphics_info_t::get_widget_from_builder("main_window_toolbar_hbox");
      // std::cout << "@@@@@@@ in main_toolbar() looked up " << w << std::endl;
   } else {
      std::cout << "not gui from builder" << std::endl;
   }
   // std::cout << "@@@@@@@ in main_toolbar() returning " << w << std::endl;
   return w;
}

GtkWidget *main_hbox() {

   GtkWidget *w = 0;
   if (graphics_info_t::gui_from_gtkbuilder())  // 20220310-PE no other choice now!
      w = graphics_info_t::get_widget_from_builder("main_window_hbox");
   else
      w = 0;
   std::cout << "@@@@@@@ in main_hbox() returning " << w << std::endl;
   return w;

}



void do_rot_trans_adjustments(GtkWidget *dialog) {
   graphics_info_t g;
   g.do_rot_trans_adjustments(dialog);
}

short int delete_item_widget_is_being_shown() {
   short int r = 0;
   if (graphics_info_t::delete_item_widget != NULL) {
      r = 1;
   }
   return r;
}

short int delete_item_widget_keep_active_on() {
   short int r = 0;
   if (delete_item_widget_is_being_shown()) {

      // GtkWidget *checkbutton = lookup_widget(graphics_info_t::delete_item_widget, "delete_item_keep_active_checkbutton");

      // 20220310-PE this widget may not exist now
      GtkWidget *checkbutton = widget_from_builder("delete_item_keep_active_checkbutton");

      if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton))) {
	 r = 1;
      }
   }
   return r;
}


/*  find the molecule that the single map dialog applies to and set
    the contour level and redraw */
void single_map_properties_apply_contour_level_to_map(GtkWidget *w) {

   std::cout << "DEBUG:: in single_map_properties_apply_contour_level_to_map() needs to set widget data imol " << std::endl;
   int imol = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(w), "imol"));

   if (is_valid_map_molecule(imol)) {
      GtkWidget *sigma_radiobutton = widget_from_builder("single_map_properties_sigma_radiobutton");
      GtkToggleButton *toggle_button = GTK_TOGGLE_BUTTON(sigma_radiobutton);

      // GtkWidget *entry = lookup_widget(w, "single_map_properties_contour_level_entry");
      GtkWidget *entry = widget_from_builder("single_map_properties_contour_level_entry");
      const char *txt = gtk_editable_get_text(GTK_EDITABLE(entry));
      float level = atof(txt);
      if (gtk_toggle_button_get_active(toggle_button)) {
	 set_contour_level_in_sigma(imol, level);
      } else {
	 set_contour_level_absolute(imol, level);
      }
   }
}

#include "remarks-browser-gtk-widgets.hh"

void on_remarks_dialog_response(GtkDialog *dialog,
                                gint       response_id,
                                gpointer   user_data) {

   gtk_widget_set_visible(GTK_WIDGET(dialog), FALSE);

}


/*! \brief a gui dialog showing remarks header info (for a model molecule). */
void remarks_dialog(int imol) {

   std::cout << "::::: remarks_dialog() with imol " << imol << std::endl;

   if (graphics_info_t::use_graphics_interface_flag) {
      if (is_valid_model_molecule(imol)) {
	 mmdb::Manager *mol = graphics_info_t::molecules[imol].atom_sel.mol;
	 if (mol) {

	    GtkWidget *d = gtk_dialog_new();
	    gtk_window_set_title(GTK_WINDOW(d), "Coot Header Browser");
	    g_object_set_data(G_OBJECT(d), "remarks_dialog", d);
	    // is this correct!?
	    // GtkWidget *vbox = GTK_DIALOG(d)->vbox;
	    GtkWidget *vbox = gtk_dialog_get_content_area(GTK_DIALOG(d));
	    GtkWidget *vbox_inner = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
	    GtkWidget *scrolled_window = gtk_scrolled_window_new ();
            gtk_widget_set_vexpand(vbox, TRUE);
            gtk_widget_set_hexpand(scrolled_window, TRUE);
            gtk_widget_set_vexpand(scrolled_window, TRUE);
	    // gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window),
	    //     				  GTK_WIDGET(vbox_inner));
	    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), vbox_inner);
	    gtk_box_append(GTK_BOX(vbox), GTK_WIDGET(scrolled_window));
	    gtk_widget_set_visible(scrolled_window, TRUE);
	    gtk_widget_set_visible(vbox_inner, TRUE);

	    remarks_browser_fill_compound_info(mol, vbox_inner);

	    remarks_browser_fill_author_info(mol, vbox_inner);

	    remarks_browser_fill_journal_info(mol, vbox_inner);

	    remarks_browser_fill_link_info(mol, vbox_inner);

	    mmdb::TitleContainer *tc_p = mol->GetRemarks();
	    int l = tc_p->Length();
	    std::map<int, std::vector<std::string> > remarks;
	    for (int i=0; i<l; i++) {
	       mmdb::Remark *cr = static_cast<mmdb::Remark *> (tc_p->GetContainerClass(i));
	       int rn = cr->remarkNum;
	       std::string s = cr->remark;
	       remarks[rn].push_back(s);
	    }
	    if (! remarks.size()) {
	       info_dialog("WARNING:: No REMARKS");
	    } else {

	       std::map<int, std::vector<std::string> >::const_iterator it;
	       for (it=remarks.begin(); it != remarks.end(); ++it) {
		  std::string remark_name = "REMARK ";
		  remark_name += coot::util::int_to_string(it->first);
		  GtkWidget *frame = gtk_frame_new(remark_name.c_str());
		  gtk_box_append(GTK_BOX(vbox_inner), frame);
		  gtk_widget_set_visible(frame, TRUE);
		  // std::cout << "REMARK number " << it->first << std::endl;
		  GtkTextBuffer *text_buffer = gtk_text_buffer_new(NULL);
		  GtkWidget *text_view = gtk_text_view_new();

		  gtk_widget_set_size_request(GTK_WIDGET(text_view), 400, -1);

		  gtk_frame_set_child(GTK_FRAME(frame), GTK_WIDGET(text_view));
		  gtk_widget_set_visible(GTK_WIDGET(text_view), TRUE);
		  gtk_text_view_set_buffer(GTK_TEXT_VIEW(text_view), text_buffer);
		  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);

		  GdkRGBA colour = remark_number_to_colour(it->first);

                  std::cout << "fix the colour of the remarks view " << std::endl;
		  // gtk_widget_modify_base(GTK_WIDGET(text_view), GTK_STATE_NORMAL, &colour);

		  GtkTextIter end_iter;
		  for (unsigned int itext=0; itext<it->second.size(); itext++) {
		     gtk_text_buffer_get_end_iter(text_buffer, &end_iter);
		     std::string s = it->second[itext];
		     s += "\n";
		     gtk_text_buffer_insert(text_buffer, &end_iter, s.c_str(), -1);
		  }
	       }


	       // GtkWidget *close_button = gtk_button_new_with_label("  Close   ");
	       // GtkWidget *aa = gtk_dialog_get_action_area(GTK_DIALOG(d));
	       // gtk_box_pack_start(GTK_BOX(aa), close_button, FALSE, FALSE, 2);


               // 20211001-PE this is not the "orthodox" method to close/hide the dialog
	       // gtk_widget_set_visible(close_button, TRUE);
               // data used by the callback (to hide this dialog)
               // g_object_set_data(G_OBJECT(close_button), "remarks_dialog", d);

               //  std::cout << "signal connect for close button for dialog d " << d << std::endl;
	       //  g_signal_connect(G_OBJECT(close_button), "clicked",
               // G_CALLBACK(on_remarks_dialog_close_button_clicked), d);

               gtk_dialog_add_button(GTK_DIALOG(d), "Close", GTK_RESPONSE_CLOSE);
               g_signal_connect(d, "response", G_CALLBACK(on_remarks_dialog_response), d);

	       gtk_widget_set_size_request(d, 500, 400);
               set_transient_and_position(COOT_UNDEFINED_WINDOW, d);
	       gtk_widget_set_visible(d, TRUE);
	    }
	 }
      }
   }
}

void remarks_browser_fill_compound_info(mmdb::Manager *mol, GtkWidget *vbox) {

   std::string title = coot::get_title(mol);
   std::vector<std::string> compound_lines = coot::get_compound_lines(mol);

   if (!title.empty()) {
      title = std::string("<b>") + title;
      title += "</b>";
      GtkWidget *label = gtk_label_new(title.c_str());
      gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
      gtk_box_append(GTK_BOX(vbox), label);
      gtk_widget_set_visible(label, TRUE);
   }

   if (compound_lines.size() > 0) {
      std::string compound_label = "Compound";
      GtkWidget *frame = gtk_frame_new(compound_label.c_str());
      gtk_box_append(GTK_BOX(vbox), frame);
      gtk_widget_set_vexpand(frame, TRUE);
      gtk_widget_set_visible(frame, TRUE);
      // this doesn't look right - needs checking.
      std::string s;
      for (std::size_t i=0; i<compound_lines.size(); i++) {
	 s += compound_lines[i];
	 s += "\n"; // needed?
      }
      GtkTextBuffer *text_buffer = gtk_text_buffer_new(NULL);
      GtkWidget *text_view = gtk_text_view_new();
      gtk_widget_set_vexpand(frame, TRUE);
      // gtk_text_view_set_border_window_size(GTK_TEXT_VIEW(text_view), GTK_TEXT_WINDOW_RIGHT, 10);
      gtk_widget_set_margin_start(GTK_WIDGET(text_view), 6);
      gtk_widget_set_margin_end(GTK_WIDGET(text_view), 6);
      gtk_widget_set_margin_top(GTK_WIDGET(text_view), 6);
      gtk_widget_set_margin_bottom(GTK_WIDGET(text_view), 6);
      gtk_widget_set_size_request(GTK_WIDGET(text_view), 400, -1);
      gtk_frame_set_child(GTK_FRAME(frame), GTK_WIDGET(text_view));
      gtk_widget_set_visible(GTK_WIDGET(text_view), TRUE);
      gtk_text_view_set_buffer(GTK_TEXT_VIEW(text_view), text_buffer);
      gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);

      GdkRGBA colour;
      colour.red   = 65535;
      colour.green = 63535;
      colour.blue  = 63535;
      // colour.pixel = 65535;
      // Using CSS is the way to change widget background colours, not like this
      // gtk_widget_modify_base(GTK_WIDGET(text_view), GTK_STATE_NORMAL, &colour);
      // gtk_widget_override_background_color(GTK_WIDGET(text_view), GTK_STATE_FLAG_NORMAL, &colour);

      GtkTextIter end_iter;
      for (unsigned int itext=0; itext<compound_lines.size(); itext++) {
	 gtk_text_buffer_get_end_iter(text_buffer, &end_iter);
	 std::string ss = compound_lines[itext];
	 ss += "\n";
	 gtk_text_buffer_insert(text_buffer, &end_iter, ss.c_str(), -1);
      }
   }
}

void remarks_browser_fill_author_info(mmdb::Manager *mol, GtkWidget *vbox) {

   std::vector<std::string> author_lines;

   access_mol *am = static_cast<access_mol *>(mol); // causes indent problem

   const mmdb::Title *tt = am->GetTitle();
   mmdb::Title *ttmp = const_cast<mmdb::Title *>(tt);
   access_title *at = static_cast<access_title *> (ttmp);
   mmdb::TitleContainer *author_container = at->GetAuthor();
   unsigned int al = author_container->Length();
   for (unsigned int i=0; i<al; i++) {
      mmdb::Author *a_line = mmdb::PAuthor(author_container->GetContainerClass(i));
      if (a_line) {
 	 std::string line(a_line->Line);
	 author_lines.push_back(line);
      }
   }
   // std::cout << "---------------- have " << author_lines.size() << " author lines" << std::endl;
   if (author_lines.size() > 0) {
      GtkWidget *frame = gtk_frame_new("Author");
      gtk_box_append(GTK_BOX(vbox), frame);
      gtk_widget_set_visible(frame, TRUE);

      GtkTextBuffer *text_buffer = gtk_text_buffer_new(NULL);
      GtkWidget *text_view = gtk_text_view_new();

      gtk_widget_set_size_request(GTK_WIDGET(text_view), 400, -1);
      gtk_frame_set_child(GTK_FRAME(frame), GTK_WIDGET(text_view));
      gtk_widget_set_visible(GTK_WIDGET(text_view), TRUE);
      gtk_text_view_set_buffer(GTK_TEXT_VIEW(text_view), text_buffer);
      gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);

       GdkRGBA colour;

      // see CSS comment above
      // gtk_widget_modify_base(GTK_WIDGET(text_view), GTK_STATE_NORMAL, &colour);

      for (unsigned int ij=0; ij<author_lines.size(); ij++) {
	 GtkTextIter end_iter;
	 gtk_text_buffer_get_end_iter(text_buffer, &end_iter);
	 std::string s = author_lines[ij];
	 s += "\n";
	 gtk_text_buffer_insert(text_buffer, &end_iter, s.c_str(), -1);
      }
   }
}

void remarks_browser_fill_journal_info(mmdb::Manager *mol, GtkWidget *vbox) {

   std::vector<std::string> journal_lines;

   access_mol *am = static_cast<access_mol *>(mol); // causes indent problem

   const mmdb::Title *tt = am->GetTitle();
   mmdb::Title *ttmp = const_cast<mmdb::Title *>(tt);
   access_title *at = static_cast<access_title *> (ttmp);
   mmdb::TitleContainer *journal_container = at->GetJournal();
   int jl = journal_container->Length();
   unsigned int al = journal_container->Length();
   for (unsigned int i=0; i<al; i++) {
      mmdb::Journal *j_line = mmdb::PJournal(journal_container->GetContainerClass(i));
      if (j_line) {
	 std::string line(j_line->Line);
	 journal_lines.push_back(line);
      }
   }
   // std::cout << "---------------- have " << journal_lines.size() << " journal_lines" << std::endl;
   if (journal_lines.size() > 0) {
      GtkWidget *frame = gtk_frame_new("Journal");
#if (GTK_MAJOR_VERSION == 3 && GTK_MINOR_VERSION == 94) || (GTK_MAJOR_VERSION == 4)
      gtk_box_append(GTK_BOX(vbox), frame);
#else
      gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 1);
#endif
      gtk_widget_set_visible(frame, TRUE);

      GtkTextBuffer *text_buffer = gtk_text_buffer_new(NULL);
      GtkWidget *text_view = gtk_text_view_new();
#if (GTK_MAJOR_VERSION == 4)
#else
      gtk_text_view_set_border_window_size(GTK_TEXT_VIEW(text_view),
					   GTK_TEXT_WINDOW_RIGHT, 10);
#endif
      gtk_widget_set_size_request(GTK_WIDGET(text_view), 400, -1);
      gtk_frame_set_child(GTK_FRAME(frame), GTK_WIDGET(text_view));
      gtk_widget_set_visible(GTK_WIDGET(text_view), TRUE);
      gtk_text_view_set_buffer(GTK_TEXT_VIEW(text_view), text_buffer);
      gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);

#if (GTK_MAJOR_VERSION == 3 && GTK_MINOR_VERSION == 94) || (GTK_MAJOR_VERSION == 4)
       // 20220528-PE FIXME color
       GdkRGBA colour;
#else
      GdkColor colour;
      colour.red   = 45535;
      colour.green = 49535;
      colour.blue  = 53535;
      colour.pixel = 65535;
#endif
      // see CSS comment above
      // gtk_widget_modify_base(GTK_WIDGET(text_view), GTK_STATE_NORMAL, &colour);

      for (unsigned int ij=0; ij<journal_lines.size(); ij++) {
	 GtkTextIter end_iter;
	 gtk_text_buffer_get_end_iter(text_buffer, &end_iter);
	 std::string s = journal_lines[ij];
	 s += "\n";
	 gtk_text_buffer_insert(text_buffer, &end_iter, s.c_str(), -1);
      }
   }
}

void remarks_browser_fill_link_info(mmdb::Manager *mol, GtkWidget *vbox) {

   int imod = 1;
   mmdb::Model *model_p = mol->GetModel(imod);
   if (model_p) {
      int n_links = model_p->GetNumberOfLinks();
      mmdb::LinkContainer *links = model_p->GetLinks();
      std::cout << "   Model "  << imod << " had " << n_links << " links\n";

      float link_dist = -1;

      if (n_links > 0) {
	 GtkWidget *frame = gtk_frame_new("Links");
#if (GTK_MAJOR_VERSION == 3 && GTK_MINOR_VERSION == 94) || (GTK_MAJOR_VERSION == 4)
	 gtk_box_append(GTK_BOX(vbox), frame);
#else
	 gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 1);
#endif
	 gtk_widget_set_visible(frame, TRUE);

	 GtkTextBuffer *text_buffer = gtk_text_buffer_new(NULL);
	 GtkWidget *text_view = gtk_text_view_new();
	 gtk_text_buffer_create_tag (text_buffer, "monospace",
				     "family", "monospace", NULL);

#if (GTK_MAJOR_VERSION == 3 && GTK_MINOR_VERSION == 94) || (GTK_MAJOR_VERSION == 4)
#else
	 gtk_text_view_set_border_window_size(GTK_TEXT_VIEW(text_view),
					      GTK_TEXT_WINDOW_RIGHT, 10);
#endif
	 gtk_widget_set_size_request(GTK_WIDGET(text_view), 400, -1);
	 gtk_frame_set_child(GTK_FRAME(frame), GTK_WIDGET(text_view));
         
	 gtk_widget_set_visible(GTK_WIDGET(text_view), TRUE);
	 gtk_text_view_set_buffer(GTK_TEXT_VIEW(text_view), text_buffer);
	 gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);

#if (GTK_MAJOR_VERSION == 3 && GTK_MINOR_VERSION == 94) || (GTK_MAJOR_VERSION == 4)
       // 20220528-PE FIXME color
         GdkRGBA colour;
#else
	 GdkColor colour;
	 colour.red   = 45535;
	 colour.green = 53535;
	 colour.blue  = 63535;
	 colour.pixel = 65535;
#endif
         // see CSS comment above
	 // gtk_widget_modify_base(GTK_WIDGET(text_view), GTK_STATE_NORMAL, &colour);

	 for (int ilink=0; ilink<n_links; ilink++) {
	    mmdb::Link *link_p = model_p->GetLink(ilink);
	    if (link_p) {
	       std::string s = "LINK ";

#ifdef MMDB_HAS_LINK_DISTANCE
               link_dist = link_p->dist;
#endif
	       std::string rn1 = link_p->resName1;
	       std::string rn2 = link_p->resName2;

	       // for alignment of monospaced text
	       if (rn1.length() == 1) rn1 += "  ";
	       if (rn1.length() == 2) rn1 += " ";
	       if (rn2.length() == 1) rn2 += "  ";
	       if (rn2.length() == 2) rn2 += " ";

	       s += link_p->atName1;
	       s += " ";
	       s += link_p->aloc1;
	       s += " ";
	       s += rn1;
	       s += " ";
	       s += coot::util::int_to_string(link_p->seqNum1);
	       s += " ";
	       s += link_p->insCode1;
	       s += " ";
	       s += link_p->atName2;
	       s += " ";
	       s += link_p->aloc2;
	       s += " ";
	       s += rn2;
	       s += " ";
	       s += coot::util::int_to_string(link_p->seqNum2);
	       s += " ";
	       s += link_p->insCode2;
	       // symm code
	       s += " ";
	       s += coot::util::float_to_string_using_dec_pl(link_dist, 3);
	       s += "\n";

	       GtkTextIter end_iter;
	       gtk_text_buffer_get_end_iter(text_buffer, &end_iter);
	       // gtk_text_buffer_insert(text_buffer, &end_iter, s.c_str(), -1);

	       gtk_text_buffer_insert_with_tags_by_name(text_buffer, &end_iter,
							s.c_str(), -1,
							"monospace", NULL);
	    }
	 }
      }
   }
}


void
on_remarks_dialog_close_button_clicked     (GtkButton *button,
					    gpointer         user_data)
{

   // 20211001-PE  not used now response is used.

   // GtkWidget *window = lookup_widget(GTK_WIDGET(button), "remarks_dialog");
   // gtk_widget_destroy(window);

   // it's not in the glade file, it's made on the fly

   std::cout << "::::::::::::::: on_remarks_dialog_close_button_clicked() " << std::endl;

   // GtkWidget *dialog = GTK_WIDGET(g_object_get_data(G_OBJECT(button), "remarks_dialog"));
   GtkWidget *dialog = GTK_WIDGET(user_data);
   gtk_widget_set_visible(dialog, FALSE);
}


GdkRGBA remark_number_to_colour(int remark_number) {

   GdkRGBA colour;
   colour.red   = 65535;
   colour.green = 65535;
   colour.blue  = 65535;
   colour.alpha = 65535;
   if (remark_number == 2) {
      colour.blue  = 60000;
   }
   if (remark_number == 3) {
      colour.red   = 60000;
   }
   if (remark_number == 4) {
      colour.green  = 60000;
   }
   if (remark_number == 5) {
      colour.green = 62000;
      colour.blue  = 62000;
   }
   if (remark_number == 280) {
      colour.green  = 61000;
      colour.red    = 62500;
   }
   if (remark_number == 350) {
      colour.green  = 61000;
      colour.blue   = 61500;
   }
   if (remark_number == 465) {
      colour.blue   = 60000;
      colour.green  = 60000;
   }
   return colour;
}


void on_simple_text_dialog_close_button_pressed( GtkWidget *button,
						 GtkWidget *dialog) {

   gtk_widget_set_visible(dialog, FALSE);
}

#include "c-interface-gui.hh"

void simple_text_dialog(const std::string &dialog_title, const std::string &text,
			int geom_x, int geom_y) {

   if (graphics_info_t::use_graphics_interface_flag) {

      GtkWidget *d = gtk_dialog_new();
      g_object_set_data(G_OBJECT(d), "simple_text_dialog", d);
      gtk_window_set_title(GTK_WINDOW (d), dialog_title.c_str());

      GtkWidget *vbox = gtk_dialog_get_content_area(GTK_DIALOG(d)); // new method to get vbox fromm dialog
      GtkWidget *vbox_inner = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
      GtkWidget *scrolled_window = gtk_scrolled_window_new();
      //gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window),
      // GTK_WIDGET(vbox_inner));
      gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), vbox_inner);

      gtk_box_append(GTK_BOX(vbox), GTK_WIDGET(scrolled_window));
      gtk_widget_set_visible(scrolled_window, TRUE);
      gtk_widget_set_visible(vbox_inner, TRUE);
      gtk_widget_set_hexpand(vbox_inner, TRUE);
      gtk_widget_set_vexpand(vbox_inner, TRUE);

      GtkWidget *text_widget = gtk_text_view_new ();
      gtk_widget_set_visible (text_widget, TRUE);
      gtk_box_append (GTK_BOX (vbox_inner), text_widget);
      gtk_text_view_set_editable (GTK_TEXT_VIEW (text_widget), FALSE);
      gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (text_widget), GTK_WRAP_WORD);
      gtk_text_buffer_set_text (gtk_text_view_get_buffer(GTK_TEXT_VIEW (text_widget)),
				text.c_str(), -1);
      gtk_window_set_default_size(GTK_WINDOW(d), geom_x, geom_y);

      GtkWidget *close_button = gtk_dialog_add_button(GTK_DIALOG(d), "Close", 2);
      gtk_widget_set_visible(close_button, TRUE);

      g_signal_connect(G_OBJECT(close_button), "clicked",
 		       G_CALLBACK(on_simple_text_dialog_close_button_pressed),
 		       (gpointer) d);

      set_transient_for_main_window(d);
      gtk_widget_set_visible(d, TRUE);

   }
}

void clear_generic_objects_dialog_pointer() {

   graphics_info_t g;
   g.generic_objects_dialog = NULL;
}

/* Donna's request to do the counts in the Mutate Residue range dialog */
void mutate_molecule_dialog_check_counts(GtkWidget *res_no_1_widget, GtkWidget *res_no_2_widget,
					 GtkWidget *text_widget, GtkWidget *label_widget) {

   if (true) {
      std::cout << "debug:: ===== mutate_molecule_dialog_check_counts()" << std::endl;
      std::cout << "   res_no_1_widget " << res_no_1_widget << std::endl;
      std::cout << "   res_no_2_widget " << res_no_2_widget << std::endl;
      std::cout << "   text_widget " << text_widget << std::endl;
      std::cout << "   label_widget " << label_widget << std::endl;
   }
   if (res_no_1_widget && res_no_2_widget) {
      if (text_widget && label_widget) {
	 std::string rn_1_str = gtk_editable_get_text(GTK_EDITABLE(res_no_1_widget));
	 std::string rn_2_str = gtk_editable_get_text(GTK_EDITABLE(res_no_2_widget));
	 GtkTextBuffer* tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_widget));
	 GtkTextIter startiter;
	 GtkTextIter enditer;
	 char *txt = NULL;
	 gtk_text_buffer_get_iter_at_offset(tb, &startiter, 0);
	 gtk_text_buffer_get_iter_at_offset(tb, &enditer, -1);
	 txt = gtk_text_buffer_get_text(tb, &startiter, &enditer, 0);

	 if (txt) {

	    std::string sequence_str(txt);

	    try {

	       int t1_int = coot::util::string_to_int(rn_1_str);
	       int t2_int = coot::util::string_to_int(rn_2_str);
	       int counts = t2_int - t1_int;
	       int res_no_counts = counts + 1;

	       std::string res_no_diff_count_str("-");
	       std::string sequence_count_str("-");

	       if (counts >= 0)
		  res_no_diff_count_str = coot::util::int_to_string(res_no_counts);

	       int sequence_count = 0;
	       for (std::size_t i=0; i<sequence_str.size(); i++) {
		  char c = sequence_str[i];
		  if (c >= 'a' && c <= 'z') sequence_count++;
		  if (c >= 'A' && c <= 'Z') sequence_count++;
	       }
	       // std::cout << "debug:: sequence_str " << sequence_str << " gives sequence_count " << sequence_count << std::endl;
	       if (sequence_count > 0)
		  sequence_count_str = coot::util::int_to_string(sequence_count);

	       std::string label = "Counts: Residues ";
	       label += res_no_diff_count_str;
	       label += " Sequence: ";
	       label += sequence_count_str;

	       GtkWidget *red_light_widget   = widget_from_builder("mutate_sequence_red_light_image");
	       GtkWidget *green_light_widget = widget_from_builder("mutate_sequence_green_light_image");
	       bool show_green_light = false;
	       if (res_no_counts >= 1) {
		  if (sequence_count >= 1) {
		     if (res_no_counts == sequence_count) {
			label += " counts match";
			show_green_light = true;
		     }
		  }
	       }
	       if (show_green_light) {
		  gtk_widget_set_visible(red_light_widget, FALSE);
		  gtk_widget_set_visible(green_light_widget, TRUE);
	       } else {
		  gtk_widget_set_visible(red_light_widget, TRUE);
		  gtk_widget_set_visible(green_light_widget, FALSE);
	       }

	       gtk_label_set_text(GTK_LABEL(label_widget), label.c_str());

	    }
	    catch (const std::runtime_error &rte) {

	    }
	 } else {
	    std::cout << "Null text" << std::endl;
	 }
      }
   }
}


#include "cc-interface.hh"

/* handle_read_ccp4_map is now a .hh/c++ interface function, so give the callback an internal c function */
int handle_read_ccp4_map_internal(const char *fn, int is_difference_map) {

   int status = 0;
   if (fn) { 
      std::string file_name(fn);
      status = handle_read_ccp4_map(file_name, is_difference_map);
   }
   return status;
}
