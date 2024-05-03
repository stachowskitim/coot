/* src/generic-objects.cc
 * 
 * Copyright 2005, 2006 The University of York
 * Copyright 2007, 2012 The University of Oxford
 * Copyright 2014 by Medical Research Council
1 * 
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
 * write to the Free Software Foundation, Inc., 51 Franklin Street,  02110-1301, USA
 */


#ifdef USE_PYTHON
#include "Python.h"  // before system includes to stop "POSIX_C_SOURCE" redefined problems
#include "python-3-interface.hh"
#endif

#include <epoxy/gl.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "compat/coot-sysdep.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

#include <gtk/gtk.h>
#include <math.h>

#include "c-interface.h"
#include "cc-interface.hh"
#include "cc-interface-scripting.hh"
#include "old-generic-display-object.hh" // needed? - remove it later
#include "meshed-generic-display-object.hh"
#include "c-interface-widgets.hh" // for generic_objects_dialog_table_add_object_internal()

#include "graphics-info.h"
#include "c-interface-generic-objects.h"


/*  ----------------------------------------------------------------------- */
/*                  Generic Objects                                         */
/*  ----------------------------------------------------------------------- */
void to_generic_object_add_line(int object_number,
                                const char *colour_name,
                                int line_width,
                                float from_x1,
                                float from_y1,
                                float from_z1,
                                float to_x2,
                                float to_y2,
                                float to_z2) {

   clipper::Coord_orth x1(from_x1, from_y1, from_z1);
   clipper::Coord_orth x2(to_x2, to_y2, to_z2);

   graphics_info_t g;
   std::pair<clipper::Coord_orth, clipper::Coord_orth> coords(x1, x2);

   std::string c(colour_name);
   if (object_number >= 0) {
      gtk_gl_area_attach_buffers(GTK_GL_AREA(graphics_info_t::glareas[0]));
      unsigned int object_number_u(object_number);
      if (object_number_u < g.generic_display_objects.size()) {
         std::pair<glm::vec3, glm::vec3> p(coord_orth_to_glm(x1), coord_orth_to_glm(x2));
         meshed_generic_display_object &obj = g.generic_display_objects[object_number];
         float radius = 0.1 * static_cast<float>(line_width);
         unsigned int n_slices = 12;
         coot::colour_holder col = colour_values_from_colour_name(colour_name);
         obj.add_cylinder(p, col, radius, n_slices, true, true,
                          meshed_generic_display_object::FLAT_CAP,
                          meshed_generic_display_object::FLAT_CAP);

         Material material;
         obj.mesh.setup(material);
      } else {
         std::cout << "BAD object_number in to_generic_object_add_line"
                   << " out of range high" << object_number << std::endl;
      }
   } else {
      std::cout << "BAD object_number (out of range low) in to_generic_object_add_line"
                << object_number << std::endl;
   }
}

bool is_valid_generic_display_object_number(int obj) {
   if (obj < 0) return false;
   int s = graphics_info_t::generic_display_objects.size();
   if (obj >= s) return false;
   return true;
}


void to_generic_object_add_cylinder(int object_number,
                                    const char *colour,
                                    float line_radius,
                                    int n_slices, // 4, 8, 16
                                    float from_x,
                                    float from_y,
                                    float from_z,
                                    float to_x,
                                    float to_y,
                                    float to_z,
                                    bool cap_start,
                                    bool cap_end) {

   glm::vec3 x1(from_x, from_y, from_z);
   glm::vec3 x2(to_x, to_y, to_z);
   std::pair<glm::vec3, glm::vec3> p(x1, x2);
   std::string colour_name(colour);
   coot::colour_holder col = colour_values_from_colour_name(colour_name);
   graphics_info_t g;
   if (is_valid_generic_display_object_number(object_number)) {
      meshed_generic_display_object &obj = g.generic_display_objects[object_number];
      obj.add_cylinder(p, col, line_radius, n_slices, cap_start, cap_end,
                       meshed_generic_display_object::FLAT_CAP,
                       meshed_generic_display_object::FLAT_CAP);
   }
}


/*! \brief add line to generic object object_number */
void to_generic_object_add_dashed_line(int object_number,
                                       const char *colour,
                                       int line_width,
                                       float dash_density,
                                       float from_x1,
                                       float from_y1,
                                       float from_z1,
                                       float to_x2,
                                       float to_y2,
                                       float to_z2) {

   clipper::Coord_orth p_start(from_x1, from_y1, from_z1);
   clipper::Coord_orth p_end(to_x2, to_y2, to_z2);
   float ll = clipper::Coord_orth::length(p_start, p_end);
   int n_dashes = int(dash_density * ll);
   bool visible = 1;

   for (int idash=0; idash<(n_dashes-1); idash++) {
      if (visible) {
         float fracs = float(idash)/float(n_dashes);
         float fracn = float(idash+1)/float(n_dashes);
         clipper::Coord_orth p1 = p_start + fracs * (p_end - p_start);
         clipper::Coord_orth p2 = p_start + fracn * (p_end - p_start);
         to_generic_object_add_line(object_number, colour, line_width,
                                    p1.x(), p1.y(), p1.z(),
                                    p2.x(), p2.y(), p2.z());
      }
      visible = !visible;
   }
}





void to_generic_object_add_point(int object_number,
                                 const char *colour_name,
                                 int point_width,
                                 float from_x1,
                                 float from_y1,
                                 float from_z1) {

   graphics_info_t g;
   clipper::Coord_orth pt(from_x1, from_y1, from_z1);
   std::string c(colour_name);
   coot::colour_holder colour = coot::old_generic_display_object_t::colour_values_from_colour_name(c);

   to_generic_object_add_point_internal(object_number, colour_name, colour, point_width, pt);

}

void to_generic_object_add_point_internal(int object_number,
                                          const std::string &colour_name,
                                          const coot::colour_holder &colour,
                                          int point_width,
                                          const clipper::Coord_orth &pt) {
   graphics_info_t g;

   if (object_number >=0 && object_number < int(g.generic_display_objects.size())) {

      if (false)
         std::cout << "......... to_generic_object_add_point_internal() " << object_number
                   << " " << colour << " " << pt.format() << std::endl;

      gtk_gl_area_attach_buffers(GTK_GL_AREA(graphics_info_t::glareas[0]));
      unsigned int object_number_u(object_number);
      if (object_number_u < g.generic_display_objects.size()) {
         meshed_generic_display_object &obj = g.generic_display_objects[object_number];
         Material material;
         unsigned int num_subdivisions = 2;
         g.generic_display_objects[object_number].add_point(colour, colour_name, point_width, pt, num_subdivisions);
         // obj.mesh.setup(&g.shader_for_moleculestotriangles, material); // fast return if already done
         obj.mesh.setup(material); // fast return if already done
      }
   } else {
      std::cout << "BAD object_number in to_generic_object_add_point: "
                << object_number << std::endl;
   }
}


void from_generic_object_remove_last_item(int object_number) {

   graphics_info_t::from_generic_object_remove_last_item(object_number);
}


void to_generic_object_add_dodecahedron(int object_number,
                                        const char *colour_name,
                                        float radius,
                                        float x,
                                        float y,
                                        float z) {

   graphics_info_t g;
   clipper::Coord_orth x1(x, y, z);
   std::string c(colour_name);
   coot::colour_holder colour =
      coot::old_generic_display_object_t::colour_values_from_colour_name(c);

   if (object_number >=0 && object_number < int(g.generic_display_objects.size())) {

      g.generic_display_objects.at(object_number).add_dodecahedron(colour, c, radius, x1);

   } else {
      std::cout << "BAD object_number in to_generic_object_add_point: "
                << object_number << std::endl;
   }
}

void to_generic_object_add_pentakis_dodecahedron(int object_number,
                                                 const char *colour_name,
                                                 float stellation_factor,
                                                 float radius,
                                                 float x,
                                                 float y,
                                                 float z) {

   graphics_info_t g;
   clipper::Coord_orth x1(x, y, z);
   std::string c(colour_name);
   coot::colour_holder colour =
      coot::old_generic_display_object_t::colour_values_from_colour_name(c);

   if (object_number >=0 && object_number < int(g.generic_display_objects.size())) {

      g.generic_display_objects[object_number].add_pentakis_dodecahedron(colour, c, stellation_factor, radius, x1);
   } else {
      std::cout << "BAD object_number in to_generic_object_add_point: "
                << object_number << std::endl;
   }
}

void to_generic_object_add_torus_internal(int object_number,
                                          const std::string &colour_name,
                                          const clipper::Coord_orth &position,
                                          const clipper::Coord_orth &normal,
                                          float radius_1,
                                          float radius_2) {

   graphics_info_t g;
   if (is_valid_generic_display_object_number(object_number)) {
      meshed_generic_display_object::torus_t torus(position, normal, radius_1, radius_2);
      coot::colour_holder colour =
         coot::old_generic_display_object_t::colour_values_from_colour_name(colour_name);
      torus.col = colour;
      g.generic_display_objects[object_number].add_torus(torus);
      meshed_generic_display_object &obj = g.generic_display_objects[object_number];
      Material material;
      // obj.mesh.setup(&g.shader_for_moleculestotriangles, material); //20210910-PE 
      obj.mesh.setup(material);
   }
}

void to_generic_object_add_torus(int object_number,
                               const char *colour_name,
                               float radius,
                               float radius_inner,
                               float centre_point_x,
                               float centre_point_y,
                               float centre_point_z,
                               float normal_x,
                               float normal_y,
                               float normal_z) {

   clipper::Coord_orth position(centre_point_x, centre_point_y, centre_point_z);
   clipper::Coord_orth normal(normal_x, normal_y, normal_z);
   to_generic_object_add_torus_internal(object_number, std::string(colour_name),
                                        position, normal, radius, radius_inner);

}



/*! \brief add point to generic object object_number */
void to_generic_object_add_arc(int object_number,
                               const char *colour_name,
                               float radius,
                               float radius_inner,
                               float angle_delta,
                               float start_point_x,
                               float start_point_y,
                               float start_point_z,
                               float start_dir_x,
                               float start_dir_y,
                               float start_dir_z,
                               float normal_x,
                               float normal_y,
                               float normal_z) {
   graphics_info_t g;
   if (object_number >=0 && object_number < int(g.generic_display_objects.size())) {
      meshed_generic_display_object::arc_t arc(angle_delta,
                                               clipper::Coord_orth(start_point_x,
                                                                   start_point_y,
                                                                   start_point_z),
                                               clipper::Coord_orth(start_dir_x,
                                                                   start_dir_y,
                                                                   start_dir_z),
                                               clipper::Coord_orth(normal_x,
                                                                   normal_y,
                                                                   normal_z),
                                               radius, radius_inner);
      coot::colour_holder colour =
         coot::old_generic_display_object_t::colour_values_from_colour_name(std::string(colour_name));
      // bleugh!  Make your colour holders consistent!  - use the utils version throughout!
      arc.col.red   = colour.red;
      arc.col.green = colour.green;
      arc.col.blue  = colour.blue;

      g.generic_display_objects[object_number].add_arc(arc);

   } else {
      std::cout << "BAD object_number in to_generic_object_add_arc: "
                << object_number << std::endl;
   }
}

void
to_generic_object_add_arrow(int object_number,
                            const char *colour_name,
                            float stem_radius,
                            float from_x1,
                            float from_y1,
                            float from_z1,
                            float to_x2,
                            float to_y2,
                            float to_z2) {

   // pass this?
   float arrow_head_length = 4 * stem_radius;

   float radius = stem_radius;

   clipper::Coord_orth x1(from_x1, from_y1, from_z1);
   clipper::Coord_orth x2(to_x2, to_y2, to_z2);

   graphics_info_t g;
   std::pair<clipper::Coord_orth, clipper::Coord_orth> coords(x1, x2);

   std::string c(colour_name);
   coot::colour_holder colour = colour_values_from_colour_name(c);
   if (object_number >= 0) {
      gtk_gl_area_attach_buffers(GTK_GL_AREA(graphics_info_t::glareas[0]));
      unsigned int object_number_u(object_number);
      if (object_number_u < g.generic_display_objects.size()) {
         std::pair<glm::vec3, glm::vec3> p(coord_orth_to_glm(x1), coord_orth_to_glm(x2));
         const glm::vec3 &pt_start_g = p.first;
         const glm::vec3 &pt_end_g   = p.second;
         meshed_generic_display_object &obj = g.generic_display_objects[object_number];
         unsigned int n_slices = 24;
         coot::colour_holder col = colour_values_from_colour_name(colour_name);

         // we have to think about the positioning of the head and the stick
         glm::vec3 b = p.second - p.first;
         float bl_input = glm::distance(b, glm::vec3(0,0,0));
         float bl = bl_input - arrow_head_length;
         if (arrow_head_length > bl_input)
            arrow_head_length = bl_input;
         glm::vec3 b_uv = glm::normalize(b);
         float bl_stick = bl;
         if (bl_stick < 0.0) bl_stick = 0.0;

         glm::vec3 meeting_point = pt_start_g + b_uv * bl_stick;
         std::pair<glm::vec3, glm::vec3> pp_stick(pt_start_g, meeting_point);
         std::pair<glm::vec3, glm::vec3> pp_cone(pt_end_g, meeting_point);

         obj.add_cylinder(pp_stick, col, radius, n_slices, true, false,
                          meshed_generic_display_object::FLAT_CAP,
                          meshed_generic_display_object::FLAT_CAP);

         float base_radius = radius * 2.0f;
         float top_radius = 0.0f;
         obj.add_cone(pp_cone, col, base_radius, top_radius, n_slices, false, true,
                      meshed_generic_display_object::FLAT_CAP,
                      meshed_generic_display_object::FLAT_CAP);

         Material material;
         // obj.mesh.setup(&g.shader_for_moleculestotriangles, material);
         obj.mesh.setup(material);
      } else {
         std::cout << "BAD object_number in to_generic_object_add_line"
                   << " out of range high" << object_number << std::endl;
      }
   } else {
      std::cout << "BAD object_number (out of range low) in to_generic_object_add_line"
                << object_number << std::endl;
   }
}



void to_generic_object_add_display_list_handle(int object_number, int display_list_id) {

   // we can't do this now
#if 0
   graphics_info_t g;
   if (object_number >=0 && object_number < int(g.generic_display_objects.size())) {
      g.generic_display_objects[object_number].GL_display_list_handles.push_back(display_list_id);
   } else {
      std::cout << "BAD object_number in to_generic_object_add_point: "
                << object_number << std::endl;
   }
#endif
}


void set_display_generic_object_simple(int object_number, short int istate) {

   graphics_info_t g;
   if (object_number >=0  && object_number < int(g.generic_display_objects.size())) {
      g.generic_display_objects[object_number].mesh.set_draw_this_mesh(istate);
   } else {
      std::cout << "ERROR:: BAD object_number in to_generic_object_add_point: "
                << object_number << std::endl;
   }

#if 0 // 20230521-PE a modern version of this would useful. FIXME.

   if (g.generic_objects_dialog) {

      // get the togglebutton and set its state
      std::string toggle_button_name = "generic_object_" + coot::util::int_to_string(object_number) + "_toggle_button";

      GtkWidget *toggle_button = lookup_widget(g.generic_objects_dialog, toggle_button_name.c_str()); // #ifdef 0
      // GtkWidget *toggle_button = 0;

      if (toggle_button) {
         if (istate)
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle_button), TRUE);
         else
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle_button), FALSE);
      }
   }
#endif
}

void set_display_generic_object(int object_number, short int istate) {
   set_display_generic_object_simple(object_number, istate);
   graphics_draw();
}

#include "widget-from-builder.hh"

/*! \brief display (1) or undisplay (0) all generic objects */
void set_display_all_generic_objects(int state) {

   graphics_info_t g;
   GtkWidget *grid = widget_from_builder("generic_objects_dialog_grid");
   if (g.generic_objects_dialog) {
      unsigned int n_rows = 104;
      for (unsigned int i_row=0; i_row<n_rows; i_row++) {
         GtkWidget *checkbutton = gtk_grid_get_child_at(GTK_GRID(grid), 1, i_row);
         if (! checkbutton) break;
         // changing the state of the check button calls the callback
         // which changes the draw state of the mesh. I don't need to do
         // that explicitly in this function.
         if (state) {
            gtk_check_button_set_active(GTK_CHECK_BUTTON(checkbutton), TRUE);
         } else {
            gtk_check_button_set_active(GTK_CHECK_BUTTON(checkbutton), FALSE);
         }
      }
   }
   graphics_draw();
}


/*! \brief is generic display object displayed?

  @return 1 for yes, otherwise 0  */
int generic_object_is_displayed_p(int object_number) {

   int is_displayed = 0;
   graphics_info_t g;
   if (object_number >=0  && object_number < int(g.generic_display_objects.size())) {
      is_displayed = g.generic_display_objects[object_number].mesh.get_draw_this_mesh();
   }
   return is_displayed;
}


int new_generic_object_number(const std::string &name_string) {

   std::cout << "--------------- new_generic_object_number() " << name_string << std::endl;

   graphics_info_t g;
   int n_new = g.new_generic_object_number(name_string);
   return n_new;
}

// create a new generic display object that is attached to imol
//
int new_generic_object_number_for_molecule(const std::string &name, int imol) {

   graphics_info_t g;
   return g.new_generic_object_number_for_molecule(name, imol);
}


// return the index of the object with name name, if not, return -1;
//
int generic_object_index(const std::string &name) {

   return graphics_info_t::generic_object_index(name);
}


// OLD code passing back a const char (yeuch)
//
// /*! \brief what is the name of generic object number obj_number?

// return 0 (NULL) #f  on obj_number not available */
// const char *generic_object_name(int obj_number) {

//    std::string s;
//    const char *r = 0;
//    graphics_info_t g;
//    int n_objs = g.generic_objects_p->size();
//    // funny way! (no good reason)
//    for (unsigned int i=0; i<n_objs; i++) {
//       if (i == obj_number)
// 	 r = (*g.generic_objects_p)[i].name.c_str();
//    }
//    return r;
// }



/*! \brief what is the name of generic object number obj_number? 
  return #f on obj_number not available */
#ifdef USE_GUILE
SCM generic_object_name_scm(int obj_number) {
   graphics_info_t g;
   int n_objs = g.generic_display_objects.size();
   SCM r = SCM_BOOL_F;
   for (int i=(n_objs-1); i>=0; i--) {
      if (i == obj_number) {
	 if (!g.generic_display_objects[i].mesh.this_mesh_is_closed) { 
	    r = scm_from_locale_string(g.generic_display_objects[i].mesh.name.c_str());
	 }
      }
   }
   return r;
}
#endif /* USE_GUILE */

#ifdef USE_PYTHON
PyObject *generic_object_name_py(unsigned int obj_number_in) {
   graphics_info_t g;
   int obj_number = obj_number_in;
   int n_objs = g.generic_display_objects.size();
   PyObject *r = Py_False;
   for (int i=(n_objs-1); i>=0; i--) {
      if (i == obj_number) {
	 if (!g.generic_display_objects[i].mesh.this_mesh_is_closed) { 
	    r = myPyString_FromString(g.generic_display_objects[i].mesh.name.c_str());
	    break;
	 }
      }
   }
   if (PyBool_Check(r)) {
     Py_INCREF(r);
   }
   return r;
} 
#endif /* USE_PYTHON */


/*! \brief clear out the lines and points from object_number, but keep
  it displayable (not closed). */
void generic_object_clear(int object_number) {

   graphics_info_t g;
   if (object_number >= 0) {
      if (object_number < int(g.generic_display_objects.size())) {
	 g.generic_display_objects[object_number].clear();
      }
   }
}



/*! \brief close generic object, clear the lines/points etc, not
  available for buttons/displaying etc
*/
void close_generic_object(int object_number) {

   graphics_info_t g;
   if (object_number >=0) {
      if (object_number < int(g.generic_display_objects.size())) {
	 g.generic_display_objects[object_number].close_yourself();
      }
   }

   if (g.generic_objects_dialog) {
      // get the togglebutton and set its state
      std::string stub = "generic_object_" + coot::util::int_to_string(object_number);
      std::string toggle_button_name = stub + "_toggle_button";
      std::string label_name = stub + "_label";
      // GtkWidget *toggle_button = lookup_widget(g.generic_objects_dialog, toggle_button_name.c_str());
      // GtkWidget *label = lookup_widget(g.generic_objects_dialog, label_name.c_str());
      std::cout << "WARNING:: in close_generic_object() set the toggle_button and label correctly " << std::endl;
      GtkWidget *toggle_button = 0;
      GtkWidget *label = 0;
      if (toggle_button)
	 gtk_widget_set_visible(toggle_button, FALSE);
      if (label)
	 gtk_widget_set_visible(label, FALSE);
   }
}

/*! \brief has the generic object been closed? 

   @return 1 for yes, 0 othersize
*/
short int is_closed_generic_object_p(int object_number) {

   short int state = 0;
   graphics_info_t g;
   if (object_number >=0) {
      if (object_number < int(g.generic_display_objects.size())) {
	 state = g.generic_display_objects[object_number].mesh.this_mesh_is_closed;
      }
   }
   return state;
}


void close_all_generic_objects() {

   std::cout << ":::::::::::::: debug:: close_all_generic_objects() " << std::endl;

   graphics_info_t g;
   unsigned int n_for_row_delete = 0;

   // ----------------- graphics_info meshes --------------------

   int n_objs = g.generic_display_objects.size();
   n_for_row_delete += n_objs;
   for (int i=0; i<n_objs; i++) {
      meshed_generic_display_object &obj = g.generic_display_objects[i];
      std::cout << ":::::::::::::: debug:: close_all_generic_objects() closing internal generic object "
                << obj.mesh.name << std::endl;
      if (! obj.mesh.this_mesh_is_closed) // Hmm.
	 obj.close_yourself();
   }

   // ----------------- molecule_class_info meshes --------------------

   for (unsigned int i=0; i<g.molecules.size(); i++) {
      molecule_class_info_t &m = g.molecules[i];
      for (unsigned int j=0; j<m.instanced_meshes.size(); j++) {
         Instanced_Markup_Mesh &imm = m.instanced_meshes[j];
         std::cout << ":::::::::::::: debug:: close_all_generic_objects() closing molecule generic object "
                   << imm.get_name() << std::endl;
         imm.close();
         n_for_row_delete++;
      }
   }

   // now clear the grid widgets

   GtkWidget *grid = widget_from_builder("generic_objects_dialog_grid");
   for (unsigned int i=0; i<n_for_row_delete; i++)
      gtk_grid_remove_row(GTK_GRID(grid), 0);

   graphics_draw();
}

#include "c-interface-gui.hh" // for set_transient_for_main_window()

void generic_objects_gui_wrapper() {

   graphics_info_t g;
   g.generic_objects_dialog = wrapped_create_generic_objects_dialog();
   set_transient_for_main_window(g.generic_objects_dialog);
   gtk_widget_set_visible(g.generic_objects_dialog, TRUE);
}



/*! \brief attach the generic object to a particular molecule

one might do this if the generic object is specific to a molecule.
 */
void attach_generic_object_to_molecule(int object_number, int imol) {

   graphics_info_t g;
   if (object_number >=0) {
      if (object_number < int(g.generic_display_objects.size())) {
         if (is_valid_model_molecule(imol)) {
            g.generic_display_objects[object_number].attach_to_molecule(imol);
         }
      }
   }
}



/*! \brief print to the console the name and display status of the
  generic display objects */
void generic_object_info() {

   graphics_info_t g;
   unsigned int n_obs = g.generic_display_objects.size();
   std::cout << "There are " << n_obs << " generic objects\n";
   if (n_obs > 0) {
      for (unsigned int i=0; i<n_obs; i++) {
         std::string display_str(":Displayed:");
         if (! g.generic_display_objects[i].mesh.get_draw_this_mesh())
            display_str = ":Not Displayed:";
         std::string closed_str(":Closed:");
         if (! g.generic_display_objects[i].mesh.this_mesh_is_closed) // Hmm.
            closed_str = ":Not Closed:";
         std::cout << " # " << i << " \"" << g.generic_display_objects[i].mesh.name << "\" "
                   << display_str << " " << closed_str << std::endl;
      }
   } else {
      std::cout << "No Generic Display Objects" << std::endl;
   }
}

#ifdef USE_PYTHON
/*! \brief get generic display objects */
PyObject *get_generic_object_info(int obj_number) {

   PyObject *o = Py_None;
   graphics_info_t g;
   int n_obs = g.generic_display_objects.size();
   if (obj_number < n_obs) {
      if (obj_number >= 0) {

         const auto &info = g.generic_display_objects[obj_number].info;
         o = PyList_New(info.size());
         for (unsigned int i=0; i<info.size(); i++) {

            PyObject *d = PyDict_New();
            PyObject *colour_py = PyList_New(3);
            const auto &col = info[i].colour;
            PyList_SetItem(colour_py, 0, PyFloat_FromDouble(static_cast<double>(col.red)));
            PyList_SetItem(colour_py, 1, PyFloat_FromDouble(static_cast<double>(col.green)));
            PyList_SetItem(colour_py, 2, PyFloat_FromDouble(static_cast<double>(col.blue)));
            PyDict_SetItemString(d, "colour", colour_py);
            const auto &pos = info[i].position;
            PyObject *position_py = PyList_New(3);
            PyList_SetItem(position_py, 0, PyFloat_FromDouble(pos.x()));
            PyList_SetItem(position_py, 1, PyFloat_FromDouble(pos.y()));
            PyList_SetItem(position_py, 2, PyFloat_FromDouble(pos.z()));
            PyDict_SetItemString(d, "position", position_py);

            PyList_SetItem(o, i, d);
         }
      }
   }
   return o;

}
#endif /* USE_PYTHON */


// generic object obj_no has things to display?
// Return 0 or 1.
//
short int generic_object_has_objects_p(int object_number) {

   short int r = 0;
   graphics_info_t g;
   if ((object_number >=0) && (object_number < int(g.generic_display_objects.size()))) {
      if (true) // some test here?
         r = 1;
   } else {
      std::cout << "WARNING:: object_number in generic_display_objects "
                << object_number << std::endl;
   }
   return r;
}



/*! \brief pass a filename that contains molprobity's probe output in XtalView
format */
void handle_read_draw_probe_dots(const char *dots_file) {

   // std::cout << "handle reading dots file" << dots_file << std::endl;
   if (dots_file) {

      FILE* dots = fopen(dots_file, "r" );
      if ( dots == NULL ) {
         std::cout << "handle_read_draw_probe_dots  - Could not read: "
                   << dots_file << std::endl;
         // fclose(dots);
      } else {

         // clear up what probe contacts/overlaps we already have:
         std::vector<std::string> deletable_names;
         deletable_names.push_back("wide contact");
         deletable_names.push_back("close contact");
         deletable_names.push_back("small overlap");
         deletable_names.push_back("bad overlap");
         deletable_names.push_back("H-bonds");
         unsigned int nobjs = graphics_info_t::generic_display_objects.size();
         for (unsigned int i=0; i< nobjs; i++) {
            for (unsigned int d=0; d<deletable_names.size(); d++) {
               if (graphics_info_t::generic_display_objects[i].mesh.name == deletable_names[d]) {
                  // close_generic_object(i); // empty it, really
                  graphics_info_t::generic_display_objects.clear();
               }
            }
         }
         int n_lines = 0;
         int n_points = 0;
         std::string current_colour = "blue"; // should be reset.
         std::string current_name   = "Unassigned";
         int obj_no = number_of_generic_objects();
         char line[240];
         char s[240];
         char s1[240];
         char s2[240];
         char s3[240];
         float x1, x2, x3, x4, x5, x6;
         while ( fgets( line, 240, dots ) != NULL ) {
            if (sscanf(line, "# %s %s %s", s1, s2, s3)) {
               std::string st1, st2, st3;
               st1 = s1;
               st2 = s2;
               st3 = s3;
               // a comment line, what type of dots are we looking at?
               // std::cout << "# line --- " << line << std::endl;
               clipper::String contact_line(s);
               std::vector<std::string> vs;
               vs.push_back(st1);
               vs.push_back(st2);
               vs.push_back(st3);
               int n_bits = 3;

               std::pair<short int, std::string> p = is_interesting_dots_object_next_p(vs);
               if (p.first) {
                  if (p.second != current_name) {
                     int maybe_old_object = generic_object_index(p.second.c_str());
                     if (maybe_old_object > -1) {
                        obj_no = maybe_old_object;
                     } else {
                        obj_no = new_generic_object_number(p.second.c_str());
                     }
                     // non-member function usage, so that we don't do the redraw.
                     graphics_info_t::generic_display_objects[obj_no].mesh.set_draw_this_mesh(true);
                     graphics_info_t::generic_display_objects[obj_no].mesh.this_mesh_is_closed = false;
                     current_name = p.second;
                  }
               }

            } else {
               if (sscanf(line, "%f %f %f %f %f %f %s", &x1, &x2, &x3, &x4, &x5, &x6, s)) {
                  current_colour = s;
                  float length2 = pow((x1-x4),2) + pow((x2-x5),2) + pow((x3-x6),2);
                  if (length2 > 0.1) {
                     n_lines++;
                     to_generic_object_add_line(obj_no, current_colour.c_str(), 3,
                                                x1, x2, x3, x4, x5, x6);
                  } else {
                     n_points++;
                     to_generic_object_add_point(obj_no, current_colour.c_str(), 2,
                                                 x1, x2, x3);
                  }
               } else {
                  if (strlen(line) > 0)
                     std::cout << ":" << line << ": failed to scan" << std::endl;
               }
            }
         }
         // std::cout << "INFO:: added " << n_lines << " lines and " << n_points << " points\n";
      }
      fclose(dots);
   }
}


void handle_read_draw_probe_dots_unformatted(const char *dots_file, int imol,
                                             int show_clash_gui_flag) {

   std::cout << "in handle_read_draw_probe_dots_unformatted() " << std::endl;

   int dot_size_scale_factor = 1; // user param?

   std::vector<coot::atom_spec_t> clash_atoms;
   bool hybrid_36_enabled_probe_flag = 1; // new style
   //bool hybrid_36_enabled_probe_flag = 0; // old style

   graphics_info_t g; // we use meshed_generic_display_objects now

   if (dots_file) {

      FILE* dots = fopen(dots_file, "r" );
      if ( dots == NULL ) {
         std::cout << "handle_read_draw_probe_dots  - Could not read: "
                   << dots_file << std::endl;
         // fclose(dots);
      } else {

         // clear up what probe contacts/overlaps we already have:
         std::vector<std::string> deletable_names;
         deletable_names.push_back("wide contact");
         deletable_names.push_back("close contact");
         deletable_names.push_back("small overlap");
         deletable_names.push_back("bad overlap");
         deletable_names.push_back("H-bonds");
         int nobjs = graphics_info_t::generic_display_objects.size();
         for (int i=0; i< nobjs; i++) {
            for (unsigned int d=0; d<deletable_names.size(); d++) {
               if (graphics_info_t::generic_display_objects[i].mesh.name == deletable_names[d]) {
                  // close_generic_object(i); // empty it, really
                  graphics_info_t::generic_display_objects[i].clear();
               }
            }
         }

         int n_input_lines = 0;
         int n_lines = 0;
         int n_points = 0;
         std::string current_colour = "blue"; // should be reset.
         int obj_no = number_of_generic_objects();

         int imol_source, imol_target;
         float gap1, factor3, gap2, factor4, factor5, factor6;
         char line[240];
         char c_type[20];
         char atom_id1[30];
         char atom_id2[30];
         char contact_type1[200];
         char contact_type2[3];
         float x1, x2, x3, x4, x5, x6;
         std::string current_contact_type = "none";
         int dot_size = 2; // 3 for hydrogen bonds

         // null string arrays
         for (int i=0; i<20; i++) c_type[i] = 0;
         for (int i=0; i<30; i++) atom_id1[i] = 0;
         for (int i=0; i<30; i++) atom_id2[i] = 0;
         std::string current_useful_name;

         std::string scan_line = ":%d->%d:%2c:%15c:%15c:%f:%f:%f:%f:%f:%f:%f:%s";
         if (hybrid_36_enabled_probe_flag)
            scan_line = ":%d->%d:%2c:%16c:%16c:%f:%f:%f:%f:%f:%f:%f:%s";

         std::set<int> mgdos_needing_a_setup;

         std::map<std::string, coot::colour_holder> colour_map;
         colour_map["blue"      ] = ::colour_values_from_colour_name("blue");
         colour_map["sky"       ] = ::colour_values_from_colour_name("sky");
         colour_map["sea"       ] = ::colour_values_from_colour_name("sea");
         colour_map["greentint" ] = ::colour_values_from_colour_name("greentint");
         colour_map["darkpurple"] = ::colour_values_from_colour_name("darkpurple");
         colour_map["green"     ] = ::colour_values_from_colour_name("green");
         colour_map["orange"    ] = ::colour_values_from_colour_name("orange");
         colour_map["orangered" ] = ::colour_values_from_colour_name("orangered");
         colour_map["yellow"    ] = ::colour_values_from_colour_name("yellow");
         colour_map["yellowtint"] = ::colour_values_from_colour_name("yellowtint");
         colour_map["red"       ] = ::colour_values_from_colour_name("red");
         colour_map["#55dd55"   ] = ::colour_values_from_colour_name("#55dd55");
         colour_map["hotpink"   ] = ::colour_values_from_colour_name("hotpink");
         colour_map["grey"      ] = ::colour_values_from_colour_name("grey");
         colour_map["magenta"   ] = ::colour_values_from_colour_name("magenta");
         colour_map["royalblue" ] = ::colour_values_from_colour_name("royalblue");

         while ( fgets( line, 240, dots ) != NULL ) {
            n_input_lines++;
            for (int i=0; i<3; i++) contact_type1[i] = 0;
            for (int i=0; i<3; i++) contact_type2[i] = 0;


            if (sscanf(line,
                       scan_line.c_str(),
                       &imol_source, &imol_target, c_type, atom_id1, atom_id2,
                       &gap1, &gap2, &x1, &x2, &x3, &factor3, &factor4,
                       contact_type1)) {

               if (strlen(contact_type1) > 2) {
                  int colon_count = 0;
                  int offset = 0;

                  // std::cout << "colon search |" << contact_type1 << std::endl;
                  for (int i=0; i<10; i++) {
                     if (contact_type1[i] == ':')
                        colon_count++;
                     if (colon_count == 2) {
                        offset = i + 1;
                        break;
                     }
                  }

                  if (offset > 0) {
                     // std::cout << "scanning |" << contact_type1+offset << std::endl;

                     if (sscanf((contact_type1+offset), "%f:%f:%f:%f:%f",
                                &x4, &x5, &x6, &factor5, &factor6)) {

                        std::string atom_id_1_str(atom_id1);
                        std::string atom_id_2_str(atom_id2);
                        std::string contact_type(c_type);

                        //                          std::cout << "\"" << contact_type << "\"..\"" << atom_id_1_str
//                                   << "\"..to..\""
//                                    << atom_id_2_str << "\" " << gap1 << " " << gap2
//                                   << " (" << x1 << "," << x2 << "," << x3 << ")"
//                                    << " (" << x4 << "," << x5 << "," << x6 << ")"
//                                    << "\n";
//                          std::cout << "\"" << contact_type << "\"..\"" << atom_id_1_str
//                                    << "\"..to..\""
//                                    << atom_id_2_str << "\" " << gap1 << " " << gap2
//                                    << " (" << x1 << "," << x2 << "," << x3 << ")"
//                                    << " (" << x4 << "," << x5 << "," << x6 << ")"
//                                    << "\n";

                        // assign the colour and dot size
                        if (contact_type == "hb") {
                           current_colour = "greentint";
                           dot_size = 3;
                        } else {
                           dot_size = 2;
                           current_colour = "hotpink";
                           if (gap2> -0.4) current_colour = "red";
                           if (gap2> -0.3) current_colour = "orange";
                           if (gap2> -0.2) current_colour = "yellow";
                           if (gap2> -0.1) current_colour = "yellowtint";
                           if (gap2>  0.0) current_colour = "green";
                           if (gap2> 0.15) current_colour = "sea";
                           if (gap2> 0.25) current_colour = "sky";
                           if (gap2> 0.35) current_colour = "blue";
                        }

                        // do we need a new object?
                        if (contact_type != current_contact_type) {

                           // indexing into
                           // static std::vector<meshed_generic_display_object> generic_display_objects;

                           current_useful_name = probe_dots_short_contact_name_to_expanded_name(contact_type);
                           // do we have an object of that name already?
                           int maybe_old_object = generic_object_index(current_useful_name.c_str());
                           current_contact_type = contact_type;

                           if (maybe_old_object > -1) {
                              obj_no = maybe_old_object;
                           } else {
                              obj_no = new_generic_object_number(current_useful_name.c_str());
                              // std::cout << "changing type to " << contact_type << std::endl;
                           }
                           // non-member function usage, so that we don't do the redraw.
                           graphics_info_t::generic_display_objects[obj_no].mesh.set_draw_this_mesh(true);
                           graphics_info_t::generic_display_objects[obj_no].mesh.this_mesh_is_closed = false;
                        }

                        float length2 = pow((x1-x4),2) + pow((x2-x5),2) + pow((x3-x6),2);

#if 0 // ------------------------ old method for reference ---------------------------------------------------------------
                        if (length2 > 0.04) {

                           // hydrogen bonds are not drawn as spikes,
                           // they should be drawn as pillow surfaces.
                           if (contact_type == "hb") {
                              to_generic_object_add_point(obj_no, current_colour.c_str(),
                                                          dot_size * dot_size_scale_factor,
                                                          x1, x2, x3);
                              to_generic_object_add_point(obj_no, current_colour.c_str(),
                                                          dot_size * dot_size_scale_factor,
                                                          x4, x5, x6);
                           } else {
                              n_lines++;
                              to_generic_object_add_line(obj_no, current_colour.c_str(), 3 * dot_size_scale_factor,
                                                         x1, x2, x3, x4, x5, x6);
                           }
                        } else {
                           n_points++;
                           to_generic_object_add_point(obj_no, current_colour.c_str(), dot_size * dot_size_scale_factor,
                                                       x1, x2, x3);
                        }
#endif // ------------------------ old method for reference ---------------------------------------------------------------

// ------------------------ new method ---------------------------------------------------------------
                        const unsigned int num_subdivisions = 2;
                        const int size = 2;
                        meshed_generic_display_object &mgdo = g.generic_display_objects[obj_no];
                        mgdos_needing_a_setup.insert(obj_no); // setup called after the for loop
                        if (length2 > 0.04) {
                           coot::colour_holder col = colour_map[current_colour];
                           if (contact_type == "hb") {
                              clipper::Coord_orth pt_1(x1, x2, x3);
                              clipper::Coord_orth pt_2(x4, x5, x6);
                              mgdo.add_point(col, current_colour, size, pt_1, num_subdivisions);
                              mgdo.add_point(col, current_colour, size, pt_2, num_subdivisions);
                           } else {
                              auto start_end = std::make_pair(glm::vec3(x1, x2, x3), glm::vec3(x4,x5,x6));
                              mgdo.add_cylinder(start_end, col, 0.05 * dot_size_scale_factor, 12, true, true,
                                                meshed_generic_display_object::ROUNDED_CAP,
                                                meshed_generic_display_object::ROUNDED_CAP, false, 1.0);
                              n_lines++;
                           }
                        } else {
                           n_points++; // not useful
                           clipper::Coord_orth pt_1(x1, x2, x3);
                           coot::colour_holder col = colour_map[current_colour];
                           mgdo.add_point(col, current_colour, size, pt_1, num_subdivisions);
                        }
// ------------------------ end new method ---------------------------------------------------------------

                        if (length2 > 5) {
                           // a really long
                           std::cout << "DEBUG:: long line at line number "
                                     << n_input_lines
                                     << "  (" << x1 << "," << x2 << "," << x3 << ")"
                                     << "  (" << x4 << "," << x5 << "," << x6 << ")"
                                     << std::endl;
                           std::cout << line;
                        }

                        if (gap2 < -0.42) {
                           // it's a bad contact.  Add the 1st clash atom to the list of bad clashes.

                           std::string chain_id = atom_id_1_str.substr(0,2);
                           std::string atom_name = atom_id_1_str.substr(11,4);
                           std::string insertion_code = atom_id_1_str.substr(6,1);
                           std::string altconf = atom_id_1_str.substr(15,1);
                           if (altconf == " ")
                              altconf = "";

                           int resno = atoi(atom_id_1_str.substr(2,4).c_str());

                           // std::cout << "   makes atom " << chain_id << ":" << resno << ":"
                           // << insertion_code << ":" << atom_name << ":"
                           // << altconf << ":" << "\n";

                           coot::atom_spec_t at_spec(chain_id, resno, insertion_code, atom_name, altconf);

                           // 20211020-PE Wow this is very Old School way of adding to a set of atom specs
                           // (clash_atoms).
                           // The gui that uses clash atoms has been removed for the moment
                           bool ifound = false;
                           for (unsigned int ic=0; ic<clash_atoms.size(); ic++) {
                              if (resno == clash_atoms[ic].res_no) {
                                 if (insertion_code == clash_atoms[ic].ins_code) {
                                    if (altconf == clash_atoms[ic].alt_conf) {
                                       if (chain_id == clash_atoms[ic].chain_id) {

                                          // we are intested in bad (low) gaps
                                          if (gap2 < clash_atoms[ic].float_user_data)
                                             clash_atoms[ic].float_user_data = gap2;

                                          ifound = true;
                                          break;
                                       }
                                    }
                                 }
                              }
                           }
                           if (ifound == false) {
                              at_spec.int_user_data = imol;
                              at_spec.float_user_data = gap2;
                              at_spec.string_user_data = current_useful_name;
                              // don't put hydrogen bonds in the bad contact list
                              if (contact_type != "hb")
                                 clash_atoms.push_back(at_spec);
                           }
                        }
                     }
                  }
               }
            }
         }
         // setup the meshed_generic_display_objects that need a setup
         Material material;
         material.do_specularity = true;
         material.specular_strength = 1;
         material.shininess = 128;
         std::set<int>::const_iterator it;
         for (it=mgdos_needing_a_setup.begin(); it!=mgdos_needing_a_setup.end(); ++it) {
            g.generic_display_objects[*it].mesh.setup(material);
         }

         if (false) { // debugging
            for (it=mgdos_needing_a_setup.begin(); it!=mgdos_needing_a_setup.end(); ++it) {
               g.generic_display_objects[*it].mesh.debug();
            }
         }

         if (show_clash_gui_flag) {
            if (graphics_info_t::use_graphics_interface_flag) {
                if (show_clash_gui_flag == 2) {
                   // something here.
                }
            }
         }
         fclose(dots);
      }
   }
}
