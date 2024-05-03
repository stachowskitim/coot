/*
 * src/old-generic-display-object.cc
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


#if defined (USE_PYTHON)
#include "Python.h"
#include "python-3-interface.hh"
#endif

#include "graphics-info.h"
#include "c-interface-generic-objects.h"
#include "c-interface-python.hh" // for display_python

#if defined USE_PYTHON
PyObject *colour_holder_to_py(const coot::colour_holder &c) {

   PyObject *r = PyDict_New();
   PyDict_SetItemString(r, "red",   PyFloat_FromDouble(c.red));
   PyDict_SetItemString(r, "green", PyFloat_FromDouble(c.green));
   PyDict_SetItemString(r, "blue",  PyFloat_FromDouble(c.blue));

   return r;
}
#endif

#if defined USE_PYTHON
PyObject *get_generic_object_py(unsigned int idx) {

   // Why did I want to do this? Anyway, much too tricky to fix now.

   PyObject *r = Py_False;

#if 0
   graphics_info_t g;
   unsigned int size = g.generic_objects_p->size();

   if (idx < size) {
      r = PyDict_New();
      const coot::old_generic_display_object_t &gdo = g.generic_objects_p->at(idx);
      if (! gdo.is_closed_flag) {
	 // also, is_transparent_flag, is_solid_flag, opacity
	 PyDict_SetItemString(r, "name", myPyString_FromString(gdo.name.c_str()));
	 PyDict_SetItemString(r, "imol", PyLong_FromLong(gdo.imol));
	 PyDict_SetItemString(r, "is_displayed_flag", PyLong_FromLong(gdo.is_displayed_flag));
	 PyDict_SetItemString(r, "is_solid_flag", PyLong_FromLong(gdo.is_solid_flag));
	 if (gdo.lines_set.size() > 0) {
	    PyObject *lines_set_py = PyList_New(gdo.lines_set.size());
	    for (std::size_t i=0; i<gdo.lines_set.size(); i++) {
	       const coot::old_generic_display_line_set_t &ls = gdo.lines_set[i];
	       PyObject *line_set_dict = PyDict_New();
	       PyObject *colour_py = colour_holder_to_py(ls.colour);
	       PyObject *lines_py = PyList_New(ls.lines.size());
	       for (std::size_t j=0; j<ls.lines.size(); j++) {
		  const coot::old_generic_display_line_t &l = ls.lines[j];
		  PyObject *pt_0_py = PyList_New(3);
		  PyObject *pt_1_py = PyList_New(3);
		  for (std::size_t jj=0; jj<3; jj++){
		     PyList_SetItem(pt_0_py, jj, PyFloat_FromDouble(l.coords.first[jj]));
		     PyList_SetItem(pt_1_py, jj, PyFloat_FromDouble(l.coords.second[jj]));
		  }
		  PyObject *line_py = PyList_New(2);
		  PyList_SetItem(line_py, 0, pt_0_py);
		  PyList_SetItem(line_py, 1, pt_1_py);
		  PyList_SetItem(lines_py, j, line_py);
	       }
	       PyDict_SetItemString(line_set_dict, "colour", colour_py);
	       PyDict_SetItemString(line_set_dict, "colour_name", myPyString_FromString(ls.colour_name.c_str()));
	       PyDict_SetItemString(line_set_dict, "width", PyLong_FromLong(ls.width));
	       PyDict_SetItemString(line_set_dict, "lines", lines_py);
	       PyList_SetItem(lines_set_py, i, line_set_dict);
	    }
	    PyDict_SetItemString(r, "lines_set", lines_set_py);
	 }
	 if (gdo.points_set.size() > 0) {
	    PyObject *points_set_py = PyList_New(gdo.points_set.size());
	    for (std::size_t i=0; i<gdo.points_set.size(); i++) {
	       const coot::old_generic_display_point_set_t &ps = gdo.points_set[i];
	       PyObject *point_set_dict = PyDict_New();
	       PyObject *colour_py = colour_holder_to_py(ps.colour);
	       PyObject *points_py = PyList_New(ps.points.size());
	       PyObject *size_py   = PyLong_FromLong(ps.size); // radius
	       for (std::size_t j=0; j<ps.points.size(); j++) {
		  const clipper::Coord_orth &pt = ps.points[j];
		  PyObject *pt_py = PyList_New(3);
		  for (std::size_t jj=0; jj<3; jj++)
		     PyList_SetItem(pt_py, jj, PyFloat_FromDouble(pt[jj]));
		  PyList_SetItem(points_py, j, pt_py);
	       }
	       PyDict_SetItemString(point_set_dict, "colour", colour_py);
	       PyDict_SetItemString(point_set_dict, "colour_name", myPyString_FromString(ps.colour_name.c_str()));
	       PyDict_SetItemString(point_set_dict, "size", size_py);
	       PyDict_SetItemString(point_set_dict, "points", points_py);
	       PyList_SetItem(points_set_py, i, point_set_dict);
	    }
	    PyDict_SetItemString(r, "points_set", points_set_py);
	 }
      }
   }

#endif
   if (PyBool_Check(r)) {
      Py_XINCREF(r);
   }
   return r;
}
#endif
