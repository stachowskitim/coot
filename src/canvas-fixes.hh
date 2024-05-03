/*
 * src/canvas-fixes.hh
 *
 * Copyright 2009 by University of York
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

#ifdef HAVE_GNOME_CANVAS
  typedef GnomeCanvas GtkCanvas;
  typedef GnomeCanvasItem GtkCanvasItem;
  typedef GnomeCanvasPoints GtkCanvasPoints;
  #define GTK_CANVAS GNOME_CANVAS
  #define GTK_CANVAS_ITEM GNOME_CANVAS_ITEM
  #define GTK_CANVAS_TYPE_CANVAS_RECT GNOME_TYPE_CANVAS_RECT
  #define GTK_CANVAS_TYPE_CANVAS_LINE GNOME_TYPE_CANVAS_LINE
  #define GTK_CANVAS_TYPE_CANVAS_TEXT GNOME_TYPE_CANVAS_TEXT
  #define gtk_canvas_init gnome_canvas_init
  #define gtk_canvas_new  gnome_canvas_new
  #define gtk_canvas_root gnome_canvas_root
  #define gtk_canvas_item_new gnome_canvas_item_new
  #define gtk_canvas_points_new gnome_canvas_points_new
  #define gtk_canvas_points_free gnome_canvas_points_free
  #define gtk_canvas_item_w2i gnome_canvas_item_w2i
  #define gtk_canvas_item_grab gnome_canvas_item_grab
  #define gtk_canvas_item_lower_to_bottom gnome_canvas_item_lower_to_bottom
  #define gtk_canvas_item_lower gnome_canvas_item_lower
  #define gtk_canvas_set_scroll_region gnome_canvas_set_scroll_region
  #define gtk_canvas_item_raise_to_top gnome_canvas_item_raise_to_top
  #define gtk_canvas_item_raise gnome_canvas_item_raise
  #define gtk_canvas_item_move gnome_canvas_item_move
  #define gtk_canvas_item_ungrab gnome_canvas_item_ungrab
  #define gtk_canvas_rect_get_type gnome_canvas_rect_get_type
  #define gtk_canvas_set_pixels_per_unit gnome_canvas_set_pixels_per_unit
  #define gtk_canvas_window_to_world gnome_canvas_window_to_world
  #define gtk_canvas_item_set gnome_canvas_item_set
#endif
