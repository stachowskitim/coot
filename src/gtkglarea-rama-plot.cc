/* src/gtkglarea-rama-plot.cc
 *
 * Copyright 2022 by Paul Emsley
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

// this is the Gtk Wrapper of the OpengGL Rama plot

#include "graphics-info.h"

#include "gtkglarea-rama-plot.hh"
#include "gl-rama-plot.hh"
#include "widget-from-builder.hh"

void
gtkgl_rama_realize(GtkWidget *gtk_gl_area) {

   if (!gtk_gl_area) return;

   // ----- expand the Rama pane if needed

   graphics_info_t g;
   if (!g.rama_plot_boxes.empty()) {
      GtkWidget *paned = widget_from_builder("main_window_graphics_rama_vs_graphics_pane");
      int position = gtk_paned_get_position(GTK_PANED(paned));
      // std::cout << ":::::::::::: in gtkgl_rama_realize() the paned had position " << position << std::endl;
      if (position < 10) {
         gtk_paned_set_position(GTK_PANED(paned), 400);
         // std::cout << ":::::::::::: gtk_paned_set_position 400 here " << std::endl;
      }
   }

   // ----- find the right rama plot box for this gtk_gl_area and set up the rama
   //       in it with the molecule from the imol in the box.

   bool done = false;
   for (unsigned int i=0; i<g.rama_plot_boxes.size(); i++) {
      auto &rama_box = g.rama_plot_boxes[i];
      if (rama_box.gtk_gl_area == gtk_gl_area) {

         // Dangerous for main window code
         // GdkGLContext *context = gtk_gl_area_get_context(GTK_GL_AREA(gtk_gl_area)); // needed?
         gtk_gl_area_make_current(GTK_GL_AREA (gtk_gl_area));

         rama_box.rama.setup_buffers(0.9);
         int imol = g.rama_plot_boxes[i].imol;
         const std::string residue_selection = rama_box.residue_selection;
         auto &m = graphics_info_t::molecules[imol];
         gl_rama_plot_t::draw_mode_t draw_mode = gl_rama_plot_t::draw_mode_t::DRAW_MODE;
         g.rama_plot_boxes[i].rama.setup_from(imol, m.atom_sel.mol, residue_selection, draw_mode);
         done = true;
      }
   }

   if (!done) {
      std::cout << "WARNING:: oops - failed to setup in gtkgl_rama_realize() " << gtk_gl_area
                << " with " << g.rama_plot_boxes.size() << " rama-boxs " << std::endl;
   }

}

void
gtkgl_rama_unrealize(GtkWidget *gl_area) {


}

void
gtkgl_rama_on_glarea_render(GtkWidget *gtk_gl_area) {

   graphics_info_t g;
   for (unsigned int i=0; i<g.rama_plot_boxes.size(); i++) {
      if (g.rama_plot_boxes[i].gtk_gl_area == gtk_gl_area) {

         // Dangerous for main window code
         // GdkGLContext *context = gtk_gl_area_get_context(GTK_GL_AREA(gtk_gl_area)); // needed?
         gtk_gl_area_make_current(GTK_GL_AREA (gtk_gl_area));
         bool need_clear = true; // this is a "validation" Rama plot and has its own context

         GtkAllocation allocation;
         gtk_widget_get_allocation(GTK_WIDGET(gtk_gl_area), &allocation);
         int w = allocation.width;
         int h = allocation.height;

         g.rama_plot_boxes[i].rama.draw(&g.shader_for_rama_plot_axes_and_ticks,
                                        &g.shader_for_rama_plot_phi_phis_markers, // instanced
                                        &g.shader_for_hud_image_texture,
                                        w, h, w, h, need_clear); // background texture (not text!), uses window_resize_position_correc
      }
   }
}

void
gtkgl_rama_on_glarea_resize(GtkWidget *gl_area, gint width, gint height) {

   std::cout << "resize gl rama to " << width << " " << height << std::endl;
}


void show_opengl_ramachandran_plot(int imol, const std::string &residue_selection) {

   // find a better name for this function?
   //
   // We have come from on_ramachandran_plot_molecule_chooser_ok_button_clicked()
   // i.e. we have just selected a "validation" rama plot to display.
   // These Rama plots have their own glarea context.

   auto on_rama_glarea_click = +[] (GtkGestureClick* click_gesture,
                                   gint n_press,
                                   gdouble x,
                                   gdouble y,
                                   gpointer user_data) {

      GtkWidget *gl_area = GTK_WIDGET(user_data);

      GtkAllocation allocation;
      gtk_widget_get_allocation(gl_area, &allocation);
      int w = allocation.width;
      int h = allocation.height;

      // std::cout << "Rama click! " << x << " " << y << " width " << w << " height " << h << std::endl;

      // Find the right rama plot and get the mouse-over hit, and if it was a residue, go there.
      graphics_info_t g;
      for (unsigned int i=0; i<g.rama_plot_boxes.size(); i++) {
         const auto &rama_box = g.rama_plot_boxes[i];
         if (rama_box.matches_gl_area(gl_area)) {
            auto rama_plot_hit = rama_box.rama.get_mouse_over_hit(x, y, w, h);
            if (rama_plot_hit.residue_was_clicked) {
               int imol = rama_box.imol;
               g.go_to_residue(imol, rama_plot_hit.residue_spec);
            }
         }
      }
   };

   graphics_info_t g;
   if (g.is_valid_model_molecule(imol)) {

      GtkWidget *scrolled = widget_from_builder("ramachandran_plots_scrolled_window");
      gtk_widget_set_visible(scrolled, TRUE);

      GtkWidget *pane_to_show  = widget_from_builder("main_window_ramchandran_and_validation_pane");
      gtk_widget_set_visible(pane_to_show,  TRUE);

      GtkWidget *box_for_all_plots = widget_from_builder("ramachandran_plots_vbox");

      GtkWidget *box_for_this_plot = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
      GtkWidget *gl_area = gtk_gl_area_new();
      GtkWidget *close_button = gtk_button_new_with_label("Close");
      GtkWidget *box_for_selection = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
      GtkWidget *selection_label = gtk_label_new("Selection: ");
      GtkWidget *selection_entry = gtk_entry_new();
      GtkWidget *outliers_only_checkbutton = gtk_check_button_new_with_label("Outliers Only");
      gtk_editable_set_text(GTK_EDITABLE(selection_entry), residue_selection.c_str());
      gtk_widget_set_margin_start(selection_label, 6);
      gtk_widget_set_margin_start(box_for_this_plot, 6);
      gtk_widget_set_margin_start(close_button, 8);
      gtk_widget_set_margin_end(close_button, 8);

      gtk_widget_set_margin_start(outliers_only_checkbutton, 6);
      gtk_widget_set_margin_end(outliers_only_checkbutton, 6);

      gl_rama_plot_t rama;
      graphics_info_t::widgeted_rama_plot_t wr(imol, residue_selection, rama, gl_area, close_button, box_for_this_plot);
      g.rama_plot_boxes.push_back(wr);

      gtk_widget_set_size_request(gl_area, 400, 400);
      g_signal_connect(gl_area, "realize",   G_CALLBACK(gtkgl_rama_realize),   NULL);
      g_signal_connect(gl_area, "unrealize", G_CALLBACK(gtkgl_rama_unrealize), NULL);
      g_signal_connect(gl_area, "render",    G_CALLBACK(gtkgl_rama_on_glarea_render),  NULL);
      g_signal_connect(gl_area, "resize",    G_CALLBACK(gtkgl_rama_on_glarea_resize),  NULL);

      gtk_widget_set_can_focus(gl_area, TRUE);
      gtk_widget_set_focusable(gl_area, TRUE);

      gtk_widget_set_hexpand(gl_area, FALSE);
      gtk_widget_set_vexpand(gl_area, FALSE);

      GtkGesture *click_controller          = gtk_gesture_click_new();
      gtk_widget_add_controller(GTK_WIDGET(gl_area), GTK_EVENT_CONTROLLER(click_controller));
      g_signal_connect(click_controller, "pressed",  G_CALLBACK(on_rama_glarea_click), gl_area);

      auto selection_entry_activate_callback = +[] (GtkWidget *entry, gpointer user_data) {
         std::string entry_string = gtk_editable_get_text(GTK_EDITABLE(entry));
         std::cout << "Now do something with " << entry_string << std::endl;
         GtkWidget *box_for_this_rama_plot = GTK_WIDGET(user_data);
         graphics_info_t::rama_plot_boxes_handle_molecule_update(box_for_this_rama_plot, entry_string);
         graphics_info_t::draw_rama_plots();
      };

      g_signal_connect(G_OBJECT(selection_entry), "activate", G_CALLBACK(selection_entry_activate_callback), box_for_this_plot);

      auto close_callback = +[] (G_GNUC_UNUSED GtkWidget *close_button, gpointer user_data) {
         GtkWidget *box_for_all_plots = widget_from_builder("ramachandran_plots_vbox");
         GtkWidget *box_for_this_plot = GTK_WIDGET(user_data);
         graphics_info_t g;
         g.remove_plot_from_rama_plots(box_for_this_plot); 
         gtk_box_remove(GTK_BOX(box_for_all_plots), box_for_this_plot);
         GtkWidget *scrolled = widget_from_builder("ramachandran_plots_scrolled_window");
         gtk_widget_set_visible(scrolled, FALSE);
         graphics_info_t::hide_vertical_validation_frame_if_appropriate();
      };

      auto outliers_only_checkbutton_toggled = +[] (GtkCheckButton *button, gpointer user_data) {
         GtkWidget *box = GTK_WIDGET(user_data);
         graphics_info_t g;
         std::vector<graphics_info_t::widgeted_rama_plot_t>::iterator it;
         for (it=g.rama_plot_boxes.begin(); it!=g.rama_plot_boxes.end(); ++it) {
            GtkWidget *this_plot_box = it->box;
            if (this_plot_box == box) {
               if (gtk_check_button_get_active(button)) {
                  it->rama.set_outliers_only(true);
               } else {
                  it->rama.set_outliers_only(false);
               }
            }
         }
         graphics_info_t::draw_rama_plots();
      };

      g_signal_connect(G_OBJECT(close_button), "clicked", G_CALLBACK(close_callback), box_for_this_plot);
      g_signal_connect(G_OBJECT(outliers_only_checkbutton), "toggled", G_CALLBACK(outliers_only_checkbutton_toggled), box_for_this_plot);

      gtk_box_append(GTK_BOX(box_for_selection), selection_label);
      gtk_box_append(GTK_BOX(box_for_selection), selection_entry);
      gtk_box_append(GTK_BOX(box_for_selection), outliers_only_checkbutton);

      gtk_box_append(GTK_BOX(box_for_this_plot), gl_area);
      gtk_box_append(GTK_BOX(box_for_this_plot), box_for_selection);
      gtk_box_append(GTK_BOX(box_for_this_plot), close_button);
      gtk_box_append(GTK_BOX(box_for_all_plots), box_for_this_plot);

      gtk_widget_set_vexpand(GTK_WIDGET(box_for_this_plot), TRUE);

      gtk_widget_set_visible(gl_area, TRUE);

   }
}

// static
void
graphics_info_t::remove_plot_from_rama_plots(GtkWidget *plot_box) {

   std::vector<widgeted_rama_plot_t>::const_iterator it;
   for (it=rama_plot_boxes.begin(); it!=rama_plot_boxes.end(); ++it) {
      GtkWidget *this_plot_box = it->box;
      if (this_plot_box == plot_box) {
         rama_plot_boxes.erase(it);
         break;
      }
   }
}

// static
void
graphics_info_t::rama_plot_boxes_handle_close_molecule(int imol) {

   // date this needs to be called by close_molecule/close_yourself().

   std::vector<widgeted_rama_plot_t>::const_iterator it;
   for (it=rama_plot_boxes.begin(); it!=rama_plot_boxes.end(); ++it) {
      auto &rama_plot_box = *it;
      if (rama_plot_box.imol == imol) {
         remove_plot_from_rama_plots(rama_plot_box.box);
      }
   }
}

// static
void
graphics_info_t::rama_plot_boxes_handle_molecule_update(int imol) {

   std::vector<widgeted_rama_plot_t>::iterator it;
   for (it=rama_plot_boxes.begin(); it!=rama_plot_boxes.end(); ++it) {
      auto &rama_plot_box = *it;
      if (rama_plot_box.imol == imol) {
         auto &m = graphics_info_t::molecules[imol];
         gl_rama_plot_t::draw_mode_t draw_mode = gl_rama_plot_t::draw_mode_t::DRAW_MODE;
         rama_plot_box.rama.setup_from(imol, m.atom_sel.mol, rama_plot_box.residue_selection, draw_mode);
      }
   }
}


// static
void
graphics_info_t::rama_plot_boxes_handle_molecule_update(GtkWidget *rama_box, const std::string &residue_selection) {

   std::cout << "Here in rama_plot_boxes_handle_molecule_update() " << rama_box << " " << residue_selection << std::endl;
   std::vector<widgeted_rama_plot_t>::iterator it;
   for (it=rama_plot_boxes.begin(); it!=rama_plot_boxes.end(); ++it) {
      auto &rama_plot_box = *it;
      rama_plot_box.residue_selection = residue_selection;
      std::cout << "in rama_plot_boxes_handle_molecule_update() alpha-1 " << std::endl;
      if (rama_plot_box.box == rama_box) {
         std::cout << "in rama_plot_boxes_handle_molecule_update() beta-1 calling setu_from() "
                   << rama_plot_box.imol << " " << rama_plot_box.residue_selection << std::endl;
         auto &m = graphics_info_t::molecules[rama_plot_box.imol];
         gl_rama_plot_t::draw_mode_t draw_mode = gl_rama_plot_t::draw_mode_t::DRAW_MODE;
         rama_plot_box.rama.setup_from(rama_plot_box.imol, m.atom_sel.mol, rama_plot_box.residue_selection, draw_mode);
      }
   }
}

// static
void
graphics_info_t::rama_plot_boxes_handle_molecule_update(GtkWidget *rama_box) {

   // called from accept_moving_atoms();

   std::vector<widgeted_rama_plot_t>::iterator it;
   for (it=rama_plot_boxes.begin(); it!=rama_plot_boxes.end(); ++it) {
      auto &rama_plot_box = *it;
      std::cout << "in rama_plot_boxes_handle_molecule_update() alpha-2 " << std::endl;
      if (rama_plot_box.box == rama_box) {
         std::cout << "in rama_plot_boxes_handle_molecule_update() beta-2 " << rama_plot_box.imol << " " << rama_plot_box.residue_selection
                   << std::endl;
         auto &m = graphics_info_t::molecules[rama_plot_box.imol];
         gl_rama_plot_t::draw_mode_t draw_mode = gl_rama_plot_t::draw_mode_t::DRAW_MODE;
         rama_plot_box.rama.setup_from(rama_plot_box.imol, m.atom_sel.mol, rama_plot_box.residue_selection, draw_mode);
      }
   }
}





//
//  static
void
graphics_info_t::draw_rama_plots() {

   // these are the "validation" rama plots (on the left), not the interactive plot.

   for (unsigned int i=0; i<rama_plot_boxes.size(); i++) {
      GtkGLArea *gl_area = GTK_GL_AREA(rama_plot_boxes[i].gtk_gl_area);
      if (GTK_IS_GL_AREA(gl_area)) {
         GdkGLContext *context = gtk_gl_area_get_context(gl_area); // needed?
         gtk_gl_area_make_current(GTK_GL_AREA (gl_area));

         GtkAllocation allocation;
         gtk_widget_get_allocation(GTK_WIDGET(gl_area), &allocation);
         int w = allocation.width;
         int h = allocation.height;
         bool clear_needed_flag = true; // because drawing to its own glarea OpenGL context
         rama_plot_boxes[i].rama.draw(&shader_for_rama_plot_axes_and_ticks,
                                      &shader_for_rama_plot_phi_phis_markers, // instanced
                                      &shader_for_hud_image_texture,
                                      w, h, w, h, clear_needed_flag);
         gtk_widget_queue_draw(GTK_WIDGET(gl_area));
      } else {
         std::cout << "ERROR:: ploting rama plot " << i << " which hash gl_area that has gone out of scope!"
                   << std::endl;
      }
   }
}
