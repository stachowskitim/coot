/*
 * src/cc-interface-scripting.cc
 *
 * Copyright 2020 by Medical Research Council
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
#include "c-interface-python.hh"
#endif

#include <iostream>

// these are needed to comiple the cc-interface-scripting.hh
#include <string>
#include <vector>

#ifdef USE_GUILE
#include <cstdio> /* for std::FILE in gmp.h for libguile.h */
#include <libguile.h>
#endif

#include "graphics-info.h"
#include "cc-interface-scripting.hh"
#include "key-bindings.hh"

// put other scripting functions in here

#ifdef USE_PYTHON
// Prefered:
void add_key_binding_gtk4_py(PyObject *key_py, int ctrl_key, PyObject *func, const std::string &description) {

   int key = 0;

   if (PyLong_Check(key_py)) {
      key = PyLong_AsLong(key_py);
   } else {
      if (PyUnicode_Check(key_py)) {
         std::string key_str = PyBytes_AS_STRING(PyUnicode_AsUTF8String(key_py));
         // now convert the string to a key code.
         if (!key_str.empty()) {
            guint kk = gdk_unicode_to_keyval(key_str[0]);
            std::cout << "debug here with kk " << kk << std::endl;
            key = kk;
         } else {
            std::cout << "WARNING:: empty key " << std::endl;
         }
      }
   }
   if (key != 0) {
      keyboard_key_t k(key, ctrl_key);
      bool func_is_valid = true;
      if (! PyCallable_Check(func))
         func_is_valid = false;
      if (func_is_valid) {
         key_bindings_t kb(func, description);
         graphics_info_t::add_key_binding(k, kb);
      }
   } else {
      // use display on this
      std::cout << "WARNGING:: add_key_binding_gtk3_py() failed to interpet "
                << PyBytes_AS_STRING(PyUnicode_AsUTF8String(display_python(key_py)))
                << std::endl;
   }
}
#endif


#ifdef USE_GUILE
void add_key_binding_gtk3_scm(int key, int ctrl_key, SCM thunk, const std::string &description) {

}
#endif


void reload_shaders() {

   std::cout << "INGO:: reload_shaders() " << std::endl;
   graphics_info_t g;
   g.screen_framebuffer.tear_down();
   g.blur_framebuffer.tear_down();
   // gtk_gl_area_attach_buffers(GTK_GL_AREA(g.glareas[0]));  // 20210912-PE added // 20210930-PE removed again
   g.init_shaders(); // regenerates screen and blur framebuffers also
   g.graphics_draw();

}

void reload_shader(const std::string &shader_name) {

   graphics_info_t g;
   g.init_shader(shader_name); // 20230905-PE perhaps this should be named reload_shader()?

}


#ifdef USE_PYTHON
void set_light_position_py(int light_id, float x, float y, float z) {

   glm::vec4 pos(x,y,z,1.0);
   graphics_info_t::lights[light_id].position = pos;
   graphics_info_t g;
   g.graphics_draw();
   glFlush();
}
#endif


//! \brief enable or diable sounds (coot needs to have been compiled with sounds of course)
void set_use_sounds(bool state) {
   graphics_info_t::use_sounds = state;
}

//! no sounds
void curmudgeon_mode() {
   graphics_info_t::use_sounds = false;
   graphics_info_t::curmudgeon_mode = true;
}


void halloween() {

   // make a graphical object
   graphics_info_t g;
   g.pumpkin();

}
