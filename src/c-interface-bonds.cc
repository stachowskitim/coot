/*
 * src/c-interface-bonds.cc
 *
 * Copyright 2017 by Medical Research Council
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
#include <Python.h>  // before system includes to stop "POSIX_C_SOURCE" redefined problems
#endif

// #include "compat/coot-sysdep.h"

// #include <stdlib.h>
#include <iostream>

// #include <vector>
// #include <string>
// #include <algorithm>

#include <mmdb2/mmdb_manager.h>
#include "coords/mmdb-extras.h"
#include "coords/mmdb.hh"

#include "coords/mmdb-crystal.h"

#include "coords/Cartesian.h"
#include "coords/Bond_lines.h"

#include "graphics-info.h"

#include "coot-utils/coot-coord-utils.hh"

#include "c-interface.h"
// #include "c-interface-gtk-widgets.h"
#include "cc-interface.hh"
#include "cc-interface-scripting.hh"

#include "c-interface-bonds.hh"

#include "coot-utils/coot-h-bonds.hh"
#include "glarea_tick_function.hh"

#ifdef USE_PYTHON

//! \brief return a Python object for the bonds
//
PyObject *get_bonds_representation(int imol) {

   // Because "residue picking/selection" is crucial for so many coot tools, we want to support
   // a mechanism that allows the client-side display of the "active" residue, we do that by providing
   // a "residue-index" for every atom and bond.

   // currently we use get_at_pos() to convert from coordinates to a atom.
   // This is easier to program, but slow and really a silly thing to do.  It would be better
   // to add the residue_index for the atoms and bonds when the 
   // bonds and atom positions are created.
   // if get_bonds_respresenation() is slow, go back and fix it.
   // 3wj7 takes 0.78s for just the atom loop get_at_pos().

   // Cartesian doesn't yet work as key of a map
   // std::map<coot::Cartesian, mmdb::Residue *> residue_map;


   PyObject *r = Py_False;

   if (is_valid_model_molecule(imol)) {

      graphics_info_t g;
      // write a class that also returns atom specs for the atom. Possibly use inheritance
      // or encapsulation.
      graphical_bonds_container bonds_box = graphics_info_t::molecules[imol].get_bonds_representation();
      r = g.pyobject_from_graphical_bonds_container(imol, bonds_box);
   }
   if (PyBool_Check(r)) {
      Py_INCREF(r);
   }
   return r;

}

//! \brief return a Python object for the bonds
//
PyObject *get_intermediate_atoms_bonds_representation() {

   graphics_info_t g;
   return g.get_intermediate_atoms_bonds_representation();
}




PyObject *get_environment_distances_representation_py(int imol, PyObject *residue_spec_py) {

   PyObject *r = Py_False;

   if (is_valid_model_molecule(imol)) {

      coot::residue_spec_t residue_spec = residue_spec_from_py(residue_spec_py);
      graphics_info_t g;
      graphical_bonds_container gbc = g.molecules[imol].make_environment_bonds_box(residue_spec, g.Geom_p());
      r = g.pyobject_from_graphical_bonds_container(imol, gbc);
   }

   if (PyBool_Check(r)) { 
      Py_INCREF(r);
   }
   return r;

}

#include "python-3-interface.hh"

//! \brief return a Python object for the radii of the atoms in the dictionary
//
PyObject *get_dictionary_radii() {

   bool use_vdwH_flag = false; // I suppose caller can pass this

   PyObject *r = Py_False;
   int imol_enc = coot::protein_geometry::IMOL_ENC_ANY;

   graphics_info_t g;
   PyObject *residue_dict_py = PyDict_New();
   for (std::size_t i=0; i<g.Geom_p()->size(); i++) {
      const coot::dictionary_residue_restraints_t &rest = g.Geom_p()->get_monomer_restraints(i);
      const std::string residue_name = rest.comp_id();
      const std::vector<coot::dict_atom> &ai = rest.atom_info;
      PyObject *atoms_dict_py = PyDict_New();
      for (std::size_t j=0; j<ai.size(); j++) {
	 const std::string &atom_name = ai[j].atom_id_4c;
	 double r = g.Geom_p()->get_vdw_radius(atom_name, residue_name, imol_enc, use_vdwH_flag);
	 PyDict_SetItem(atoms_dict_py, myPyString_FromString(atom_name.c_str()), PyFloat_FromDouble(r));
      }
      PyDict_SetItem(residue_dict_py, myPyString_FromString(residue_name.c_str()), atoms_dict_py);
   }
   r = residue_dict_py;
   return r;
}



// deliberately inside the Python test

//! \brief return the continue-updating-refinement-atoms state
//
// 0 means off, 1 means on.
int get_continue_updating_refinement_atoms_state() {

   // bool to int
   return graphics_info_t::continue_update_refinement_atoms_flag;

}

#endif // Python


#ifdef USE_GUILE
SCM test_mol_triangles_scm(SCM i_scm, SCM j_scm) {

   SCM r = SCM_BOOL_F;

#ifdef USE_MOLECULES_TO_TRIANGLES
   if (true) {

      int imol = scm_to_int(i_scm);
      if (is_valid_model_molecule(imol)) {
	 graphics_info_t::molecules[imol].make_molecularrepresentationinstance("//", "RampChainsScheme", "Ribbon");
	 // graphics_info_t::mol_tri_scene_setup->addRepresentationInstance(graphics_info_t::molecules[imol].molrepinst);
	 graphics_draw();
      }
   }
#endif // USE_MOLECULES_TO_TRIANGLES

   return r;
}
#endif


void set_draw_hydrogen_bonds(int state) {

   graphics_info_t::mesh_for_hydrogen_bonds.set_draw_this_mesh(state);

}


void calculate_hydrogen_bonds(int imol) {

   auto mmdb_to_glm = [] (mmdb::Atom *at) { return glm::vec3(at->x, at->y, at->z); };

   if (is_valid_model_molecule(imol)) {
      coot::h_bonds hbs;
      mmdb::Manager *mol = graphics_info_t::molecules[imol].atom_sel.mol;
      int sel_1 = graphics_info_t::molecules[imol].atom_sel.SelectionHandle;
      int sel_2 = graphics_info_t::molecules[imol].atom_sel.SelectionHandle;
      const coot::protein_geometry &geom(*graphics_info_t::Geom_p());
      std::vector<coot::h_bond> h_bonds = hbs.get(sel_1, sel_2, mol, geom, imol);

      std::cout << "INFO:: Found " << h_bonds.size() << " hydrogen bonds " << std::endl;

      graphics_info_t::hydrogen_bonds_atom_position_pairs.clear();
      for (unsigned int i=0; i<h_bonds.size(); i++) {
         const coot::h_bond hb = h_bonds[i];
         if (hb.has_hydrogen()) {
            mmdb::Atom *at_1 = hb.hb_hydrogen;
            mmdb::Atom *at_2 = hb.acceptor;
            if (at_1 && at_2) {
               glm::vec3 p_1 = mmdb_to_glm(at_1);
               glm::vec3 p_2 = mmdb_to_glm(at_2);
               graphics_info_t::hydrogen_bonds_atom_position_pairs.push_back(std::pair<glm::vec3, glm::vec3>(p_2, p_1));
            }

         } else {
            mmdb::Atom *at_1 = hb.donor;
            mmdb::Atom *at_2 = hb.acceptor;
            if (at_1 && at_2) {
               glm::vec3 p_1 = mmdb_to_glm(at_1);
               glm::vec3 p_2 = mmdb_to_glm(at_2);
               graphics_info_t::hydrogen_bonds_atom_position_pairs.push_back(std::pair<glm::vec3, glm::vec3>(p_2, p_1));
            }
         }
      }

      std::string label = "Hydrogen Bonds for Molecule  " + std::to_string(imol);
      graphics_info_t::update_hydrogen_bond_mesh(label);

   }
}
