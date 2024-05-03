/*
 * src/key-bindings.hh
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

#ifndef KEY_BINDINGS_HH
#define KEY_BINDINGS_HH

#ifdef USE_PYTHON
#include <Python.h>
#endif

// Python header has already been loaded by now.

#include <string>
#include <gtk/gtk.h>

#ifdef USE_GUILE
#include <cstdio> /* for std::FILE in gmp.h for libguile.h */
#include <libguile.h>
#endif

class keyboard_key_t {
public:
   int gdk_key;
   bool ctrl_is_pressed;
   keyboard_key_t(int g) {
      gdk_key = g;
      ctrl_is_pressed = false;
   }
   keyboard_key_t(int g, bool state) {
      gdk_key = g;
      ctrl_is_pressed = state;
   }
   bool operator<(const keyboard_key_t &other) const {
      if (other.gdk_key < gdk_key) {
         return true;
      } else {
         if (other.gdk_key == gdk_key) {
            if (other.ctrl_is_pressed) {
               if (ctrl_is_pressed) {
                  return false;
               } else {
                  return true;
               }
            } else {
               if (ctrl_is_pressed) {
                  return false; // hmm.
               } else {
                  return false;
               }
            }
         } else {
            return false;
         }
      }
   }
};

class key_bindings_t {

public:
   enum binding_type { NONE, SCHEME, PYTHON, BUILT_IN };
   binding_type type;
   std::string scripting_function_text;
#ifdef USE_GUILE
   SCM thunk;
#endif
#ifdef USE_PYTHON
   PyObject *function_py;
#endif
   std::string description;
   gboolean (*func)();
   key_bindings_t() {
           type = NONE;
#ifdef USE_PYTHON
           function_py = 0;
#endif
           func = 0; }
   // external
#ifdef USE_GUILE
   key_bindings_t(SCM thunk_in, const std::string &descr) : thunk(thunk_in), description(descr) {
      type = SCHEME;
      func = 0;
   }
#endif
#ifdef USE_PYTHON
   key_bindings_t(PyObject *func_py, const std::string &descr) : description(descr) {
      type = PYTHON;
      function_py = func_py;
      func = 0;
   }
   key_bindings_t(const std::string &function_as_string, const std::string &descr) :
      scripting_function_text(function_as_string), description(descr) {
      type = PYTHON;
      function_py = 0;
      func = 0;
   }
#endif
   key_bindings_t(binding_type bt, const std::string &fn, const std::string &description_in) :
      type(bt), scripting_function_text(fn), description(description_in) {
#ifdef USE_PYTHON
      function_py = 0;
#endif
      func = 0;
   }
   // internal
   key_bindings_t(gboolean (*func_in) (), const std::string &description_in) : description(description_in) {
      type = BUILT_IN;
      func = func_in;
#ifdef USE_PYTHON
      function_py = 0;
#endif
   }
   gboolean run() const;
};

#endif // KEY_BINDINGS_HH

