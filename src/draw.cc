/*
 * src/draw.cc
 *
 * Copyright 2018 by Medical Research Council
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

#ifdef USE_PYTHON
#include <Python.h>
#endif // USE_PYTHON

#include <epoxy/gl.h>

#include "globjects.h"
#include "graphics-info.h"

#include "draw.hh"

void
stereo_projection_setup_maybe(GtkWidget *widget, short int in_stereo_flag) {

#if 0 // 20220602-PE ancient code

   bool do_first  = false;
   bool do_second = false;

   if (graphics_info_t::display_mode_use_secondary_p()) {
      if (widget == graphics_info_t::glareas[1]) {
         do_second = true;
      } else {
         do_first = true;
      }
   }

   if (in_stereo_flag == IN_STEREO_HARDWARE_STEREO) {
      if (graphics_info_t::which_eye == graphics_info_t::LEFT_EYE)
         do_second = true;
      if (graphics_info_t::which_eye == graphics_info_t::RIGHT_EYE)
         do_first = true;
   }

   if (false)
      std::cout << "debug:: stereo_projection_setup_maybe() in_stereo_flag "
                << in_stereo_flag << " vs "
                << IN_STEREO_SIDE_BY_SIDE_LEFT << " "
                << IN_STEREO_SIDE_BY_SIDE_RIGHT << " "
                << "do first and second " << do_first << " " << do_second << std::endl;

   if (do_first || do_second) {

      if (graphics_info_t::stereo_style_2010) {

         float skew_factor = 0.05 * graphics_info_t::hardware_stereo_angle_factor;
         float view_skew_matrix[16];

         // identity matrices first
         for(unsigned int ii=0; ii<16; ii++) view_skew_matrix[ii]   = 0.0;
         for(unsigned int ii=0; ii<4;  ii++) view_skew_matrix[ii*5] = 1.0;

         if (do_first) {
            view_skew_matrix[8] = skew_factor; // 8 because this is the transpose
            glMultMatrixf(view_skew_matrix);
            glTranslatef(-0.1, 0.0, 0.0); // maybe needs more, reversed sign
         } else {
            view_skew_matrix[8] = -skew_factor;
            glMultMatrixf(view_skew_matrix);
            glTranslatef(0.1, 0.0, 0.0);
         }

      } else {
         float skew_factor = 0.05 * graphics_info_t::hardware_stereo_angle_factor;
         float view_skew_matrix[16];

         // identity matrices first
         for(unsigned int ii=0; ii<16; ii++) view_skew_matrix[ii]   = 0.0;
         for(unsigned int ii=0; ii<4;  ii++) view_skew_matrix[ii*5] = 1.0;
         float trans_fac = 0.038;

         if (graphics_info_t::display_mode == coot::SIDE_BY_SIDE_STEREO_WALL_EYE) {
            if (do_first) {
               view_skew_matrix[8] = -skew_factor; // 8 because this is the transpose
               glMultMatrixf(view_skew_matrix);
               glTranslatef(-trans_fac, 0.0, 0.0);
            } else {
               view_skew_matrix[8] = skew_factor;
               glMultMatrixf(view_skew_matrix);
               glTranslatef(trans_fac, 0.0, 0.0);
            }
         }

         if (graphics_info_t::display_mode == coot::SIDE_BY_SIDE_STEREO) {
            if (do_first) {
               view_skew_matrix[8] = skew_factor; // 8 because this is the transpose
               glMultMatrixf(view_skew_matrix);
               glTranslatef(trans_fac, 0.0, 0.0);
            } else {
               view_skew_matrix[8] = -skew_factor;
               glMultMatrixf(view_skew_matrix);
               glTranslatef(-trans_fac, 0.0, 0.0);
            }
         }

         if (graphics_info_t::display_mode == coot::HARDWARE_STEREO_MODE) {
            if (do_first) {
               view_skew_matrix[8] = skew_factor; // 8 because this is the transpose
               glMultMatrixf(view_skew_matrix);
               glTranslatef(trans_fac, 0.0, 0.0);
            } else {
               view_skew_matrix[8] = -skew_factor;
               glMultMatrixf(view_skew_matrix);
               glTranslatef(-trans_fac, 0.0, 0.0);
            }
         }
      }
   }
#endif
}



#if 0 // historical reasons - remove this function when useless

gint
draw_mono(GtkWidget *widget, GdkEventExpose *event, short int stereo_mode) {

   // std::cout << "draw_mono() with widget " << widget << std::endl;

   if ((event-1) != 0) {
      /* Draw only last expose. */
      if (event->count > 0) {
         //       cout << "event->count is " << event->count << endl;
         //       cout << "chucking an event" << endl;
         return TRUE;
      }
   }

   // graphics_info_t info;          // static members
   // Those two classes should be rationalised.

   if (graphics_info_t::GetFPSFlag()) {
      graphics_info_t::ShowFPS();
   }

   // GLCONTEXT
   int gl_context = GL_CONTEXT_MAIN;
   if (stereo_mode == IN_STEREO_SIDE_BY_SIDE_RIGHT)
      gl_context = GL_CONTEXT_SECONDARY;

   GtkWidget *glarea_0 = 0;
   GtkWidget *glarea_1 = 0;
   graphics_info_t g;
   if (g.glareas.size() > 0) glarea_0 = g.glareas[0];
   if (g.glareas.size() > 1) glarea_1 = g.glareas[1];
   gl_context_info_t glci(glarea_0, glarea_1);

   bool is_bb = graphics_info_t::background_is_black_p();

   // void glDepthRange(GLclampd near, GLclampd far); Defines an encoding
   // for z coordinates that's performed during the viewport
   // transformation. The near and far values represent adjustments to the
   // minimum and maximum values that can be stored in the depth buffer. By
   // default, they're 0.0 and 1.0, respectively, which work for most
   // applications. These parameters are clamped to lie within [0,1].

   /* OpenGL functions can be called only if make_current returns true */
   if (graphics_info_t::make_current_gl_context(widget)) {

      GtkAllocation allocation;
      gtk_widget_get_allocation(widget, &allocation);
      float aspect_ratio = allocation.width/allocation.height;


      if (graphics_info_t::display_mode == coot::DTI_SIDE_BY_SIDE_STEREO) {
         aspect_ratio *= 2.0; // DTI side by side stereo mode
      }

      // Clear the scene
      //
      // BL says:: another hack!? FIXME
      // dont clear when we want to draw the 2 Zalman views
      if (in_stereo_flag != IN_STEREO_ZALMAN_LEFT)
         glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

      // From Bernhard

      // do these need to be here every frame?
      glEnable (GL_FOG);
      glFogi(GL_FOG_MODE, GL_LINEAR);
      glFogf(GL_FOG_START, -20.0);
      glFogf(GL_FOG_END, 20.0);
      glDepthFunc (GL_LESS);
      // glFogfv(GL_FOG_COLOR, graphics_info_t::background_colour);
      glEnable(GL_DEPTH_TEST);

      // BL:: this is code for Zalman monitor. Maybe can be somewhere else!?
      // Zalman works here?! but crap lighting!?

      if (in_stereo_flag == IN_STEREO_ZALMAN_RIGHT) {
         // draws one Zalman lines
         glEnable(GL_STENCIL_TEST);
         glStencilFunc(GL_EQUAL, 1, 1);
         glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
         glEnable(GL_STENCIL_TEST);

         /* red triangle for testing*/
         //glColor3ub(200, 0, 0);
         //glBegin(GL_POLYGON);
         //glVertex3i(-4, -4, 0);
         //glVertex3i(4, -4, 0);
         //glVertex3i(0, 4, 0);
         //glEnd();

         //glDisable(GL_STENCIL_TEST);
      }

      if (in_stereo_flag == IN_STEREO_ZALMAN_LEFT) {
         // g_print("BL DEBUG:: now draw 'right'\n");
         // draws the other Zalman lines
         glStencilFunc(GL_EQUAL, 0, 1);
         glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
         glEnable(GL_STENCIL_TEST);
         /* green square for testing */
         //glColor3ub(0, 200, 0);
         //glBegin(GL_POLYGON);
         //glVertex3i(3, 3, 0);
         //glVertex3i(-3, 3, 0);
         //glVertex3i(-3, -3, 0);
         //glVertex3i(3, -3, 0);
         //glEnd();
      }


      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();

      stereo_projection_setup_maybe(widget, stereo_mode);

      //          glOrtho(GLdouble left,   GLdouble right,
      //               GLdouble bottom, GLdouble top,
      //               GLdouble near,   GLdouble far);

      GLdouble near_scale = 0.1;
      //       if (! graphics_info_t::esoteric_depth_cue_flag)
      //          near_scale = 0.3;

      GLdouble near = -near_scale*graphics_info_t::zoom * (graphics_info_t::clipping_front*-0.1 + 1.0);
      GLdouble far  =        0.30*graphics_info_t::zoom * (graphics_info_t::clipping_back* -0.1 + 1.0);

      //         if (graphics_info_t::esoteric_depth_cue_flag)
      glOrtho(-0.3*graphics_info_t::zoom*aspect_ratio, 0.3*graphics_info_t::zoom*aspect_ratio,
              -0.3*graphics_info_t::zoom,  0.3*graphics_info_t::zoom,
              near, far);
      //          else
      //             glOrtho(-0.3*info.zoom*aspect_ratio, 0.3*info.zoom*aspect_ratio,
      //                     -0.3*info.zoom,  0.3*info.zoom,
      //                     -0.10*info.zoom,
      //                       +0.30*info.zoom);

      //glFogf(GL_FOG_START, -0.00*info.zoom);
      //glFogf(GL_FOG_END,    0.3*info.zoom);


      if (graphics_info_t::esoteric_depth_cue_flag) {
         glFogf(GL_FOG_START,  0.0f);
         glFogf(GL_FOG_END, far);
      } else {
         glFogf(GL_FOG_COORD_SRC, GL_FRAGMENT_DEPTH);
         glFogf(GL_FOG_DENSITY, 1.0);
         GLdouble fog_start = 0;
         GLdouble fog_end =  far;
         glFogf(GL_FOG_START,  fog_start);
         glFogf(GL_FOG_END,    fog_end);
         // std::cout << "GL_FOG_START " << fog_start << " with far  " << far  << std::endl;
         // std::cout << "GL_FOG_END "   << fog_end   << " with near " << near << std::endl;
      }

      if (false) { // try/test clipping
         // I don't understand what I need to do
         GLdouble plane[] = { 0.0, 0.0, -1.0, -2.0};
         glEnable(GL_CLIP_PLANE0);
         glClipPlane(GL_CLIP_PLANE0, plane);
         glPopMatrix();
      }

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      // Scene Rotation
      GL_matrix m;

      // no quat
      //  m.from_quaternion(graphics_info_t::quat); // consider a constructor.
      //       glMultMatrixf(m.get());

      // Translate the scene to the the view centre
      // i.e. the screenrotation center is at (X(), Y(), Z())
      //
      glTranslatef(-graphics_info_t::RotationCentre_x(),
                   -graphics_info_t::RotationCentre_y(),
                   -graphics_info_t::RotationCentre_z());

      draw_molecular_triangles(widget);

      if (false) { // try/test clipping
         // This does indeed clip the model, but it's in world coordinates,
         // not eye coordinates
         GLdouble plane[] = { 0.0, 0.0, -1.0, -2.0};
         glEnable(GL_CLIP_PLANE0);
         glClipPlane(GL_CLIP_PLANE0, plane);
         glPopMatrix();
      }

      if (! graphics_info_t::esoteric_depth_cue_flag) {
         coot::Cartesian front; // = unproject(0.0);
         coot::Cartesian back; //  = unproject(1.0);
         coot::Cartesian front_to_back = back - front;
         coot::Cartesian fbs = front_to_back.by_scalar(-0.2);
         // glTranslatef(fbs.x(), fbs.y(), fbs.z());
      }

      if (true) {
         glPushMatrix();
         glLoadIdentity(); // this doesn't seem to have an effect on mol-triangles lighting
         GLfloat  light_0_position[] = {  0.7,   0.0,   0.7, 0.0}; // 1 is positional, 0 is directional
         GLfloat  light_1_position[] = { -0.3,   0.2,   1.0, 0.0};
         GLfloat  light_2_position[] = {  0.7,  -0.7,  21.0, 0.0};
         GLfloat  light_3_position[] = {  0.7,  -0.7,  21.0, 0.0};
         GLfloat  light_4_position[] = {  0.7,   0.7, -21.0, 0.0};
         GLfloat  light_5_position[] = { -0.7,   0.7,  21.0, 0.0};

         glLightfv(GL_LIGHT0, GL_POSITION, light_0_position);
         glLightfv(GL_LIGHT1, GL_POSITION, light_1_position);
         glLightfv(GL_LIGHT2, GL_POSITION, light_2_position);

         glLightfv(GL_LIGHT3, GL_POSITION, light_3_position);
         glLightfv(GL_LIGHT4, GL_POSITION, light_4_position);
         glLightfv(GL_LIGHT5, GL_POSITION, light_5_position);

         glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION,    0.0);
         glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0);
         glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION,    0.0);
         glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0);
         glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION,    0.0);
         glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.0);

         glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION,    0.0);
         glLightf(GL_LIGHT3, GL_QUADRATIC_ATTENUATION, 0.0);
         glLightf(GL_LIGHT4, GL_LINEAR_ATTENUATION,    0.0);
         glLightf(GL_LIGHT4, GL_QUADRATIC_ATTENUATION, 0.0);
         glLightf(GL_LIGHT5, GL_LINEAR_ATTENUATION,    0.0);
         glLightf(GL_LIGHT5, GL_QUADRATIC_ATTENUATION, 0.0);

         // 	 GLfloat light_ambient[] =  { 0.1, 0.1, 0.1, 1.0 };
         // 	 GLfloat light_diffuse[] =  { 1.0, 1.0, 1.0, 1.0 };
         // 	 GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

         // 	 glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
         // 	 glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
         // 	 glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

         // 	 glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
         // 	 glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
         // 	 glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

         glPopMatrix();
      }

      glMatrixMode(GL_MODELVIEW);

      // do we need to turn on the lighting?
      int n_display_list_objects = 0;

      for (int ii=graphics_info_t::n_molecules()-1; ii>=0; ii--) {

         // Molecule stuff
         //
         // turning off cis-peptides goes hand in hand with turning off atom disks in
         // display_bonds(). We don't want to see them - they confuse the view
         bool dcp =  graphics_info_t::draw_cis_peptide_markups;
         if (graphics_info_t::moving_atoms_displayed_p())
            dcp = false;
         graphics_info_t::molecules[ii].draw_molecule(graphics_info_t::draw_zero_occ_spots_flag, is_bb, dcp);

         //
         graphics_info_t::molecules[ii].draw_dipoles();

         // draw display list objects
         if (graphics_info_t::molecules[ii].has_display_list_objects()) {
            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);
            glEnable(GL_LIGHT1);
            n_display_list_objects +=
               graphics_info_t::molecules[ii].draw_display_list_objects(gl_context);
            glDisable(GL_LIGHTING);
	      }

	 if (graphics_info_t::molecules[ii].draw_animated_ligand_interactions_flag) {
	    glEnable(GL_LIGHTING);
	    glEnable(GL_LIGHT0);
	    glEnable(GL_LIGHT1);
	    glDisable(GL_LIGHT2);
            gl_context_info_t gl_info = graphics_info_t::get_gl_context_info();
	    graphics_info_t::molecules[ii].draw_animated_ligand_interactions(gl_info,
                                                                             graphics_info_t::time_holder_for_ligand_interactions);
	    glDisable(GL_LIGHTING);
	 }

	 // draw anisotropic atoms maybe
	 graphics_info_t::molecules[ii].draw_anisotropic_atoms();

	 // We need to (also) pass whether we are drawing the first or
	 // secondary window, so that, when display lists are being
	 // used we use the correct part of theMapContours.
	 //
         // BL says:: bad hack FIXME
         if (stereo_mode == IN_STEREO_ZALMAN_LEFT || stereo_mode == IN_STEREO_ZALMAN_RIGHT) {
	    graphics_info_t::molecules[ii].draw_density_map(graphics_info_t::display_lists_for_maps_flag,
							    0);
         } else {
	    graphics_info_t::molecules[ii].draw_density_map(graphics_info_t::display_lists_for_maps_flag,
							    stereo_mode);
         }

         // Goodbye map drawing

         // Turn the light(s) on and after off, if needed.
         //
         graphics_info_t::molecules[ii].draw_surface();

         // extra restraints - thin blue lines or some such
         graphics_info_t::molecules[ii].draw_extra_restraints_representation();

         // Label the atoms in the atoms label list.
         //
         graphics_info_t::molecules[ii].draw_atom_labels(graphics_info_t::brief_atom_labels_flag,
                                                         graphics_info_t::seg_ids_in_atom_labels_flag);

         // Draw the dotted atoms:
         graphics_info_t::molecules[ii].draw_dots();

         // Draw Unit cell maybe.
         // graphics_info_t::molecules[ii].draw_coord_unit_cell(graphics_info_t::cell_colour);

         // Draw Map unit cell maybe;
         // graphics_info_t::molecules[ii].draw_map_unit_cell(graphics_info_t::cell_colour);

         //
         graphics_info_t::molecules[ii].draw_skeleton(is_bb);
      }

      // atom pull restraint
      // graphics_info_t::draw_atom_pull_restraint();


      // regularize object
      // graphics_info_t::draw_moving_atoms_graphics_object(is_bb); gone

      // and the pull restraint neighbour displacement radius (maybe)
      graphics_info_t::draw_pull_restraint_neighbour_displacement_max_radius_circle();

      // restraints for regularize/moving atoms object
      graphics_info_t::draw_moving_atoms_restraints_graphics_object();

      // environment object
      graphics_info_t::draw_environment_graphics_object();

      // flash the picked intermediate atom (Erik-mode)
      graphics_info_t::picked_intermediate_atom_graphics_object();

      //
      graphics_info_t::draw_baton_object();

      //
      graphics_info_t::draw_geometry_objects(); // angles and distances

      // pointer distances
      graphics_info_t::draw_pointer_distances_objects();

      // lsq atom blobs
      if (graphics_info_t::lsq_plane_atom_positions->size() > 0) {
         graphics_info_t g;
         g.render_lsq_plane_atoms();
      }

      // ligand flash bond
      graphics_info_t::draw_chi_angles_flash_bond();

      // draw reference object, which sits at the model origin.
      //
      if (graphics_info_t::show_origin_marker_flag) {
         glLineWidth(1.0);
         glColor3f(0.8,0.8,0.8);
         myWireCube (0.6);
      }

      graphics_info_t::draw_generic_objects();
      graphics_info_t::draw_generic_text();

      // Put a wirecube at the rotation centre.
      //
      glPushMatrix();
      glTranslatef(graphics_info_t::RotationCentre_x(),
                   graphics_info_t::RotationCentre_y(),
                   graphics_info_t::RotationCentre_z());

      draw_axes(m);

      graphics_info_t::graphics_ligand_view();

      glScalef (graphics_info_t::rotation_centre_cube_size,
                graphics_info_t::rotation_centre_cube_size,
                graphics_info_t::rotation_centre_cube_size);

      if (! graphics_info_t::smooth_scroll_on) {
         glLineWidth(2.0);
         glColor3f(0.8,0.6,0.7);
         myWireCube (1.0);
      }

      // Now we have finished displaying our annotation objects and
      // making transformations, lets put the matrix back how it used
      // to be.
      glPopMatrix();

      // Note that is important to leave with the modelling and
      // view matrices that are the same as those that were when
      // the molecule was drawn.  Atom picking depends on this.

      // Transparent density maps
      //
      for (int ii=graphics_info_t::n_molecules()-1; ii>=0; ii--) {
         if (graphics_info_t::is_valid_map_molecule(ii)) {
            // enable lighting internal to this function
            bool do_flat =
               graphics_info_t::do_flat_shading_for_solid_density_surface;
            graphics_info_t::molecules[ii].draw_solid_density_surface(do_flat);
         }
      }

      //
      draw_crosshairs_maybe();

      //
      display_density_level_maybe();

      // BL says:: not sure if we dont need to do this for 2nd Zalman view
      if (in_stereo_flag != IN_STEREO_HARDWARE_STEREO && in_stereo_flag != IN_STEREO_ZALMAN_RIGHT) {

#if 0 // OpenGL interface

         /* Swap backbuffer to front */
         GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);
         if (gdk_gl_drawable_is_double_buffered (gldrawable)) {
            gdk_gl_drawable_swap_buffers (gldrawable);
         } else {
            glFlush ();
         }
         graphics_info_t::Increment_Frames();
#endif

      }


      if (graphics_info_t::display_mode == coot::ZALMAN_STEREO)
         glDisable(GL_STENCIL_TEST);

      // show_lighting();

   } // gtkgl make area current test


   gdkglext_finish_frame(widget);

   return TRUE;
}
#endif

void draw_molecular_triangles(GtkWidget *widget) {

   // goodbye innards.
}

void gdkglext_finish_frame(GtkWidget *widget) {

#if 0
   // should not even be called by GTK.
   GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);
   gdk_gl_drawable_gl_end (gldrawable);
#endif

}


void
display_density_level_maybe() {

   if (graphics_info_t::display_density_level_on_screen == 1) {
      if (graphics_info_t::display_density_level_this_image == 1) {

    //	 std::cout << "DEBUG:: screen label: "
    // << graphics_info_t::display_density_level_screen_string
    // << std::endl;

    bool is_bb = graphics_info_t::background_is_black_p();

    GLfloat white[3] = {1.0, 1.0, 1.0};
    GLfloat black[3] = {0.0, 0.0, 0.0};

    if (is_bb)
       glColor3fv(white);
    else
       glColor3fv(black);

    glPushMatrix();
    glLoadIdentity();

 	 glMatrixMode(GL_PROJECTION);
    glPushMatrix();
 	 glLoadIdentity();

    // Disable the fog so that the density level text will not
    // change intensity in a zoom-dependent way:
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_FOG);

    // glRasterPos3f();
    graphics_info_t::printString_for_density_level(graphics_info_t::display_density_level_screen_string,
   0.0, 0.95, -0.9);

         glPopAttrib();
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glPopMatrix();

      }
   }
   graphics_info_t::display_density_level_this_image = 0;

   // glScalef (2.0, 2.0, 2.0);
   // glRasterPos3f(2.0, 28.0,0.0);

   // This does resonable things when changing the contour level (it
   // waits until each change has been made before returning (rather
   // than combining them into one change), but terrible things to
   // regular rotation (unusable jerkiness).
//    while (gtk_events_pending())
//       gtk_main_iteration();

}

/* When widget is exposed it's contents are redrawn. */
#if 0
gint draw(GtkWidget *widget, GdkEventExpose *event) {

//    GtkWidget *w = graphics_info_t::glarea;
//    GdkGLContext *glcontext = gtk_widget_get_gl_context (w);
//    GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (w);
//    int i = gdk_gl_drawable_gl_begin (gldrawable, glcontext);
//    std::cout << "DEBUG gdk_gl_drawable_gl_begin returns state: "
// 	     << i << std::endl;
//    if (i == 0)
//       return TRUE;

// for every graphics_draw()
// this function is called for glarea (and then glarea_2 if needed)

#ifdef USE_PYTHON
   // Hamish function
   if (! graphics_info_t::python_draw_function_string.empty()) {
      PyRun_SimpleString(graphics_info_t::python_draw_function_string.c_str());
   }
#endif
   if (graphics_info_t::display_mode == coot::HARDWARE_STEREO_MODE) {
      draw_hardware_stereo(widget, event);
   } else {
      if (graphics_info_t::display_mode == coot::ZALMAN_STEREO) {
         draw_zalman_stereo(widget, event);
      } else {

         if (graphics_info_t::display_mode_use_secondary_p()) {
            if (graphics_info_t::glareas.size() == 2) {
               draw_mono(widget, event, IN_STEREO_SIDE_BY_SIDE_RIGHT);
            } else {
               draw_mono(widget, event, IN_STEREO_SIDE_BY_SIDE_LEFT);
            }
         } else {
            draw_mono(widget, event, IN_STEREO_MONO);
         }
      }
   }
   return TRUE;
}
#endif


#if 0 // maybe useful future? but remove for now 20220228-PE

gint draw_hardware_stereo(GtkWidget *widget, GdkEventExpose *event) {

   if (graphics_info_t::stereo_style_2010) {

      // tinker with graphics_info_t::quat, rotate it left, draw it,
      // rotate it right, draw it.
      graphics_info_t g; // is this a slow thing?
      float tbs =  g.get_trackball_size();
      float spin_quat[4];
      // 0.0174 = 1/(2*pi)
      trackball(spin_quat, 0, 0, -g.hardware_stereo_angle_factor*0.0358, 0.0, tbs);
      add_quats(spin_quat, graphics_info_t::quat, graphics_info_t::quat);

      // draw right:
      glDrawBuffer(GL_BACK_RIGHT);
      draw_mono(widget, event, IN_STEREO_HARDWARE_STEREO);

      trackball(spin_quat, 0, 0, 2.0*g.hardware_stereo_angle_factor*0.0358, 0.0, tbs);
      add_quats(spin_quat, graphics_info_t::quat, graphics_info_t::quat);

      // draw left:
      glDrawBuffer(GL_BACK_LEFT);
      draw_mono(widget, event, IN_STEREO_HARDWARE_STEREO);

      // reset the viewing angle:
      trackball(spin_quat, 0, 0, -g.hardware_stereo_angle_factor*0.0358, 0.0, tbs);
      add_quats(spin_quat, graphics_info_t::quat, graphics_info_t::quat);
      graphics_info_t::which_eye = graphics_info_t::FRONT_EYE;

      // show it
      GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);
      gdk_gl_drawable_swap_buffers(gldrawable);

   } else {

      // do the skew thing in draw_mono() depending on which stereo eye.

      // draw right:
      graphics_info_t::which_eye = graphics_info_t::RIGHT_EYE;
      glDrawBuffer(GL_BACK_RIGHT);
      draw_mono(widget, event, IN_STEREO_HARDWARE_STEREO);

      // draw left:
      graphics_info_t::which_eye = graphics_info_t::LEFT_EYE;
      glDrawBuffer(GL_BACK_LEFT);
      draw_mono(widget, event, IN_STEREO_HARDWARE_STEREO);

      // reset the viewing angle:
      graphics_info_t::which_eye = graphics_info_t::FRONT_EYE;

      // show it
      GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);
      gdk_gl_drawable_swap_buffers(gldrawable);
   }
   return TRUE;
}
#endif


#if 0

gint draw_zalman_stereo(GtkWidget *widget, GdkEventExpose *event) {

   // tinker with graphics_info_t::quat, rotate it left, draw it,
   // rotate it right, draw it.
   graphics_info_t g; // is this a slow thing?
   float tbs =  g.get_trackball_size();
   float spin_quat[4];
   // 0.0174 = 1/(2*pi)
   if (graphics_info_t::display_mode == coot::ZALMAN_STEREO) {
     GLint viewport[4];
     glGetIntegerv(GL_VIEWPORT,viewport);

     glPushAttrib(GL_ENABLE_BIT);
     glMatrixMode(GL_PROJECTION);
     glPushMatrix();
     glLoadIdentity();
     glOrtho(0,viewport[2],0,viewport[3],-10.0,10.0);
     glMatrixMode(GL_MODELVIEW);
     glPushMatrix();
     glLoadIdentity();
     glTranslatef(0.33F,0.33F,0.0F);
     glDisable(GL_STENCIL_TEST);
     /* We/you may not need all of these... FIXME*/

     glDisable(GL_ALPHA_TEST);
     glDisable(GL_LIGHTING);
     glDisable(GL_FOG);
     glDisable(GL_NORMALIZE);
     glDisable(GL_DEPTH_TEST);
     glDisable(GL_COLOR_MATERIAL);
     glDisable(GL_LINE_SMOOTH);
     glDisable(GL_DITHER);
     glDisable(GL_BLEND);
     glShadeModel(GL_SMOOTH);
     glDisable(0x809D); /* GL_MULTISAMPLE_ARB */

     //glDisable(GL_STENCIL_TEST);
     glClearStencil(0);
     glColorMask(false,false,false,false);
     glDepthMask(false);
     glClear(GL_STENCIL_BUFFER_BIT);

     glEnable(GL_STENCIL_TEST);
     glStencilFunc(GL_ALWAYS, 1, 1);
     glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

     glLineWidth(1.0);
     glBegin(GL_LINES);
     int h = viewport[3], w=viewport[2];
     int y;
     for(y=0;y<h;y+=2) {
       glVertex2i(0,y);
       glVertex2i(w,y);
     }
     glEnd();

     glColorMask(true,true,true,true);
     glDepthMask(true);

     glMatrixMode(GL_MODELVIEW);
     glPopMatrix();
     glMatrixMode(GL_PROJECTION);
     glPopMatrix();
     //
     glPopAttrib();
   }

   trackball(spin_quat, 0, 0, -g.hardware_stereo_angle_factor*0.0358, 0.0, tbs);
   add_quats(spin_quat, graphics_info_t::quat, graphics_info_t::quat);

   // draw right (should maybe be left)??:
   draw_mono(widget, event, IN_STEREO_ZALMAN_RIGHT); // 5 for right

   trackball(spin_quat, 0, 0, 2.0*g.hardware_stereo_angle_factor*0.0358, 0.0, tbs);
   add_quats(spin_quat, graphics_info_t::quat, graphics_info_t::quat);

   // draw left (should maybe be right)??:
   draw_mono(widget, event, IN_STEREO_ZALMAN_LEFT); // 6 for left

   // reset the viewing angle:
   trackball(spin_quat, 0, 0, -g.hardware_stereo_angle_factor*0.0358, 0.0, tbs);
   add_quats(spin_quat, graphics_info_t::quat, graphics_info_t::quat);

   return TRUE;
}
#endif



#include "c-interface-generic-objects.h"


void
debug_eye_position(GtkWidget *widget) {

   coot::Cartesian rc(graphics_info_t::RotationCentre_x(),
                      graphics_info_t::RotationCentre_y(),
                      graphics_info_t::RotationCentre_z());

   coot::Cartesian ep(0,0,0); // needs a fix up if you want to resurect this functionn

   coot::Cartesian pt((ep + rc) * 0.5);

   int go = generic_object_index("eye position");
   if (go == -1)
      go = new_generic_object_number("eye position");

   to_generic_object_add_point(go, "red", 4, pt.x(), pt.y(), pt.z());
   set_display_generic_object(go, 1);
}
 
