/* src/molecule-class-info-bonds.cc
 *
 * Copyright 2016 by Medical Research Council
 * Author: Paul Emsley
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

#include "molecule-class-info.h"

#include "graphics-info.h" // for attach_buffers()


void
molecule_class_info_t::set_user_defined_colour_indices_by_selections(const std::vector<std::pair<std::string, unsigned int> > &cis) {

   // two functions doing the same thing
   bool apply_colour_to_non_carbon_atoms_also = true;
   set_user_defined_atom_colour_by_selection(cis, apply_colour_to_non_carbon_atoms_also);
}


void
molecule_class_info_t::set_user_defined_colour_indices(const std::vector<std::pair<coot::atom_spec_t, int> > &cis) {

   if (atom_sel.mol) {
      int udd_handle = atom_sel.mol->GetUDDHandle(mmdb::UDR_ATOM, "user-defined-atom-colour-index");

      if (udd_handle == 0)
	 udd_handle = atom_sel.mol->RegisterUDInteger(mmdb::UDR_ATOM, "user-defined-atom-colour-index");

      for (unsigned int i=0; i<cis.size(); i++) {
	 const coot::atom_spec_t &spec = cis[i].first;
	 mmdb::Atom *at = get_atom(spec);
	 if (at) {
	    int ierr = at->PutUDData(udd_handle, cis[i].second);
	    if (ierr != mmdb::UDDATA_Ok) {
	       std::cout << "ERROR:: problem setting udd on atom " << coot::atom_spec_t(at) << std::endl;
	    }
	 } else {
            std::cout << "WARNING:: in set_user_defined_colour_indices() failed to get atom "
                      << spec << std::endl;
         }
      }
   }
}

void
molecule_class_info_t::user_defined_colours_representation(coot::protein_geometry *geom_p,
							   bool all_atoms_mode,
                                                           bool draw_missing_loops_flag) {

   bonds_box.clear_up();
   if (all_atoms_mode) {

      Bond_lines_container bonds(atom_sel, imol_no, geom_p, Bond_lines_container::COLOUR_BY_USER_DEFINED_COLOURS);
      bonds_box = bonds.make_graphical_bonds_no_thinning();
      bonds_box_type = coot::COLOUR_BY_USER_DEFINED_COLOURS____BONDS;

   } else {

      // CA mode then

      // std::cout << "::::::::::::::::::::::::::::: in user_defined_colours_representation() Path B " << std::endl;
      std::set<int> no_bonds_to_these_atom_indices;
      Bond_lines_container bonds(geom_p, "dummy-CA-mode", no_bonds_to_these_atom_indices, false);
      bonds.do_Ca_plus_ligands_bonds(atom_sel, imol_no, geom_p, 2.4, 4.7,
                                     draw_missing_loops_flag,
                                     coot::COLOUR_BY_USER_DEFINED_COLOURS, false);
      bool add_residue_indices = true;
      bonds_box = bonds.make_graphical_bonds_no_thinning();
      bonds_box_type = coot::COLOUR_BY_USER_DEFINED_COLOURS_CA_BONDS;
   }
   graphics_info_t::attach_buffers();
   // make_mesh_from_bonds_box();
   make_meshes_from_bonds_box_instanced_version();

}


void
molecule_class_info_t::clear_user_defined_atom_colours() {

   if (atom_sel.mol) {
      int udd_handle = atom_sel.mol->GetUDDHandle(mmdb::UDR_ATOM, "user-defined-atom-colour-index");
      if (udd_handle != 0) {
	 udd_handle = 0; // reset
      }
   }
}

// This is used in getting atom specs from a bonds box. Not a sensible thing to do
//
// instead the bonds box should contain atom specs
//
mmdb::Atom *
molecule_class_info_t::get_atom_at_pos(const coot::Cartesian &pt) const {

   mmdb::Atom *at = NULL;

   int imod = 1;
   mmdb::Model *model_p = atom_sel.mol->GetModel(imod);
   if (model_p) {
      int n_chains = model_p->GetNumberOfChains();
      for (int ichain=0; ichain<n_chains; ichain++) {
	 mmdb::Chain *chain_p = model_p->GetChain(ichain);
	 int nres = chain_p->GetNumberOfResidues();
	 for (int ires=0; ires<nres; ires++) {
	    mmdb::Residue *residue_p = chain_p->GetResidue(ires);
	    int n_atoms = residue_p->GetNumberOfAtoms();
	    for (int iat=0; iat<n_atoms; iat++) {
	       mmdb::Atom *atl = residue_p->GetAtom(iat);
	       coot::Cartesian at_pos(atl->x, atl->y, atl->z);
	       at_pos -= pt;
	       float d = at_pos.amplitude_squared();
	       if (d < 0.001) {
		  at = atl;
		  break;
	       }
	    }
	    if (at) break;
	 }
	 if (at) break;
      }
   }

   return at;


}
