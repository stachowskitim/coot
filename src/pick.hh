/*
 * src/pick.hh
 *
 * Copyright 2019 by Medical Research Council
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
//
#ifndef PICK_HH
#define PICK_HH

#include <string>

#include <gtk/gtk.h>
#include <epoxy/gl.h>

#include <mmdb2/mmdb_manager.h>
#include "clipper/core/coords.h"

#include "coords/mmdb-extras.h"
#include "coords/mmdb-crystal.h"

#include "clip-hybrid-atom.hh"

// These unproject functions now return a glm::vec4 and are in graphics_info_t
// coot::Cartesian unproject(float screen_z);
// coot::Cartesian unproject_xyz(int x, int y, float screen_z);

class pick_info {
 public:
   bool is_intermediate_atoms_molecule;
   int success;
   int model_number;
   int atom_index;
   int imol;
   float min_dist;
   pick_info() {
      is_intermediate_atoms_molecule = false;
      success = GL_FALSE;
      model_number = -1;
      atom_index = -1;
      imol = -1;
      min_dist = -1;
   }
};

// this is a class, because it contains a class (symm_trans_t)
//
class symm_atom_info_t {

 public:
   symm_atom_info_t() {
      success = 0;
      imol = -1;
      atom_index = 0;
      n_trans_sel = -1;
   };
   int success;
   int atom_index;
   symm_trans_t symm_trans;
   mmdb::PPAtom trans_sel;
   int n_trans_sel;
   int imol;

};

namespace coot {

   class Symm_Atom_Pick_Info_t {
   public:
      int success;
      int atom_index;
      int imol;
      clip_hybrid_atom hybrid_atom;
      symm_trans_t symm_trans;
      Cell_Translation pre_shift_to_origin;
      Symm_Atom_Pick_Info_t() : symm_trans(symm_trans_t(-1, -999, -999, -999)) {
         success = 0;
         imol = -1;
         atom_index = -1;
      }
      clip_hybrid_atom Hyb_atom() const {
         return hybrid_atom;
      }
   };

}


enum { PICK_ATOM_ALL_ATOM, PICK_ATOM_CA_ONLY, PICK_ATOM_CA_OR_LIGAND, PICK_ATOM_NON_HYDROGEN, PICK_ATOM_CA_OR_SIDECHAIN_OR_LIGAND };

#if (GTK_MAJOR_VERSION >= 4)
#else
// a NULL event can be passed - in that case the check for CTRL press is not made.
pick_info atom_pick(GdkEventButton *event); // atom index in the atom selection
#endif


// pick_info atom_pick_gtk3(); // and that wraps the below function:

// Now in graphics_info_t
// pick_info atom_pick_gtk3(bool intermediate_atoms_only_flag);


// pick_info moving_atoms_atom_pick(); not here, it's in graphics.

pick_info pick_atom(const atom_selection_container_t &SelAtom, int imol,
                    const coot::Cartesian &front, const coot::Cartesian &back,
                    short int pick_mode, bool verbose_mode);

pick_info pick_intermediate_atom(const atom_selection_container_t &SelAtom);


// symm_atom_info_t symmetry_atom_pick();

coot::CartesianPair
screen_x_to_real_space_vector(GtkWidget *widget);
coot::Cartesian screen_z_to_real_space_vector(GtkWidget *widget);

std::string make_symm_atom_label_string(mmdb::PAtom atom, symm_trans_t symm_trans);

coot::Symm_Atom_Pick_Info_t symmetry_atom_pick();
// used by symmetry_atom_pick():
void
fill_hybrid_atoms(std::vector<coot::clip_hybrid_atom> *hybrid_atoms,
                  const atom_selection_container_t &basc,
                  const clipper::Spacegroup &spg,
                  const clipper::Cell &cell,
                  const symm_trans_t &symm_trans);


// void clear_symm_atom_info(symm_atom_info_t symm_atom_info);


// now in molecule_class_info, where they should have been.
/* std::string make_atom_label_string(mmdb::PAtom atom); */
/* std::string make_atom_label_string_old(mmdb::PAtom atom); */



#endif // PIck
