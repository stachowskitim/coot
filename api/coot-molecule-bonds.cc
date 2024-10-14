/* src/coot-molecule-bonds.cc
 *
 * Copyright 2002, 2003, 2004, 2005, 2006, 2007 by The University of York
 * Copyright 2007 by Paul Emsley
 * Copyright 2007, 2008, 2009 by The University of Oxford
 * Copyright 2013, 2014, 2015, 2016 by Medical Research Council
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */


#include <iostream>
#include <iomanip>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
// #include <glm/ext.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>

#include "compat/coot-sysdep.h"

#include "coot-utils/prideout-octasphere.hh"  // needed?
#include "coot-utils/oct.hh"
#include "coot-utils/cylinder.hh"
#include "coot-molecule.hh"

// #include "molecular-mesh-generator.hh"

//! set the colour wheel rotation base for the specified molecule
void
coot::molecule_t::set_colour_wheel_rotation_base(float r) {
   bonds_colour_map_rotation = r;
}

//! set the base colour - to be used as a base for colour wheel rotation
void
coot::molecule_t::set_base_colour_for_bonds(float r, float g, float b) {

   base_colour_for_bonds = colour_holder(r,g,b);

}

//! user-defined colour-index to colour
void
coot::molecule_t::set_user_defined_bond_colours(const std::map<unsigned int, std::array<float, 4> > &colour_map) {

   // Fill user_defined_bond_colours

   std::map<unsigned int, std::array<float, 4> >::const_iterator it;
   for (it=colour_map.begin(); it!=colour_map.end(); ++it) {
      unsigned int idx = it->first;
      const auto &col = it->second;
      colour_holder ch(col[0], col[1], col[2], col[3]);
      // std::cout << "   " << idx << " " << ch << std::endl;
      user_defined_bond_colours[idx] = ch;
   }
}

//! user-defined atom selection to colour index
// This function should be called `set_user_defined_atom_colour_by_selections`
void
coot::molecule_t::set_user_defined_atom_colour_by_selections(const std::vector<std::pair<std::string, unsigned int> > &indexed_residues_cids,
                                                             bool colour_applies_to_non_carbon_atoms_also,
                                                             mmdb::Manager *mol) {

   store_user_defined_atom_colour_selections(indexed_residues_cids, colour_applies_to_non_carbon_atoms_also);
   apply_user_defined_atom_colour_selections(indexed_residues_cids, colour_applies_to_non_carbon_atoms_also, mol);

}

void
coot::molecule_t::store_user_defined_atom_colour_selections(const std::vector<std::pair<std::string, unsigned int> > &indexed_residues_cids_in,
                                                            bool colour_applies_to_non_carbon_atoms_also_in) {

   indexed_user_defined_colour_selection_cids = indexed_residues_cids_in;
   indexed_user_defined_colour_selection_cids_apply_to_non_carbon_atoms_also = colour_applies_to_non_carbon_atoms_also_in;

}

// why is this passed a mol? If it should be passed a mol, this function should be marked as static.
// Ah.. I note that I say "// mol in the following argument need not be this mol" in the calling function.
void
coot::molecule_t::apply_user_defined_atom_colour_selections(const std::vector<std::pair<std::string, unsigned int> > &indexed_residues_cids,
                                                            bool colour_applies_to_non_carbon_atoms_also,
                                                            mmdb::Manager *mol) {

   auto reset_udd_handle_values = [] (mmdb::Manager *mol, int udd_handle) {
      for(int imod = 1; imod<=mol->GetNumberOfModels(); imod++) {
         mmdb::Model *model_p = mol->GetModel(imod);
         if (model_p) {
            int n_chains = model_p->GetNumberOfChains();
            for (int ichain=0; ichain<n_chains; ichain++) {
               mmdb::Chain *chain_p = model_p->GetChain(ichain);
               int n_res = chain_p->GetNumberOfResidues();
               for (int ires=0; ires<n_res; ires++) {
                  mmdb::Residue *residue_p = chain_p->GetResidue(ires);
                  if (residue_p) {
                     int n_atoms = residue_p->GetNumberOfAtoms();
                     for (int iat=0; iat<n_atoms; iat++) {
                        mmdb::Atom *at = residue_p->GetAtom(iat);
                        int idx;
                        int ierr = at->GetUDData(udd_handle, idx);
                        // If it wasn't there already, there's no need to reset it.
                        if (ierr == mmdb::UDDATA_Ok) {
                           at->PutUDData(udd_handle, -1);
                        }
                     }
                  }
               }
            }
         }
      }
   };

   if (! is_valid_model_molecule()) return;

   int udd_handle = mol->GetUDDHandle(mmdb::UDR_ATOM, "user-defined-atom-colour-index");
   if (udd_handle == 0)
      udd_handle = mol->RegisterUDInteger(mmdb::UDR_ATOM, "user-defined-atom-colour-index");

    // this should happen if the indexed_residues_cids is empty.
   reset_udd_handle_values(mol, udd_handle);

   if (indexed_residues_cids.empty()) return;

   for (unsigned int i=0; i<indexed_residues_cids.size(); i++) {
      const auto &rc = indexed_residues_cids[i];
      const std::string &multi_cid = rc.first;

      std::vector<std::string> v = coot::util::split_string(multi_cid, "||");
      if (! v.empty()) {
         for (const auto &cid : v) {

            int colour_index = rc.second; // change type
            int selHnd = mol->NewSelection(); // d

            mmdb::Atom **SelAtoms = nullptr;
            int nSelAtoms = 0;
            mol->Select(selHnd, mmdb::STYPE_ATOM, cid.c_str(), mmdb::SKEY_NEW);
            mol->GetSelIndex(selHnd, SelAtoms, nSelAtoms);
            if (nSelAtoms > 0) {
               for(int iat=0; iat<nSelAtoms; iat++) {
                  mmdb:: Atom *at = SelAtoms[iat];
                  std::string element(at->element);
                  if (element == " C" || colour_applies_to_non_carbon_atoms_also) {
                     int ierr = at->PutUDData(udd_handle, colour_index);
                     if (ierr != mmdb::UDDATA_Ok) {
                        std::cout << "WARNING:: in set_user_defined_atom_colour_by_residue() problem setting udd on atom "
                                  << coot::atom_spec_t(at) << std::endl;
                     } else {
                        if (false)
                           std::cout << "debug:: set_user_defined_atom_colour_by_residue() sets user-define atom colour index "
                                     << "of atom " << coot::atom_spec_t(at) << "to " << colour_index << std::endl;
                     }
                  }
               }
            }
            mol->DeleteSelection(selHnd);
         }
      }
   }
}



void
coot::molecule_t::add_to_non_drawn_bonds(const std::string &atom_selection_cid) {

   // std::cout << "******* add_to_non_drawn_bonds() called with atom_selection_cid " << atom_selection_cid << std::endl;

   // add_to_non_drawn_bonds() works at the residue level, either all of the atoms of a residue
   // are drawn or all of them are not.
   // atom_selection_cid can specify an atom. In that case, all of the atoms of that residue should
   // not be drawn.

   if (atom_sel.mol) {
      int atom_index_udd_handle = atom_sel.UDDAtomIndexHandle;
      std::set<mmdb::Residue *> selected_residues;
      std::vector<std::string> v = coot::util::split_string(atom_selection_cid, "||");
      if (! v.empty()) {
         for (const auto &cid : v) {
            int selHnd = atom_sel.mol->NewSelection(); // d
            mmdb::Atom **SelAtoms = nullptr;
            int nSelAtoms = 0;
            atom_sel.mol->Select(selHnd, mmdb::STYPE_ATOM, cid.c_str(), mmdb::SKEY_NEW);
            atom_sel.mol->GetSelIndex(selHnd, SelAtoms, nSelAtoms);
            if (nSelAtoms > 0) {
               for(int iat=0; iat<nSelAtoms; iat++) {
                  mmdb:: Atom *at = SelAtoms[iat];
                  selected_residues.insert(at->residue);
               }
            }
            atom_sel.mol->DeleteSelection(selHnd);
         }
      }

      std::set<mmdb::Residue *>::const_iterator it;
      for (it=selected_residues.begin(); it!=selected_residues.end(); ++it) {
         mmdb::Residue *residue_p = *it;
         mmdb::Atom **residue_atoms = nullptr;
         int n_residue_atoms = 0;
         residue_p->GetAtomTable(residue_atoms, n_residue_atoms);
         for (int iat=0; iat<n_residue_atoms; iat++) {
            mmdb::Atom *at = residue_atoms[iat];
            if (! at->isTer()) {
               int idx;
               at->GetUDData(atom_index_udd_handle, idx);
               no_bonds_to_these_atom_indices.insert(idx);
               if (false)
                  std::cout << "No-bonds-to-these-atoms " << coot::atom_spec_t(at) << "  " << idx << std::endl;
            }
         }
      }

   }
   if (false)
      std::cout << "add_to_non_drawn_bonds() no_bonds_to_these_atom_indices now has size "
                << no_bonds_to_these_atom_indices.size() << std::endl;
}

void
coot::molecule_t::print_non_drawn_bonds() const {

   std::set<int>::const_iterator it;
   std::cout << "----------- no bonds to these atoms table: " << std::endl;
   for (it=no_bonds_to_these_atom_indices.begin(); it!=no_bonds_to_these_atom_indices.end(); ++it) {
      int i = *it;
      if (i >= 0) {
         if (i < atom_sel.n_selected_atoms) {
            mmdb:: Atom *at = atom_sel.atom_selection[i];
            std::cout << "  " << i << "  " << coot::atom_spec_t(at) << std::endl;
         } else {
            std::cout << "ERROR:: atom index " << i << " out of range" << std::endl;
         }
      }
   }
}



// public - because currently making bonds is not done on molecule construction
void
coot::molecule_t::make_bonds(coot::protein_geometry *geom, coot::rotamer_probability_tables *rot_prob_tables_p,
                             bool draw_hydrogen_atoms_flag, bool draw_missing_loops_flag) {

   bonds_box_type = api_bond_colour_t::COLOUR_BY_CHAIN_BONDS;
   make_bonds_type_checked(geom, rot_prob_tables_p, draw_hydrogen_atoms_flag, draw_missing_loops_flag);

   // std::cout << "debug:: in molecule_t::make_bonds() " << bonds_box.n_bonds() << " bonds " << bonds_box.n_atoms() << " atoms "
   // << std::endl;
}



// private
void
coot::molecule_t::makebonds(coot::protein_geometry *geom, coot::rotamer_probability_tables *rotamer_tables_p,
                            const std::set<int> &no_bonds_to_these_atoms,
                            bool draw_hydrogen_atoms_flag, bool draw_missing_loops_flag) {

   bool force_rebond = true;
   bool do_rotamer_markup = true; // pass this
   make_colour_by_chain_bonds(geom, no_bonds_to_these_atoms, true, false, draw_hydrogen_atoms_flag, draw_missing_loops_flag, do_rotamer_markup, rotamer_tables_p, force_rebond);

}


void
coot::molecule_t::make_colour_by_chain_bonds(coot::protein_geometry *geom,
                                             const std::set<int> &no_bonds_to_these_atoms,
                                             bool change_c_only_flag,
                                             bool goodsell_mode,
                                             bool draw_hydrogen_atoms_flag,
                                             bool draw_missing_loops_flag,
                                             bool do_rota_markup,
                                             coot::rotamer_probability_tables *tables_p,
                                             bool force_rebonding) {

   // We don't want to rebond if we don't have to (i.e the mode requested is the current mode)
   // so check the previous value of bonds_box_type so that we can know if it can be skipped.

   bool draw_hydrogens_flag = draw_hydrogen_atoms_flag;

   Bond_lines_container bonds(geom, no_bonds_to_these_atoms, draw_hydrogens_flag);

   bonds.add_rotamer_tables(tables_p);
   bonds.do_colour_by_chain_bonds(atom_sel, false, imol_no, draw_hydrogens_flag,
                                  draw_missing_loops_flag,
                                  change_c_only_flag, goodsell_mode, do_rota_markup);

   // std::cout << "------------------- calling make_graphical_bonds_no_thinning() " << std::endl;

   // 20221030-PE Hmmm..
   // bonds_box = bonds.make_graphical_bonds_no_thinning(); // make_graphical_bonds() is pretty
                                                            // stupid when it comes to thining.

   // 20221030-PE I think that I actually want thinning. This needs more thought, I think.

   bonds_box = bonds.make_graphical_bonds(); // make_graphical_bonds() is pretty
                                             // stupid when it comes to thining.

   // std::cout << "::::::: in make_colour_by_chain_bonds() bonds_box n_rotamer_markups "
   // << bonds_box.n_rotamer_markups << " :::::::::" << std::endl;

   // testing previous values of bonds_box_type
   if (bonds_box_type != api_bond_colour_t::COLOUR_BY_CHAIN_BONDS)
      force_rebonding = true;

   if (goodsell_mode)
      if (bonds_box_type != api_bond_colour_t::COLOUR_BY_CHAIN_GOODSELL)
         force_rebonding = true;

   bonds_box_type = api_bond_colour_t::COLOUR_BY_CHAIN_BONDS;

   if (goodsell_mode)
      bonds_box_type = api_bond_colour_t::COLOUR_BY_CHAIN_GOODSELL;

   // 20221011-PE Hmm... is this needed in this API? I don't think so
   //
   // if (force_rebonding)
   //    make_glsl_bonds_type_checked(__FUNCTION__);

}

void
coot::molecule_t::make_bonds_type_checked(coot::protein_geometry *geom_p,
                                          coot::rotamer_probability_tables *rotamer_probability_tables_p,
                                          bool draw_hydrogen_atoms_flag,
                                          bool draw_missing_loops_flag,
                                          const char *caller) {

   bool rotate_colour_map_on_read_pdb_c_only_flag = true; // pass this or make class data item

   bool debug = false;

   // Note caller can be 0 (e.g. with clang) - so be aware of that when debugging.

   std::string caller_s("NULL");
   if (caller) caller_s = std::string(caller);

   bool is_intermediate_atoms_molecule = false; // 20221005-PE IMPORT-HACK
   if (debug)
      std::cout << "debug:: plain make_bonds_type_checked() --------start--------- called by "
                << caller_s << "() with is_intermediate_atoms_molecule: " << is_intermediate_atoms_molecule
                << std::endl;
#if 0
   // this no longer compiles now that api_bond_colour_t is a class
   if (debug)
      std::cout << "--------- make_bonds_type_checked() called with bonds_box_type "
                << bonds_box_type
                << " vs "
                << "NORMAL_BONDS " << coot::api_bond_colour_t::NORMAL_BONDS << " "
                << "BONDS_NO_HYDROGENS " << coot::api_bond_colour_t::BONDS_NO_HYDROGENS << " "
                << "COLOUR_BY_CHAIN_BONDS " << coot::api_bond_colour_t::COLOUR_BY_CHAIN_BONDS << " "
                << "COLOUR_BY_MOLECULE_BONDS " << coot::api_bond_colour_t::COLOUR_BY_MOLECULE_BONDS << " "
                << "CA_BONDS " << coot::api_bond_colour_t::CA_BONDS << " "
                << "CA_BONDS_PLUS_LIGANDS " << coot::api_bond_colour_t::CA_BONDS_PLUS_LIGANDS << " "
                << "COLOUR_BY_USER_DEFINED_COLOURS___BONDS " << coot::api_bond_colour_t::COLOUR_BY_USER_DEFINED_COLOURS____BONDS << " "
                << std::endl;
#endif

   // Delete this in due course
   // graphics_info_t g; // urgh!  (But the best solution?)

   bool force_rebonding = true; // if we get here, this must be true (?)

   // coot::protein_geometry *geom_p = g.Geom_p();

   std::set<int> &no_bonds_to_these_atoms = no_bonds_to_these_atom_indices;

   if (bonds_box_type == coot::api_bond_colour_t::NORMAL_BONDS)
      makebonds(geom_p, nullptr, no_bonds_to_these_atoms, draw_hydrogen_atoms_flag, draw_missing_loops_flag);

   if (bonds_box_type == coot::api_bond_colour_t::BONDS_NO_HYDROGENS)
      makebonds(geom_p, nullptr, no_bonds_to_these_atoms, draw_hydrogen_atoms_flag, draw_missing_loops_flag);
   // make_ca_bonds() hasn't been written yet.
   // if (bonds_box_type == coot::CA_BONDS)
   //    make_ca_bonds();
   if (bonds_box_type == coot::api_bond_colour_t::COLOUR_BY_CHAIN_BONDS || bonds_box_type == coot::api_bond_colour_t::COLOUR_BY_CHAIN_GOODSELL) {
      std::set<int> s;
      bool goodsell_mode = false;
      if (bonds_box_type == coot::api_bond_colour_t::COLOUR_BY_CHAIN_GOODSELL)
         goodsell_mode = true;
      bool do_rota_markup = true;

      make_colour_by_chain_bonds(geom_p, s, rotate_colour_map_on_read_pdb_c_only_flag, goodsell_mode, do_rota_markup, rotamer_probability_tables_p, force_rebonding);
   }

#if 0 // not implemenented yet
   if (bonds_box_type == coot::COLOUR_BY_MOLECULE_BONDS)
      make_colour_by_molecule_bonds(force_rebonding);
   if (bonds_box_type == coot::CA_BONDS_PLUS_LIGANDS)
      make_ca_plus_ligands_bonds(g.Geom_p());
   if (bonds_box_type == coot::CA_BONDS_PLUS_LIGANDS_AND_SIDECHAINS)
      make_ca_plus_ligands_and_sidechains_bonds(g.Geom_p());
   if (bonds_box_type == coot::BONDS_NO_WATERS)
      bonds_no_waters_representation();
   if (bonds_box_type == coot::BONDS_SEC_STRUCT_COLOUR)
      bonds_sec_struct_representation();
   if (bonds_box_type == coot::CA_BONDS_PLUS_LIGANDS_SEC_STRUCT_COLOUR)
      ca_plus_ligands_sec_struct_representation(g.Geom_p());
   if (bonds_box_type == coot::COLOUR_BY_RAINBOW_BONDS)
      ca_plus_ligands_rainbow_representation(g.Geom_p());
   if (bonds_box_type == coot::COLOUR_BY_OCCUPANCY_BONDS)
      occupancy_representation();
   if (bonds_box_type == coot::COLOUR_BY_B_FACTOR_BONDS)
      b_factor_representation();
   if (bonds_box_type == coot::CA_BONDS_PLUS_LIGANDS_B_FACTOR_COLOUR)
      b_factor_representation_as_cas();
   if (bonds_box_type == coot::COLOUR_BY_USER_DEFINED_COLOURS____BONDS)
      user_defined_colours_representation(g.Geom_p(), true, g.draw_missing_loops_flag); // hack,
                                                             // because we need to remeber somehow
                                                             // if this was called with all-atom or CA-only.
                                                             // See c-interface.cc
                                                             // graphics_to_user_defined_atom_colours_representation()
                                                             // Perhaps we need two functions
                                                             // user_defined_colours_representation_all()
                                                             // user_defined_colours_representation_Calpha() [+ ligands]
   if (bonds_box_type == coot::COLOUR_BY_USER_DEFINED_COLOURS_CA_BONDS)
      user_defined_colours_representation(geom_p, false, draw_missing_loops_flag); // hack,

#endif


#if 0 // 20221005-PE not sure what these are
   // all these will need to be changed or removed
   update_additional_representations(glci, g.Geom_p());
   update_fixed_atom_positions();
   update_ghosts();
   update_extra_restraints_representation();
#endif

   if (debug) {
      std::cout << "debug:: -------------- make_bonds_type_checked() done " << std::endl;
   }
}

// You might consider making this part of simple_mesh_t - that would mirror how the Mesh class works
void
make_graphical_bonds_spherical_atoms(coot::simple_mesh_t &m, // fill this
                                     const graphical_bonds_container &gbc,
                                     coot::api_bond_colour_t bonds_box_type,
                                     int udd_handle_bonded_type,
                                     float atom_radius,
                                     float bond_radius,
                                     unsigned int num_subdivisions,
                                     const std::vector<glm::vec4> &colour_table) {

   // udd_handle_bonded_type can be NO_BOND, BONDED_WITH_STANDARD_ATOM_BOND, BONDED_WITH_BOND_TO_HYDROGEN
   // BONDED_WITH_HETATM_BOND.

   auto cartesian_to_glm = [] (const coot::Cartesian &co) {
                            return glm::vec3(co.x(), co.y(), co.z());
                         };

   bool atoms_have_bigger_radius_than_bonds = false;
   if (atom_radius > bond_radius)
      atoms_have_bigger_radius_than_bonds = true;

   // ----------------------- setup the vertices and triangles ----------------------

   std::pair<std::vector<glm::vec3>, std::vector<g_triangle> > octasphere_geom =
      tessellate_octasphere(num_subdivisions);

   int colour_table_size = colour_table.size(); // change type
   if (colour_table_size < gbc.n_consolidated_atom_centres) {
      std::cout << "ERROR:: colour_table too small " << colour_table.size() << " vs " << gbc.n_consolidated_atom_centres << std::endl;
      return;
   }

   glm::mat4 unit(1.0);
   for (int icol=0; icol<gbc.n_consolidated_atom_centres; icol++) {
      glm::vec4 col = colour_table[icol];
      for (unsigned int i=0; i<gbc.consolidated_atom_centres[icol].num_points; i++) {
         const graphical_bonds_atom_info_t &at_info = gbc.consolidated_atom_centres[icol].points[i];
         bool do_it = atoms_have_bigger_radius_than_bonds;
         mmdb::Atom *at = at_info.atom_p;

         if (! do_it) {
            if (at) {
               int state = -1;
               at->GetUDData(udd_handle_bonded_type, state);
               if (state == graphical_bonds_container::NO_BOND) {
                  do_it = true;
                  // std::cout << "setting do_it to true because NO_BOND" << std::endl;
               } else {
                  if (false)
                     std::cout << "in make_graphical_bonds_spherical_atoms() atom " << coot::atom_spec_t(at)
                               << " state " << state << " vs NO_BOND " << graphical_bonds_container::NO_BOND
                               << std::endl;
               }
            }
         }

         // std::cout << "make_graphical_bonds_spherical_atoms() for atom " << coot::atom_spec_t(at) << " do-it: " << do_it << std::endl;

         if (do_it) {
            unsigned int idx_base = m.vertices.size();
            unsigned int idx_tri_base = m.triangles.size();
            float scale = 1.0;

            if (at_info.is_hydrogen_atom) {
               if (atoms_have_bigger_radius_than_bonds) {
                  scale *= 0.66;
               } else {
                  scale *= 0.5; // bonds go to half-width, so should atoms.
               }
            }
            glm::vec3 t = cartesian_to_glm(at_info.position);  // (at->x, at->y, at->z);
            float sar = scale * atom_radius * at_info.radius_scale;
            if (at_info.is_water) {
               if (atoms_have_bigger_radius_than_bonds) {
                  float f = 1.33; // with a radius_scale of 2.0 waters are too chonky
                  sar = scale * atom_radius * f;
               }
            }
            glm::vec3 sc(sar, sar, sar);
            glm::mat4 mm = glm::scale(unit, sc);
            mm = glm::translate(mm, t);

            std::vector<coot::api::vnc_vertex> local_vertices(octasphere_geom.first.size());

            for (unsigned int ii=0; ii<local_vertices.size(); ii++) {
               auto &vert = local_vertices[ii];
               glm::vec3 p = octasphere_geom.first[ii] * sc + t;
               vert = coot::api::vnc_vertex(p, octasphere_geom.first[ii], col);
            }
            // 20230110-PE class no longer has a colour index
            // for (auto &tri : octasphere_geom.second)
            // tri.colour_index = icol;

            m.vertices.insert(m.vertices.end(), local_vertices.begin(), local_vertices.end());
            m.triangles.insert(m.triangles.end(), octasphere_geom.second.begin(), octasphere_geom.second.end());
            for (unsigned int k=idx_tri_base; k<m.triangles.size(); k++)
               m.triangles[k].rebase(idx_base);
         }
      }
   }
}

// fill m
void make_graphical_bonds_spherical_atoms_with_vdw_radii(coot::simple_mesh_t &m, const graphical_bonds_container &gbc,
                                                         unsigned int num_subdivisions,
                                                         const std::vector<glm::vec4> &colour_table,
                                                         const coot::protein_geometry &geom) {

   std::pair<std::vector<glm::vec3>, std::vector<g_triangle> > octosphere_geom =
      tessellate_octasphere(num_subdivisions);

   std::map<std::string, float> ele_to_radius_map;
   glm::mat4 unit(1.0);
   for (int icol=0; icol<gbc.n_consolidated_atom_centres; icol++) {
      glm::vec4 col = colour_table[icol];
      for (unsigned int i=0; i<gbc.consolidated_atom_centres[icol].num_points; i++) {
         const graphical_bonds_atom_info_t &at_info = gbc.consolidated_atom_centres[icol].points[i];
         mmdb::Atom *at = at_info.atom_p;
         std::string ele(at->element);
         std::map<std::string, float>::const_iterator it = ele_to_radius_map.find(ele);
         float atom_radius = 1.0;
         if (it != ele_to_radius_map.end()) {
            atom_radius = it->second;
         } else {
            std::string atom_name(at->GetAtomName());
            std::string residue_name(at->GetResName());
            atom_radius = geom.get_vdw_radius(atom_name, residue_name, coot::protein_geometry::IMOL_ENC_ANY, false);
            ele_to_radius_map[ele] = atom_radius;
         }

         glm::vec3 t(at->x, at->y, at->z);
         glm::vec3 sc(atom_radius, atom_radius, atom_radius);

         std::vector<coot::api::vnc_vertex> local_vertices(octosphere_geom.first.size());

         for (unsigned int ii=0; ii<local_vertices.size(); ii++) {
            auto &vert = local_vertices[ii];
            glm::vec3 p = octosphere_geom.first[ii] * sc + t;
            vert = coot::api::vnc_vertex(p, octosphere_geom.first[ii], col);
         }
         // 20230110-PE class no longer has a colour index
         // for (auto &tri : octasphere_geom.second)
         // tri.colour_index = icol;

         unsigned int idx_base = m.vertices.size();
         unsigned int idx_tri_base = m.triangles.size();
         m.vertices.insert(m.vertices.end(), local_vertices.begin(), local_vertices.end());
         m.triangles.insert(m.triangles.end(), octosphere_geom.second.begin(), octosphere_geom.second.end());
         for (unsigned int k=idx_tri_base; k<m.triangles.size(); k++)
            m.triangles[k].rebase(idx_base);
      }
   }

   std::cout << "DEBUG:: make_graphical_bonds_spherical_atoms_with_vdw_radii() " << m.vertices.size() << " " << m.triangles.size() << std::endl;
}


void
make_graphical_bonds_hemispherical_atoms(coot::simple_mesh_t &m, // fill m
                                         const graphical_bonds_container &gbc,
                                         coot::api_bond_colour_t bonds_box_type,
                                         int udd_handle_bonded_type,
                                         float atom_radius,
                                         float bond_radius,
                                         unsigned int num_subdivisions,
                                         const std::vector<glm::vec4> &colour_table) {


   // BONDED_WITH_HETATM_BOND.

   // do these need to be passed to get_glm_colour_for_bonds()?

   bool atoms_have_bigger_radius_than_bonds = false;
   if (atom_radius > bond_radius)
      atoms_have_bigger_radius_than_bonds = true;

   // like above, different z axis because we want the hemisphere to extend outside the cylinder - and we don't need to
   // scale to bond length
   auto get_octahemi_matrix = [] (const glm::vec3 &pos_1, const glm::vec3 &pos_2, float radius) {
                                 glm::vec3 delta = pos_2 - pos_1;
                                 glm::mat4 u(1.0f);
                                 // orient
                                 glm::vec3 normalized_bond_orientation(glm::normalize(delta));
                                 glm::mat4 ori = glm::orientation(normalized_bond_orientation, glm::vec3(0.0, 0.0, -1.0));

                                 // 20220204-PE no translation because I want to use this matrix for the vertex normals
                                 // translate
                                 // glm::mat4 t = glm::translate(u, pos_1);
                                 // glm::mat4 m = t * ori * sc;
                                 return ori;
                              };

   auto cartesian_to_glm = [] (const coot::Cartesian &co) {
                            return glm::vec3(co.x(), co.y(), co.z());
                         };

   // ----------------------- setup the vertices and triangles ----------------------

   std::pair<std::vector<glm::vec3>, std::vector<g_triangle> > octasphere_geom =
      tessellate_hemisphere_patch(num_subdivisions);

   // ----------------------- setup the instances ----------------------

   int colour_table_size = colour_table.size(); // change type
   if (colour_table_size < gbc.n_consolidated_atom_centres) {
      std::cout << "ERROR:: colour_table too small " << colour_table.size() << " vs " << gbc.n_consolidated_atom_centres << std::endl;
      return;
   }

   // what is the other atom of the bond to this atom (if it has one). Just find the first one
   // if there are many
   //
   // this is hideous. And will be slow on a big molecule. Replace it.
   // When the bonds are calculated, the at_info should store
   // a pointer to a bonded atom (mmdb::Atom *bonded_atom_other_atom)
   //
   std::map<int, int> bonded_atom_other_atom;
   std::map<int, mmdb::Atom *> index_to_atom;
   for (int icol_bond=0; icol_bond<gbc.num_colours; icol_bond++) {
      graphical_bonds_lines_list<graphics_line_t> &ll = gbc.bonds_[icol_bond];
      for (int j=0; j<ll.num_lines; j++) {
         int idx_1 = ll.pair_list[j].atom_index_1;
         int idx_2 = ll.pair_list[j].atom_index_2;
         std::map<int, int>::const_iterator it = bonded_atom_other_atom.find(idx_1);
         if (it == bonded_atom_other_atom.end()) {
            bonded_atom_other_atom[idx_1] = idx_2;
         } else {
            // std::cout << "found other atom bond " << idx_1 << " " << it->first << " " << it->second << std::endl;
         }
         it = bonded_atom_other_atom.find(idx_2);
         if (it == bonded_atom_other_atom.end()) {
            bonded_atom_other_atom[idx_2] = idx_1;
         } else {
            //std::cout << "found other atom bond " << idx_2 << " " << it->first << " " << it->second << std::endl;
         }
      }
   }
   for (int icol=0; icol<gbc.n_consolidated_atom_centres; icol++) {
      for (unsigned int i=0; i<gbc.consolidated_atom_centres[icol].num_points; i++) {
         const graphical_bonds_atom_info_t &at_info = gbc.consolidated_atom_centres[icol].points[i];
         index_to_atom[at_info.atom_index] = at_info.atom_p;
      }
   }

   if (false) {
      for (const auto &item : index_to_atom) {
         std::cout << "   index_to_atom: " << item.first << " " << item.second << " " << coot::atom_spec_t(item.second)
                   << std::endl;
      }
   }

   glm::mat4 unit(1.0);
   for (int icol=0; icol<gbc.n_consolidated_atom_centres; icol++) {
      glm::vec4 col = colour_table[icol];
      for (unsigned int i=0; i<gbc.consolidated_atom_centres[icol].num_points; i++) {
         const graphical_bonds_atom_info_t &at_info = gbc.consolidated_atom_centres[icol].points[i];
         bool do_it = true;
         if (atoms_have_bigger_radius_than_bonds) do_it = false; // not as hemispheres, that is
         mmdb::Atom *at = at_info.atom_p;

         if (do_it) {
            if (at) {
               int state = -1;
               at->GetUDData(udd_handle_bonded_type, state);
               if (state == graphical_bonds_container::NO_BOND) {
                  do_it = false;
               }
            }
         }

         // Oh dear! I need to know where the atom at the other end of the bond is!
         // Which bond? Any bond. The first one. We need to do some pre-processing to know that.
         // That's done now.

         // There is an opportunity for missing atoms here. Things that are not NO_BOND but have not correctly
         // set the atom_index

         if (do_it) {

            unsigned int idx_base = m.vertices.size();
            unsigned int idx_tri_base = m.triangles.size();

            float scale = 1.0;
            if (at_info.is_hydrogen_atom) scale *= 0.5;
            glm::vec3 t = cartesian_to_glm(at_info.position); // (at->x, at->y, at->z);
            float sar = scale * atom_radius;
            glm::vec3 sc(sar, sar, sar);

            int atom_index = at_info.atom_index;
            std::map<int, int>::const_iterator it = bonded_atom_other_atom.find(atom_index);
            if (it != bonded_atom_other_atom.end()) {
               // check the index before using it?
               int other_atom_index = it->second;
               if (false)
                  std::cout << "here with other_atom_index " << other_atom_index << " with index_to_atom size "
                            << index_to_atom.size() << std::endl;
               mmdb::Atom *other_at = index_to_atom[other_atom_index];
               if (other_at) {
                  // std::cout << "   other_at " << other_at << " " << coot::atom_spec_t(other_at) << std::endl;
                  glm::vec3 other_atom_pos(other_at->x, other_at->y, other_at->z);
                  glm::mat4 mm = get_octahemi_matrix(t, other_atom_pos, bond_radius); // a rotation matrix

                  std::vector<coot::api::vnc_vertex> local_vertices(octasphere_geom.first.size());

                  for (unsigned int ii=0; ii<local_vertices.size(); ii++) {
                     glm::vec3 p0(octasphere_geom.first[ii]);
                     glm::vec4 p1(p0, 1.0);
                     glm::vec4 p2 = mm * p1;
                     glm::vec3 p3(p2);
                     glm::vec3 p4 = p3 * sc;
                     glm::vec3 p5 = p4 + t;
                     auto &vert = local_vertices[ii];
                     vert = coot::api::vnc_vertex(p5, p3, col);
                     if (false)
                        std::cout << "make_graphical_bonds_hemispherical_atoms() vertex " << ii << " of "
                                  << local_vertices.size() << " " << coot::atom_spec_t(at)
                                  << " vertex position " << glm::to_string(p5) << std::endl;
                  }

                  // 20230110-PE class no longer has a colour index
                  // for (auto &tri : octasphere_geom.second) // for export to blender
                  // tri.colour_index = icol;

                  m.vertices.insert(m.vertices.end(), local_vertices.begin(), local_vertices.end());
                  m.triangles.insert(m.triangles.end(), octasphere_geom.second.begin(), octasphere_geom.second.end());
                  for (unsigned int k=idx_tri_base; k<m.triangles.size(); k++)
                     m.triangles[k].rebase(idx_base);
               } else {
                  std::cout << "WARNING:: make_graphical_bonds_hemispherical_atoms() other_at is null for atom_index "
                            << atom_index << " " << coot::atom_spec_t(at) << std::endl;
               }
            } else {
               // 20220220-PE Is there an error that causes us to get here? I think so, but I have other things to do so
               // inhibit this message for the moment.
               if (false)
                  std::cout << "DEBUG:: make_graphical_bonds_hemispherical_atoms failed to find other_atom for atom index "
                            << atom_index << " " << coot::atom_spec_t(at) << " in bonded_atom_other_atom map of size "
                            << bonded_atom_other_atom.size()  << std::endl;
            }
         }
      }
   }
}


std::pair<std::vector<coot::api::vnc_vertex>, std::vector<g_triangle> >
make_cis_peptide_geom(const std::vector<glm::vec3> &cis_pep_quad_in,
                      coot::util::cis_peptide_quad_info_t::type_t type) {

   // type: 9 unset
   // type: 1 cis
   // type: 2 pre-PRO cis
   // type: 3 twisted-trans

   const   std::vector<glm::vec3> &cis_pep_quad = cis_pep_quad_in;


   // find the normal
   glm::vec3 v1 = cis_pep_quad[1] - cis_pep_quad[0];
   glm::vec3 v2 = cis_pep_quad[2] - cis_pep_quad[3];

   glm::vec3 c = glm::cross(v1, v2);
   glm::vec3 normal_1 = glm::normalize(c); // the main normal

   std::vector<coot::api::vnc_vertex> vv;
   std::vector<g_triangle> vt;

   glm::vec3 CA_midpoint  = 0.5f * (cis_pep_quad[0] + cis_pep_quad[3]);
   glm::vec3 C_N_midpoint = 0.5f * (cis_pep_quad[1] + cis_pep_quad[2]);
   glm::vec3 C_N_midpoint_to_CA_midpoint = CA_midpoint - C_N_midpoint;
   glm::vec3 C_N_midpoint_to_CA_midpoint_uv = glm::normalize(C_N_midpoint_to_CA_midpoint);
   glm::vec3 ca_this_uv = glm::normalize(cis_pep_quad[0] - CA_midpoint);
   glm::vec3 ca_next_uv = glm::normalize(cis_pep_quad[3] - CA_midpoint);
   glm::vec3 C_to_CA_midpoint = CA_midpoint - cis_pep_quad[1];
   glm::vec3 N_to_CA_midpoint = CA_midpoint - cis_pep_quad[2];
   glm::vec3 C_to_CA_midpoint_uv = glm::normalize(C_to_CA_midpoint);
   glm::vec3 N_to_CA_midpoint_uv = glm::normalize(N_to_CA_midpoint);

   float cut_back_frac = 0.2; // was 0.05
   glm::vec3 C_N_midpoint_cutback = C_N_midpoint + cut_back_frac *  C_N_midpoint_to_CA_midpoint_uv;
   glm::vec3 CA_this_cutback = cis_pep_quad[0] - cut_back_frac * ca_this_uv;
   glm::vec3 CA_next_cutback = cis_pep_quad[3] - cut_back_frac * ca_next_uv;
   glm::vec3 C_this_cutback  = cis_pep_quad[1] + cut_back_frac * C_to_CA_midpoint_uv;
   glm::vec3 N_next_cutback  = cis_pep_quad[2] + cut_back_frac * N_to_CA_midpoint_uv;

   glm::vec3 d = 3.0f * 0.02f * normal_1;

   glm::vec4 col(0.7f, 0.3f, 0.3f, 1.0f);
   if (type == coot::util::cis_peptide_quad_info_t::PRE_PRO_CIS)
      col = glm::vec4(0.1f, 0.6f, 0.1f, 1.0f);
   if (type == coot::util::cis_peptide_quad_info_t::TWISTED_TRANS)
      col = glm::vec4(0.6f, 0.6f, 0.2f, 1.0f);

   glm::vec3 normal_1A = glm::normalize(glm::cross(cis_pep_quad[0]-cis_pep_quad[1], cis_pep_quad[0]-CA_midpoint));
   glm::vec3 normal_1B = glm::normalize(glm::cross(cis_pep_quad[3]-cis_pep_quad[2], cis_pep_quad[3]-CA_midpoint));

   glm::vec3 displacement = d;
   glm::vec3 n = normal_1;

   vv.push_back(coot::api::vnc_vertex(CA_midpoint+displacement, n, col));
   vv.push_back(coot::api::vnc_vertex(C_N_midpoint_cutback+displacement, n, col));
   vv.push_back(coot::api::vnc_vertex(CA_this_cutback + displacement, -normal_1A, col));
   vv.push_back(coot::api::vnc_vertex(CA_next_cutback + displacement,  normal_1B, col));
   vv.push_back(coot::api::vnc_vertex( C_this_cutback + displacement, -normal_1A, col));
   vv.push_back(coot::api::vnc_vertex( N_next_cutback + displacement,  normal_1B, col));

   displacement = -d;
   n = -normal_1;
   vv.push_back(coot::api::vnc_vertex(CA_midpoint+displacement, n, col));
   vv.push_back(coot::api::vnc_vertex(C_N_midpoint_cutback+displacement, n, col));
   vv.push_back(coot::api::vnc_vertex(CA_this_cutback + displacement,  normal_1A, col));
   vv.push_back(coot::api::vnc_vertex(CA_next_cutback + displacement, -normal_1B, col));
   vv.push_back(coot::api::vnc_vertex(C_this_cutback + displacement,   normal_1A, col));
   vv.push_back(coot::api::vnc_vertex(N_next_cutback + displacement,  -normal_1B, col));

   vt.push_back(g_triangle(2,0,4));
   vt.push_back(g_triangle(4,0,5));
   vt.push_back(g_triangle(0,3,5));

   vt.push_back(g_triangle(8,10,6));
   vt.push_back(g_triangle(10,11,6));
   vt.push_back(g_triangle(6,11,9));

   // now the faces of the box, noting that the faces have different vertices with the same position
   // because the normals are facing in different directions.

   n = glm::normalize(CA_midpoint - C_N_midpoint);
   unsigned int idx_base = vv.size();
   vv.push_back(coot::api::vnc_vertex(CA_this_cutback + d, n, col));
   vv.push_back(coot::api::vnc_vertex(CA_this_cutback - d, n, col));
   vv.push_back(coot::api::vnc_vertex(CA_next_cutback + d, n, col));
   vv.push_back(coot::api::vnc_vertex(CA_next_cutback - d, n, col));

   vt.push_back(g_triangle(idx_base, idx_base+1, idx_base+2));
   vt.push_back(g_triangle(idx_base+1, idx_base+3, idx_base+2));

   idx_base = vv.size();
   n = glm::normalize(0.5f * (CA_this_cutback + C_this_cutback) - CA_next_cutback);
   vv.push_back(coot::api::vnc_vertex(CA_this_cutback + d, n, col));
   vv.push_back(coot::api::vnc_vertex(CA_this_cutback - d, n, col));
   vv.push_back(coot::api::vnc_vertex(C_this_cutback + d, n, col));
   vv.push_back(coot::api::vnc_vertex(C_this_cutback - d, n, col));

   vt.push_back(g_triangle(idx_base, idx_base+1, idx_base+2));
   vt.push_back(g_triangle(idx_base+1, idx_base+3, idx_base+2));

   idx_base = vv.size();
   n = glm::normalize(C_N_midpoint - CA_midpoint);
   vv.push_back(coot::api::vnc_vertex(C_this_cutback + d, n, col));
   vv.push_back(coot::api::vnc_vertex(C_this_cutback - d, n, col));
   vv.push_back(coot::api::vnc_vertex(N_next_cutback + d, n, col));
   vv.push_back(coot::api::vnc_vertex(N_next_cutback - d, n, col));
   vt.push_back(g_triangle(idx_base, idx_base+1, idx_base+2));
   vt.push_back(g_triangle(idx_base+1, idx_base+3, idx_base+2));

   idx_base = vv.size();
   n = glm::normalize(0.5f * (CA_next_cutback + N_next_cutback) - CA_this_cutback);
   vv.push_back(coot::api::vnc_vertex(N_next_cutback + d, n, col));
   vv.push_back(coot::api::vnc_vertex(N_next_cutback - d, n, col));
   vv.push_back(coot::api::vnc_vertex(CA_next_cutback + d, n, col));
   vv.push_back(coot::api::vnc_vertex(CA_next_cutback - d, n, col));
   vt.push_back(g_triangle(idx_base, idx_base+1, idx_base+2));
   vt.push_back(g_triangle(idx_base+1, idx_base+3, idx_base+2));

   // std::move here? Or is this optimized?
   return std::pair<std::vector<coot::api::vnc_vertex>, std::vector<g_triangle> >(vv, vt);

}


void
make_graphical_bonds_cis_peptides(coot::simple_mesh_t &m,
                                  const graphical_bonds_container &gbc) {

   auto cartesian_to_glm = [] (const coot::Cartesian &c) {
                              return glm::vec3(c.x(), c.y(), c.z());
                           };


   auto &vertices  = m.vertices;
   auto &triangles = m.triangles;

   for (int i=0; i<gbc.n_cis_peptide_markups; i++) {
      const graphical_bonds_cis_peptide_markup &m = gbc.cis_peptide_markups[i];
      int single_model_view_current_model_number = 0; // 20220220-PE hack - this number comes from
                                                      // a graphics_info_t static, so I suppose
                                                      // that it should be passed here.
      if ((single_model_view_current_model_number == 0) ||
          (single_model_view_current_model_number == m.model_number)) {

         std::vector<glm::vec3> glm_quad(4);
         glm_quad[0] = cartesian_to_glm(m.pt_ca_1);
         glm_quad[1] = cartesian_to_glm(m.pt_c_1);
         glm_quad[2] = cartesian_to_glm(m.pt_n_2);
         glm_quad[3] = cartesian_to_glm(m.pt_ca_2);
         // molecular_mesh_generator_t mmg;
         coot::util::cis_peptide_quad_info_t::type_t type = coot::util::cis_peptide_quad_info_t::CIS;
         if (m.is_pre_pro_cis_peptide) type = coot::util::cis_peptide_quad_info_t::PRE_PRO_CIS;
         if (m.is_twisted)             type = coot::util::cis_peptide_quad_info_t::TWISTED_TRANS;


         std::pair<std::vector<coot::api::vnc_vertex>, std::vector<g_triangle> > cpg = make_cis_peptide_geom(glm_quad, type);

         unsigned int idx_base = vertices.size();
         unsigned int idx_tri_base = triangles.size();

         vertices.insert(vertices.end(), cpg.first.begin(), cpg.first.end());
         triangles.insert(triangles.end(), cpg.second.begin(), cpg.second.end());
         for (unsigned int jj=idx_tri_base; jj<triangles.size(); jj++)
            triangles[jj].rebase(idx_base);
      }
   }
}




void
make_graphical_bonds_bonds(coot::simple_mesh_t &m,
                           const graphical_bonds_container &gbc,
                           float bond_radius,
                           unsigned int n_slices,
                           unsigned int n_stacks,
                           const std::vector<glm::vec4> &colour_table) {

   // unsigned int n_slices = 8;
   // unsigned int n_stacks = 2; // try 1 later.

   // do we need 4 version of this for the end caps? None, left only, right only and both.

   auto get_bond_matrix = [] (const glm::vec3 &pos_1, const glm::vec3 &pos_2, float radius) {
                             glm::vec3 delta = pos_2 - pos_1;
                             float l_delta = glm::distance(pos_2, pos_1);
                             glm::mat4 u(1.0f);
                             glm::mat4 sc = glm::scale(u, glm::vec3(radius, radius, l_delta));
                             // orient
                             glm::vec3 normalized_bond_orientation(glm::normalize(delta));
                             glm::mat4 ori = glm::orientation(normalized_bond_orientation, glm::vec3(0.0, 0.0, 1.0)); // nice
                             // translate
                             glm::mat4 t = glm::translate(u, pos_1);
                             glm::mat4 m = t * ori * sc;
                             return m;
                          };

   auto &vertices  = m.vertices;
   auto &triangles = m.triangles;

   std::pair<glm::vec3, glm::vec3> pp(glm::vec3(0,0,0), glm::vec3(0,0,1));

   // ----------------------- add the vertices and triangles ----------------------

   int colour_table_size = colour_table.size(); // change type
   if (colour_table_size < gbc.n_consolidated_atom_centres) {
      std::cout << "ERROR:: colour_table too small " << colour_table.size() << " vs " << gbc.n_consolidated_atom_centres << std::endl;
      return;
   }

   // std::cout << "debug:: in make_graphical_bonds_bonds() there are " << gbc.num_colours << " colours " << std::endl;
   for (int icol=0; icol<gbc.num_colours; icol++) {
      glm::vec4 col = colour_table[icol];
      graphical_bonds_lines_list<graphics_line_t> &ll = gbc.bonds_[icol];
      // std::cout << "debug:: in make_graphical_bonds_bonds() colour icol " << icol  << " has " << ll.num_lines << " bond lines " << std::endl;
      for (int j=0; j<ll.num_lines; j++) {
         const coot::Cartesian &start  = ll.pair_list[j].positions.getStart();
         const coot::Cartesian &finish = ll.pair_list[j].positions.getFinish();
         bool thin = ll.thin_lines_flag;
         float bond_radius_this = bond_radius;
         if (thin)
            bond_radius_this *= 0.5;
         if (ll.pair_list[j].cylinder_class == graphics_line_t::KEK_DOUBLE_BOND_INNER_BOND)
            bond_radius_this *= 0.7;
         float bl = ll.pair_list[j].positions.amplitude();
         glm::vec3 pos_1(start.x(),   start.y(),  start.z());
         glm::vec3 pos_2(finish.x(), finish.y(), finish.z());
         // glm::mat4 mm = get_bond_matrix(pos_1, pos_2, bond_radius);
         if (false)
            std::cout << "making bond between " << glm::to_string(pos_1) << " " << glm::to_string(pos_2)
                      << " width " << bond_radius_this << std::endl;
         cylinder cc(std::make_pair(pos_1, pos_2), bond_radius_this, bond_radius_this, bl, col, n_slices, n_stacks);
         cc.set_unstubby_rounded_cap_factor(1.0);

         // we can't add the octasphere caps yet because they appear in the middle of double bonds
         // there should be a means to say "no end caps at all"  - or cap with a hemisphere or
         // cap flat (the end of a double bond).

         if (ll.pair_list[j].has_begin_cap) {
            cc.add_flat_start_cap();
         } else {
            // cc.add_octahemisphere_start_cap();
         }
         if (ll.pair_list[j].has_end_cap) {
            cc.add_flat_end_cap();
         } else {
            // cc.add_octahemisphere_end_cap();
         }

         // 20230110-PE class no longer has a colour index
         // for (auto &tri : cc.triangles) // for export to blender
         // tri.colour_index = icol;

         unsigned int idx_base = vertices.size();
         unsigned int idx_tri_base = triangles.size();
         vertices.insert(vertices.end(), cc.vertices.begin(), cc.vertices.end());
         triangles.insert(triangles.end(), cc.triangles.begin(), cc.triangles.end());
         for (unsigned int k=idx_tri_base; k<triangles.size(); k++)
            triangles[k].rebase(idx_base);
      }
   }

}

// put this in utils?
#include "coot-colour.hh"
#include "colour-functions.hh"

glm::vec4
coot::molecule_t::get_bond_colour_by_colour_wheel_position(int icol, coot::api_bond_colour_t bonds_box_type) const {

   std::vector<float> rgb(3);
   rgb[0] = 0.2f; rgb[1] =  0.2f; rgb[2] =  0.8f; // blue

   bool done = false;
   int offset = 0; // blue starts at 0

   if (bonds_box_type == coot::api_bond_colour_t::COLOUR_BY_USER_DEFINED_COLOURS____BONDS) {
      if (icol == 0) {
         rgb[0] = 0.8f; rgb[1] =  0.8f; rgb[2] =  0.8f; // white
         done = true;
      }
      if (icol == 1) {
         rgb[0] = 0.3f; rgb[1] =  0.3f; rgb[2] =  0.3f; // dark-grey
         done = true;
      }
      offset=2; // blue starts at 2.
   }

   if (false)
      std::cout << "debug set_bond_colour_by_colour_wheel_position() " << icol
                << " box_type " << int(bonds_box_type) << " vs " << int(coot::api_bond_colour_t::COLOUR_BY_USER_DEFINED_COLOURS____BONDS)
                << std::endl;

   if (bonds_box_type == coot::api_bond_colour_t::CA_BONDS_PLUS_LIGANDS_B_FACTOR_COLOUR) {
      rgb[0] = 0.3f; rgb[1] =  0.3f; rgb[2] =  0.95f;
      const unsigned int n_b_factor_colours = 48; // matches index_for_b_factor() in my_atom_colour_map_t
      float f = static_cast<float>(icol)/static_cast<float>(n_b_factor_colours);
      // f is in the range 0 to 1
      const float pi = 3.1415926535;
      float rotation_size = -0.11 * f * 2.0  * pi;
      if (rotation_size < -0.6666) rotation_size = -0.66666; // otherwise black bonds
      // std::cout << "rotation_size: " << rotation_size << std::endl;
      rgb = rotate_rgb(rgb, rotation_size);
      done = true;
   }
   if (! done) {
      float max_colour = 30;

      // 30 is the size of rainbow colours, 0 -> 1.0 is the range of rainbow colours

      if (bonds_box_type == coot::api_bond_colour_t::COLOUR_BY_RAINBOW_BONDS) {
         // rotation_size is a fraction of a circle
         float rotation_size = 1.0 - float(icol-offset) * 0.7/max_colour;
         rgb = rotate_rgb(rgb, rotation_size);
         if (false)
            std::cout << "icol " << std::setw(2) <<  icol << " rotation size " << std::setw(7) << rotation_size
                      << "  rgb " << std::setw(6) << rgb[0] << " " << std::setw(6) << rgb[1] << " "
                      << std::setw(6) << rgb[2] << std::endl;
      } else {
         float rotation_size = 1.0 - float(icol-offset) * 0.7/max_colour + bonds_colour_map_rotation/360.0;
         rgb = rotate_rgb(rgb, rotation_size);
      }
   }

   // rotation_size size has useful colours between
   // 1.0 (or higher?) and 0.0.
   // Below 0.0, to -0.65 (more than -0.68) (dependant on starting rgb
   // values I guess) there is amusing colour continuation of the colour
   // wheel (red through purple to blue and further).
   //
   if (false)
      std::cout << "set_bond_colour_by_colour_wheel_position "  << icol << " " << " "
                << rgb[0] << " " << rgb[1] << " " << rgb[2] << " " << std::endl;


   glm::vec4 colour(rgb[0], rgb[1], rgb[2], 1.0f);
   return colour;
}

coot::colour_t
coot::molecule_t::get_bond_colour_basic(int colour_index, bool against_a_dark_background) const {

   // this is used for intermediate atoms

   coot::colour_t col(0.5, 0.5, 0.5);

   if (true) {  // background check
      switch (colour_index) {
      case CARBON_BOND:
         col = coot::colour_t (0.2, 0.7, 0.1);
         break;
      case GREEN_BOND:
         col = coot::colour_t (0.0, 0.7, 0.0);
         break;
      case BLUE_BOND:
         col = coot::colour_t (0.2, 0.2, 0.8);
         break;
      case RED_BOND:
         col = coot::colour_t (0.8, 0.1, 0.1);
         break;
      case YELLOW_BOND:
         col = coot::colour_t (0.7, 0.7, 0.0);
         break;
      case GREY_BOND:
         col = coot::colour_t (0.5, 0.5, 0.5);
         break;
      case HYDROGEN_GREY_BOND:
         col = coot::colour_t (0.7, 0.7, 0.7);
         break;
      case DEUTERIUM_PINK:
         col = coot::colour_t (0.8, 0.6, 0.64);
         break;
      case MAGENTA_BOND:
         col = coot::colour_t (0.8, 0.1, 0.8);
         break;
      case DARK_GREEN_BOND:
         col = coot::colour_t (0.05, 0.39, 0.05);
         break;
      case DARK_ORANGE_BOND:
         col = coot::colour_t (0.7, 0.7, 0.05);
         break;
      case DARK_BROWN_BOND:
         col = coot::colour_t (0.5, 0.5, 0.1);
         break;
      case VIOLET:
         col = coot::colour_t(0.93, 0.51, 0.93);
         break;
      case DARK_VIOLET:
         col = coot::colour_t(0.58, 0.0, 0.83);
         break;
      default:
         col = coot::colour_t (0.7, 0.8, 0.8);
      }
   }
   return col;

}

coot::colour_t
coot::molecule_t::get_bond_colour_by_mol_no(int colour_index, bool against_a_dark_background) const {

   // 20231017-PE note to self - this colour can be over-ridden by user-define bond colours
   //             when the colour table is made.

   // std::cout << "get_bond_colour_by_mol_no() col-index: " << colour_index
   // << " dark-bg-flag: " << against_a_dark_background << std::endl;

   coot::colour_t rgb;

   bool bonds_rotate_colour_map_flag = true;
   bool rotate_colour_map_on_read_pdb_c_only_flag = true;

   if (bonds_rotate_colour_map_flag == 0) {
      rgb = get_bond_colour_basic(colour_index, against_a_dark_background);
      // err = glGetError(); if (err) std::cout << "GL error in get_bond_colour_by_mol_no() 1 a1 " << err << std::endl;
   } else {

      float rotation_size = bonds_colour_map_rotation/360.0;

      // rotation_size typically then: 2*32/360 = 0.178

      // This is for colour-by-chain-carbons-only carbon colour

      if (colour_index >= 50) {

         if (use_bespoke_grey_colour_for_carbon_atoms) {
            rgb = bespoke_carbon_atoms_colour;
         } else {
            float ii_f = colour_index - 50;
            ii_f += 1.2 * static_cast<float>(imol_no);
            if (against_a_dark_background) {
               rgb.set(base_colour_for_bonds.red   + 0.2,
                       base_colour_for_bonds.green + 0.2,
                       base_colour_for_bonds.blue  + 0.2);
            } else {
               rgb.set(base_colour_for_bonds.red   * 0.92,
                       base_colour_for_bonds.green * 0.92,
                       base_colour_for_bonds.blue  * 0.92);
            }
            float ra = ii_f*79.0/360.0;
            ra += rotation_size;
            while (ra > 1.0) ra -= 1.0;
            if (ra > 0) {
               rgb.rotate(ra);
            }
         }
         // std::cout << "get_bond_colour_by_mol_no() get chain colour for colour_index "
         // << colour_index << " " << rgb << std::endl;
         // err = glGetError(); if (err) std::cout << "GL error in get_bond_colour_by_mol_no() 1b " << colour_index << std::endl;

      } else {

         while (rotation_size > 1.0) { // no more black bonds?
            rotation_size -= 1.0;
         }

         if (against_a_dark_background) {

            if (false)
               std::cout << "get_bond_colour_by_mol_no() against_a_dark_background==true, idx: " << colour_index << " vs "
                         << " green "   << GREEN_BOND << " "
                         << " blue "    << BLUE_BOND << " "
                         << " red "     << RED_BOND << " "
                         << " yellow "  << YELLOW_BOND << " "
                         << " grey "    << GREY_BOND << " "
                         << " H-grey "  << HYDROGEN_GREY_BOND << " "
                         << " magenta " << MAGENTA_BOND << " "
                         << std::endl;

            switch (colour_index) {
            case CARBON_BOND:
               if (use_bespoke_grey_colour_for_carbon_atoms) {
                  rgb = bespoke_carbon_atoms_colour;
               } else {
                  rgb[0] = 0.7; rgb[1] =  0.7; rgb[2] =  0.0;
               }
               break;
            case YELLOW_BOND:
               rgb[0] = 0.6; rgb[1] = 0.98; rgb[2] =  0.2;
               break;
            case BLUE_BOND:
               rgb[0] = 0.25; rgb[1] =  0.25; rgb[2] =  1.0;
               break;
            case RED_BOND:
               rgb[0] = 0.9; rgb[1] =  0.0; rgb[2] =  0.0;
               break;
            case GREEN_BOND:
               rgb[0] = 0.1; rgb[1] =  0.8; rgb[2] =  0.1;
               break;
            case GREY_BOND:
               rgb[0] = 0.6; rgb[1] =  0.6; rgb[2] =  0.6;
               break;
            case HYDROGEN_GREY_BOND:
               rgb[0] = 0.75; rgb[1] =  0.75; rgb[2] =  0.75;
               break;
            case DEUTERIUM_PINK:
               rgb[0] = 0.8; rgb[1] =  0.6; rgb[2] =  0.64;
               break;
               // replaced in mmdb-extras.h
               //       case white:
               //          rgb[0] = 0.99; rgb[1] =  0.99; rgb[2] = 0.99;
               //          break;
            case MAGENTA_BOND:
               rgb[0] = 0.99; rgb[1] =  0.2; rgb[2] = 0.99;
               break;
            case ORANGE_BOND:
               rgb[0] = 0.89; rgb[1] =  0.89; rgb[2] = 0.1;
               break;
            case CYAN_BOND:
               rgb[0] = 0.1; rgb[1] =  0.89; rgb[2] = 0.89;
               break;
            case DARK_GREEN_BOND:
               rgb[0] = 0.05; rgb[1] =  0.39; rgb[2] =  0.05;
               break;
            case DARK_ORANGE_BOND:
               rgb[0] = 0.7; rgb[1] =  0.7; rgb[2] = 0.05;
               break;
            case DARK_BROWN_BOND:
               rgb[0] = 0.5; rgb[1] =  0.5; rgb[2] = 0.1;
               break;
            case VIOLET:
               rgb[0] = 0.93; rgb[1] = 0.51; rgb[2] = 0.93;
               break;
            case DARK_VIOLET:
               rgb[0] = 0.58; rgb[1] = 0.0; rgb[2] = 0.83;
               break;
            default:
               rgb[0] = 0.8; rgb[1] =  0.2; rgb[2] =  0.2;
               rgb.rotate(colour_index*26.0/360.0);
            }

         } else {

            // against a white background.  Less pale (more saturated) and darker.

            switch (colour_index) {
            case CARBON_BOND:
               if (use_bespoke_grey_colour_for_carbon_atoms) {
                  rgb = bespoke_carbon_atoms_colour;
               } else {
                  rgb[0] = 0.2; rgb[1] =  0.2; rgb[2] =  0.0;
               }
               break;
            case YELLOW_BOND:
               rgb[0] = 0.7; rgb[1] =  0.7; rgb[2] =  0.0;
               break;
            case BLUE_BOND:
               rgb[0] = 0.1; rgb[1] =  0.1; rgb[2] =  0.6;
               break;
            case RED_BOND:
               rgb[0] = 0.6; rgb[1] =  0.1; rgb[2] =  0.075; // more tomatoey.
               break;
            case GREEN_BOND:
               rgb[0] = 0.05; rgb[1] =  0.6; rgb[2] =  0.05;
               break;
            case GREY_BOND:
               rgb[0] = 0.5; rgb[1] =  0.5; rgb[2] =  0.5;
               break;
            case HYDROGEN_GREY_BOND:
               rgb[0] = 0.6; rgb[1] =  0.6; rgb[2] =  0.6;
               break;
            case DEUTERIUM_PINK:
               rgb[0] = 0.8; rgb[1] =  0.6; rgb[2] =  0.64;
               break;
            case MAGENTA_BOND:
               rgb[0] = 0.5; rgb[1] =  0.1; rgb[2] = 0.5;
               break;
            case ORANGE_BOND:
               rgb[0] = 0.5; rgb[1] =  0.5; rgb[2] = 0.1;
               break;
            case CYAN_BOND:
               rgb[0] = 0.1; rgb[1] =  0.5; rgb[2] = 0.5;
               break;
            case DARK_GREEN_BOND:
               rgb[0] = 0.05; rgb[1] =  0.39; rgb[2] =  0.05;
               break;
            case DARK_ORANGE_BOND:
               rgb[0] = 0.7; rgb[1] =  0.7; rgb[2] = 0.05;
               break;
            case DARK_BROWN_BOND:
               rgb[0] = 0.5; rgb[1] =  0.5; rgb[2] = 0.1;
               break;
            case VIOLET:
               rgb[0] = 0.93; rgb[1] = 0.51; rgb[2] = 0.93;
               break;
            case DARK_VIOLET:
               rgb[0] = 0.58; rgb[1] = 0.0; rgb[2] = 0.83;
               break;

            default:
               rgb[0] = 0.5; rgb[1] =  0.1; rgb[2] =  0.1;
               // rgb = rotate_rgb(rgb, float(i*26.0/360.0));
               rgb.rotate(colour_index*26.0/360.0);
            }
         }

         // err = glGetError(); if (err) std::cout << "GL status in get_bond_colour_by_mol_no() 1e " << err << std::endl;

         // "correct" for the +1 added in the calculation of the rotation
         // size.
         // 21. is the default colour map rotation

         rgb.rotate(float(1.0 - 21.0/360.0));

         // err = glGetError(); if (err) std::cout << "GL status in get_bond_colour_by_mol_no() 1f " << err << std::endl;

         if (rotate_colour_map_on_read_pdb_c_only_flag) {

            if (colour_index == CARBON_BOND) {
               if (use_bespoke_grey_colour_for_carbon_atoms) {
                  rgb = bespoke_carbon_atoms_colour;
               } else {
                  rgb.rotate(rotation_size);
               }
            }
            // err = glGetError(); if (err) std::cout << "GL status in get_bond_colour_by_mol_no() 1h " << err << std::endl;
         } else {
            // err = glGetError(); if (err) std::cout << "GL status in get_bond_colour_by_mol_no() 1k " << err << std::endl;
            rgb.rotate(rotation_size);
            // err = glGetError(); if (err) std::cout << "GL status in get_bond_colour_by_mol_no() 1l " << err << std::endl;
         }
      }
   }

   // std::cout << "returning rgb " << rgb << std::endl;
   return rgb;
}

std::vector<glm::vec4>
coot::molecule_t::make_colour_table(bool dark_bg_flag) const {

   // make this function more generally available - make it a member function/friend using ostream
   //
   auto _ = [] (api_bond_colour_t bct) {
      std::string s = std::to_string(int(bct));
      if (bct == api_bond_colour_t::UNSET_TYPE)              s = "UNSET_TYPE";
      if (bct == api_bond_colour_t::NORMAL_BONDS)            s = "NORMAL_BONDS";
      if (bct == api_bond_colour_t::CA_BONDS)                s = "CA_BONDS";
      if (bct == api_bond_colour_t::COLOUR_BY_CHAIN_BONDS  ) s = "COLOUR_BY_CHAIN_BONDS";
      if (bct == api_bond_colour_t::CA_BONDS_PLUS_LIGANDS)   s = "CA_BONDS_PLUS_LIGANDS";
      if (bct == api_bond_colour_t::BONDS_NO_WATERS)         s = "BONDS_NO_WATERS";
      if (bct == api_bond_colour_t::BONDS_SEC_STRUCT_COLOUR) s = "BONDS_SEC_STRUCT_COLOUR";
      if (bct == api_bond_colour_t::BONDS_NO_HYDROGENS)      s = "BONDS_NO_HYDROGENS";
      if (bct == api_bond_colour_t::CA_BONDS_PLUS_LIGANDS_SEC_STRUCT_COLOUR) s = "CA_BONDS_PLUS_LIGANDS_SEC_STRUCT_COLOUR";
      if (bct == api_bond_colour_t::CA_BONDS_PLUS_LIGANDS_B_FACTOR_COLOUR) s = "CA_BONDS_PLUS_LIGANDS_B_FACTOR_COLOUR";
      if (bct == api_bond_colour_t::CA_BONDS_PLUS_LIGANDS_AND_SIDECHAINS) s = "CA_BONDS_PLUS_LIGANDS_AND_SIDECHAINS";
      if (bct == api_bond_colour_t::COLOUR_BY_MOLECULE_BONDS)  s = "COLOUR_BY_MOLECULE_BONDS";
      if (bct == api_bond_colour_t::COLOUR_BY_RAINBOW_BONDS)   s = "COLOUR_BY_RAINBOW_BONDS";
      if (bct == api_bond_colour_t::COLOUR_BY_B_FACTOR_BONDS)  s = "COLOUR_BY_B_FACTOR_BONDS";
      if (bct == api_bond_colour_t::COLOUR_BY_OCCUPANCY_BONDS) s = "COLOUR_BY_OCCUPANCY_BONDS";
      if (bct == api_bond_colour_t::COLOUR_BY_USER_DEFINED_COLOURS____BONDS) s = "COLOUR_BY_USER_DEFINED_COLOURS____BONDS";
      if (bct == api_bond_colour_t::COLOUR_BY_USER_DEFINED_COLOURS_CA_BONDS) s = "COLOUR_BY_USER_DEFINED_COLOURS_CA_BONDS";
      if (bct == api_bond_colour_t::COLOUR_BY_CHAIN_GOODSELL) s = "COLOUR_BY_CHAIN_GOODSELL";

      return s;
   };

   bool is_intermediate_atoms_molecule = false; // make a class member

   bool debug_colour_table = false;

   if (debug_colour_table) {
      std::cout << "........ in make_colour_table() A with bonds_box_type " << _(bonds_box_type) << std::endl;
      std::cout << "........ in make_colour_table() A with num_colours " << bonds_box.num_colours << std::endl;
      std::cout << "........ in make_colour_table() A with dark_bg_flag " << dark_bg_flag << std::endl;
   }

   std::vector<glm::vec4> colour_table;
   // std::cout << "................... in make_colour_table() HERE C " << bonds_box_type << std::endl;
   colour_table = std::vector<glm::vec4>(bonds_box.num_colours, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
   std::set<int> done_colours;
   for (int icol=0; icol<bonds_box.num_colours; icol++) {

      if (bonds_box_type == coot::api_bond_colour_t::COLOUR_BY_RAINBOW_BONDS) {
         glm::vec4 col = get_bond_colour_by_colour_wheel_position(icol, coot::api_bond_colour_t::COLOUR_BY_RAINBOW_BONDS);
         colour_table[icol] = col;
      } else {
         graphical_bonds_lines_list<graphics_line_t> &ll = bonds_box.bonds_[icol];
         int n_bonds = ll.num_lines;
         if (n_bonds > 0) {
            coot::colour_t cc = get_bond_colour_by_mol_no(icol, dark_bg_flag);
            colour_table[icol] = cc.to_glm();
            done_colours.insert(icol);

            // was there a user-defined bond colour that superceeds this for this colour index?
            if (! user_defined_bond_colours.empty()) {

               std::map<unsigned int, colour_holder>::const_iterator it;
               it = user_defined_bond_colours.find(icol);
               if (it != user_defined_bond_colours.end()) {
                  auto glm_col = colour_holder_to_glm(it->second);
                  colour_table[icol] = glm_col;
                  done_colours.insert(icol);
               } else {
                  if (false)
                     std::cout << "debug:: user_defined_bond_colours has size " << user_defined_bond_colours.size()
                               << " index " << icol << " was not found in the user-defined bond colours"
                               << std::endl;
               }
            } else {
               if (debug_colour_table)
                  std::cout << "debug:: Sad! user_defined_bond_colours was empty" << std::endl;
            }

            if (debug_colour_table) { // debugging colours
               std::cout << "debug:: colour_table: B imol " << imol_no << " icol " << std::setw(3) << icol << " has "
                         << std::setw(4) << n_bonds << " bonds dark-bg: " << dark_bg_flag
                         << " colour: " << glm::to_string(colour_table[icol]) << std::endl;
            }
         } else {
            // std::cout << "No bonds for colour with index " << icol << std::endl;
         }
      }
   }

   // now add the colour for the atoms (if they have not been added)

   if (bonds_box.n_atom_centres_ > 0) {
      for (int i=0; i<bonds_box.n_atom_centres_; i++) {
         int icol = bonds_box.atom_centres_colour_[i];
         if (done_colours.find(icol) == done_colours.end()) {
            // std::cout << "*********************** adding atom col (that has no bonds!) " << icol << std::endl;
            coot::colour_t cc = get_bond_colour_by_mol_no(icol, dark_bg_flag);
            colour_table[icol] = cc.to_glm();
         }
      }
   }


   if (debug_colour_table) {
      std::cout << "Here is the user-defined colour table:" << std::endl;
      std::map<unsigned int, colour_holder>::const_iterator it_bc;
      for (it_bc=user_defined_bond_colours.begin(); it_bc!=user_defined_bond_colours.end(); ++it_bc) {
         std::cout << "   " << it_bc->first << " " << it_bc->second << std::endl;
      }
   }

   // 20220303-PE why does this happen? (it happens when refining the newly imported 3GP ligand)
   // I guess the bonds_box for the remaining atoms (there are none of them) is incorrectly constructed.
   // FIXME later.
   // Note: we were called fromm this function:
   // void
   // molecule_class_info_t::makebonds(const coot::protein_geometry *geom_p,
   //                              const std::set<int> &no_bonds_to_these_atoms)
   //
   if (bonds_box.n_consolidated_atom_centres > bonds_box.num_colours) {
      colour_table = std::vector<glm::vec4>(bonds_box.n_consolidated_atom_centres, glm::vec4(0.6f, 0.0f, 0.6f, 1.0f));
   }

   auto pastelize = [] (glm::vec4 &col, float degree) {
                       for (unsigned int i=0; i<3; i++) {
                          const float &cc = col[i];
                          float r = 1.0f - cc;
                          col[i] += r * degree;
                          col[i] *= (1.0f - 0.5f * degree); // I don't want bright pastel
                       }
                    };

   if (is_intermediate_atoms_molecule) {
      // pastelize the colour table
      float degree = 0.5f;
      for (auto &col : colour_table) {
         pastelize(col, degree); //ref
      }
   }
   return colour_table;
}

std::vector<glm::vec4>
coot::molecule_t::make_colour_table_for_goodsell_style(float colour_wheel_rotation_step, float saturation, float goodselliness) const {

   bool debug_colour_table = true;
   float gcwrs = colour_wheel_rotation_step;
   std::vector<glm::vec4> colour_table;
   int icol_max = -1;
   for (int i=0; i<bonds_box.n_atom_centres_; i++) {
      int icol = bonds_box.atom_centres_colour_[i];
      if (icol > icol_max) {
         icol_max = icol;
      }
   }
   if (icol_max > -1) {
      colour_table.resize(icol_max+1);
      std::set<int> done_colours;
      for (int i=0; i<bonds_box.n_atom_centres_; i++) {
         int icol = bonds_box.atom_centres_colour_[i];
         if (done_colours.find(icol) == done_colours.end()) {
            // c.f. molecules-class-info.cc lines 4000 or so
            coot::colour_holder ch(0.8, 0.5, 0.6);
            int ic = icol - 100;
            bool is_C = !(ic%2);
            int chain_index =  ic/2;
            float pastel_factor = 1.0/saturation;
            ch.pastelize(pastel_factor);
            float rotation_angle = gcwrs * static_cast<float>(chain_index);
            ch.rotate_by(rotation_angle);
            if (is_C) ch.make_pale(goodselliness);
            colour_table[icol] = glm::vec4(ch.red, ch.green, ch.blue, 1.0);
         }
      }
   }
   if (debug_colour_table) {
      std::map<int, unsigned int> col_set;
      std::cout << "Here is the colour table " << std::endl;
      for (int i=0; i<bonds_box.n_atom_centres_; i++) {
         int icol = bonds_box.atom_centres_colour_[i];
         if (col_set.find(icol) == col_set.end())
            col_set[icol] = 1;
         else
            col_set[icol]++;
      }
      std::map<int, unsigned int>::const_iterator it;
      for (it=col_set.begin(); it!=col_set.end(); ++it) {
         int idx = it->first;
         std::cout << "   col-idx: " << it->first << "  counts: " << it->second << " "
                   << glm::to_string(colour_table[idx]) << std::endl;
      }

   }
   return colour_table;

}

void
coot::molecule_t::print_colour_table(const std::string &l) const {

   std::cout << "----------- Here is the user-defined colour table: " << l << " -------" << std::endl;
   std::map<unsigned int, colour_holder>::const_iterator it_bc;
   for (it_bc=user_defined_bond_colours.begin(); it_bc!=user_defined_bond_colours.end(); ++it_bc) {
      std::cout << "   " << it_bc->first << " " << it_bc->second << std::endl;
   }

   bool against_a_dark_background = false;
   std::vector<glm::vec4> colour_table = make_colour_table(against_a_dark_background);
   std::cout << "----------- Here is the colour table: " << l << " -------" << std::endl;
   for (unsigned int i=0; i<colour_table.size(); i++) {
      std::cout << "    " << i << " " << glm::to_string(colour_table[i]) << std::endl;
   }

}

coot::simple_mesh_t
coot::molecule_t::get_goodsell_style_mesh(coot::protein_geometry *geom_p, float colour_wheel_rotation_step,
                                          float saturation, float goodselliness) {

   std::set<int> empty_set;
   bool goodsell_mode = true;

   bool draw_hydrogen_atoms_flag = false;
   bool do_rama_markup = false;
   bool draw_missing_loops_flag = false;
   Bond_lines_container bonds(geom_p, empty_set, draw_hydrogen_atoms_flag);

   bool change_c_only_flag = false;
   bonds_box_type = api_bond_colour_t::COLOUR_BY_CHAIN_GOODSELL;
   bonds.do_colour_by_chain_bonds(atom_sel, false, imol_no, draw_hydrogen_atoms_flag,
                                  draw_missing_loops_flag, change_c_only_flag, goodsell_mode, do_rama_markup);
   bonds_box = bonds.make_graphical_bonds();
   std::vector<glm::vec4> colour_table = make_colour_table_for_goodsell_style(colour_wheel_rotation_step, saturation, goodselliness);
   // print_colour_table("--goodsell--");
   unsigned int num_subdivisions = 3;
   // no bonds in a goodsell mode representation
   simple_mesh_t m;
   make_graphical_bonds_spherical_atoms_with_vdw_radii(m, bonds_box, num_subdivisions, colour_table, *geom_p);
   return m;

}


coot::simple_mesh_t
coot::molecule_t::get_bonds_mesh(const std::string &mode, coot::protein_geometry *geom,
                                 bool against_a_dark_background,
                                 float bonds_width, float atom_radius_to_bond_width_ratio,
                                 int smoothness_factor,
                                 bool draw_hydrogen_atoms_flag,
                                 bool draw_missing_residue_loops_flag) {

   auto test_udd_handle = [] (mmdb::Manager *mol, int udd_handle_bonded_type) {

      // test that the udd_handle_bonded_type is useful
      int imod = 1;
      mmdb::Model *model_p = mol->GetModel(imod);
      if (model_p) {
         int n_chains = model_p->GetNumberOfChains();
         for (int ichain=0; ichain<n_chains; ichain++) {
            mmdb::Chain *chain_p = model_p->GetChain(ichain);
            int n_res = chain_p->GetNumberOfResidues();
            for (int ires=0; ires<n_res; ires++) {
               mmdb::Residue *residue_p = chain_p->GetResidue(ires);
               if (residue_p) {
                  int n_atoms = residue_p->GetNumberOfAtoms();
                  for (int iat=0; iat<n_atoms; iat++) {
                     mmdb::Atom *at = residue_p->GetAtom(iat);
                     if (! at->isTer()) {
                        int state = -1;
                        int udd_get_handle_success = at->GetUDData(udd_handle_bonded_type, state);
                        std::string e;
                        if (udd_get_handle_success == mmdb::UDDATA_WrongHandle)  e = "UDDATA_WrongHandle";
                        if (udd_get_handle_success == mmdb::UDDATA_WrongUDRType) e = "UDDATA_WrongUDRType";
                        if (udd_get_handle_success == mmdb::UDDATA_NoData)       e = "UDDATA_NoData";
                        std::cout << " start get_bonds_mesh() " << coot::atom_spec_t(at)
                                  << " handle: " << udd_handle_bonded_type
                                  << " udd_get_handle_success: " << udd_get_handle_success << " " << e
                                  << " state: " << state << std::endl;
                     }
                  }
               }
            }
         }
      }
   };


   simple_mesh_t m;

   float bond_radius = bonds_width;
   float atom_radius = bond_radius;
   if (atom_radius_to_bond_width_ratio > 1.0)
      atom_radius = bond_radius * atom_radius_to_bond_width_ratio;

   unsigned int num_subdivisions = 1;
   unsigned int n_slices = 8;
   unsigned int n_stacks = 2;
   // int representation_type = BALL_AND_STICK;

   if (smoothness_factor == 2) {
      num_subdivisions = 2;
      n_slices = 16; // was 18
   }

   if (smoothness_factor == 3) {
      num_subdivisions = 3;
      n_slices = 32;
   }

   if (smoothness_factor == 4) {
      num_subdivisions = 4;
      n_slices = 64;
   }

   bonds_box_type = coot::api_bond_colour_t::COLOUR_BY_CHAIN_BONDS;

   const std::set<int> &no_bonds_to_these_atoms = no_bonds_to_these_atom_indices;

   if (mode == "CA+LIGANDS") {
      bool do_bonds_to_hydrogens = false;
      Bond_lines_container bonds(geom, "dummy-CA-mode", no_bonds_to_these_atom_indices, do_bonds_to_hydrogens);
      float min_dist = 2.4;
      float max_dist = 4.7;
      bonds.do_Ca_plus_ligands_bonds(atom_sel, imol_no, geom, min_dist, max_dist, draw_hydrogen_atoms_flag, draw_missing_residue_loops_flag);
      bonds_box = bonds.make_graphical_bonds_no_thinning();
      std::vector<glm::vec4> colour_table = make_colour_table(against_a_dark_background);
      make_graphical_bonds_bonds(m, bonds_box, bond_radius, n_slices, n_stacks, colour_table);
   }

   if (mode == "COLOUR-BY-CHAIN-AND-DICTIONARY") {

      // we don't make rotamer dodecs in this function
      makebonds(geom, nullptr, no_bonds_to_these_atoms, draw_hydrogen_atoms_flag, draw_missing_residue_loops_flag); // this makes the bonds_box.

      // get the udd_handle_bonded_type after making the bonds (because the handle is made by making the bond)
      int udd_handle_bonded_type = atom_sel.mol->GetUDDHandle(mmdb::UDR_ATOM, "found bond");
      if (udd_handle_bonded_type == mmdb::UDDATA_WrongUDRType) {
         std::cout << "ERROR:: in get_bonds_mesh() wrong udd data type " << udd_handle_bonded_type << std::endl;
         return m;
      } else {
         // std::cout << "debug:: OK, udd_handle_bonded_type is " << udd_handle_bonded_type
         // << " not " << mmdb::UDDATA_WrongUDRType << std::endl;
      }

      if (false)
         test_udd_handle(atom_sel.mol, udd_handle_bonded_type);

      std::vector<glm::vec4> colour_table = make_colour_table(against_a_dark_background);
      if (colour_table.empty()) {
         std::cout << "ERROR:: you need to make the bonds before getting the bonds mesh" << std::endl;
      }

      const graphical_bonds_container &gbc = bonds_box; // alias because it's named like that in Mesh-from-graphical-bonds

      unsigned int n_bonds = 0;
      for (int icol_bond=0; icol_bond<gbc.num_colours; icol_bond++) {
         graphical_bonds_lines_list<graphics_line_t> &ll = gbc.bonds_[icol_bond];
         n_bonds += ll.num_lines;
      }
      unsigned int allocation_for_vertices  = 68 * n_bonds;
      unsigned int allocation_for_triangles = 80 * n_bonds;

      // "just checking"
      int cts = colour_table.size();
      if (cts < gbc.num_colours)
         std::cout << "ERROR:: wrong size colour table in get_bonds_mesh()" << std::endl;
      // OK, the the colour map
      for (int icol_bond=0; icol_bond<gbc.num_colours; icol_bond++) {
         m.colour_index_to_colour_map[icol_bond] = colour_table[icol_bond];
      }

      m.vertices.reserve(allocation_for_vertices);
      m.triangles.reserve(allocation_for_triangles);

      // these functions are in this file
      //
      make_graphical_bonds_spherical_atoms(m, gbc, bonds_box_type, udd_handle_bonded_type,
                                           atom_radius, bond_radius, num_subdivisions, colour_table);
      make_graphical_bonds_hemispherical_atoms(m, gbc, bonds_box_type, udd_handle_bonded_type, atom_radius,
                                               bond_radius, num_subdivisions, colour_table);

      if (true) // 20221023-PE was (representation_type == BALL_AND_STICK)
         make_graphical_bonds_bonds(m, gbc, bond_radius, n_slices, n_stacks, colour_table);

      if (true) // 20221023-PE was (draw_cis_peptides)
         make_graphical_bonds_cis_peptides(m, gbc);

   }

   if (mode == "VDW-BALLS") {
      makebonds(geom, nullptr, no_bonds_to_these_atoms, draw_hydrogen_atoms_flag, draw_missing_residue_loops_flag); // this makes the bonds_box.
      std::vector<glm::vec4> colour_table = make_colour_table(against_a_dark_background);
      make_graphical_bonds_spherical_atoms_with_vdw_radii(m, bonds_box, num_subdivisions, colour_table, *geom);
   }

   return m;
}



generic_3d_lines_bonds_box_t
coot::molecule_t::make_exportable_environment_bond_box(coot::residue_spec_t &spec, coot::protein_geometry &geom) const {

   // 20230327-PE when you make the libcootapi version, just export this function
   // and make a simple_mesh_t from the bonds_box_env.
   //
   auto make_bonds_box_env = [] (const coot::residue_spec_t &spec,
                                 atom_selection_container_t atom_sel,
                                 coot::protein_geometry &geom) {
      graphical_bonds_container bonds_box_env;
      float environment_min_distance = 0.0f;
      float environment_max_distance = 3.5f;
      bool draw_bonds_to_hydrogens_flag = true;

      mmdb::Residue *residue_p = coot::util::get_residue(spec, atom_sel.mol);

      if (residue_p) {

         mmdb::PPAtom residue_atoms;
         int nResidueAtoms;
         residue_p->GetAtomTable(residue_atoms, nResidueAtoms);
         if (nResidueAtoms == 0) {
            std::cout << " something broken in atom residue selection in ";
            std::cout << "make_environment_bonds_box: got " << nResidueAtoms
                      << " atoms " << std::endl;
         } else {

            bool residue_is_water_flag = false;
            std::string residue_name = residue_p->GetResName();
            if (residue_name == "HOH" || residue_name == "WAT")
               residue_is_water_flag = 1;
            coot::protein_geometry geomt;
            Bond_lines_container bonds(atom_sel,residue_atoms, nResidueAtoms,
                                       &geom,
                                       residue_is_water_flag,
                                       draw_bonds_to_hydrogens_flag,
                                       environment_min_distance,
                                       environment_max_distance);
            bonds_box_env = bonds.make_graphical_bonds();
         }
      } else {
         std::cout << "ERROR:: NULL residue_p in make_environment_bonds_box() " << std::endl;
      }
      return bonds_box_env;
   };

   graphical_bonds_container bonds_box_env = make_bonds_box_env(spec, atom_sel, geom);
   return generic_3d_lines_bonds_box_t(bonds_box_env);

}


coot::simple::molecule_t
coot::molecule_t::get_simple_molecule(int imol, const std::string &residue_cid, bool draw_bonds_to_hydrogens_flag,
                                      coot::protein_geometry *geom_p) {

   coot::simple::molecule_t sm;
   mmdb::Residue *residue_p = cid_to_residue(residue_cid);
   if (residue_p) {
      sm = get_simple_molecule(imol, residue_p, draw_bonds_to_hydrogens_flag, geom_p);
   }
   return sm;
}


coot::simple::molecule_t
coot::molecule_t::get_simple_molecule(int imol, mmdb::Residue *residue_p, bool draw_hydrogen_atoms_flag,
                                      coot::protein_geometry *geom_p) {

   auto make_simple_molecule = [this] (const graphical_bonds_container &gbc) {
      bool against_a_dark_background = true;
      std::vector<glm::vec4> colour_table = this->make_colour_table(against_a_dark_background);
      coot::simple::molecule_t sm;
      for (int icol=0; icol<gbc.n_consolidated_atom_centres; icol++) {
         glm::vec4 col = colour_table[icol];
         for (unsigned int i=0; i<gbc.consolidated_atom_centres[icol].num_points; i++) {
            const graphical_bonds_atom_info_t &at_info = gbc.consolidated_atom_centres[icol].points[i];
            int fc = 0;
            bool arom = false;
            sm.add_atom(simple::atom_t(at_info.atom_p->GetAtomName(), at_info.atom_p->element, at_info.position, fc, arom));
         }
      }

      for (int icol=0; icol<gbc.num_colours; icol++) {
         glm::vec4 col = colour_table[icol];
         graphical_bonds_lines_list<graphics_line_t> &ll = gbc.bonds_[icol];
         for (int j=0; j<ll.num_lines; j++) {
            bool thin = ll.thin_lines_flag; // do something with this?
            simple::bond_t::bond_type_t bt = simple::bond_t::SINGLE_BOND;
            sm.add_bond(simple::bond_t(ll.pair_list[j].atom_index_1, ll.pair_list[j].atom_index_2, bt));
         }
      }
      return sm;
   };

   coot::simple::molecule_t sm;

   std::set<int> no_bonds_to_these_atoms; // empty
   mmdb::Manager *new_mol = util::create_mmdbmanager_from_residue(residue_p);
   atom_selection_container_t atom_sel = make_asc(new_mol);
   Bond_lines_container bonds(geom_p, no_bonds_to_these_atoms, draw_hydrogen_atoms_flag);
   bonds.do_colour_by_chain_bonds(atom_sel, false, imol, draw_hydrogen_atoms_flag, false, true, false, false);
   bonds_box = bonds.make_graphical_bonds();
   sm = make_simple_molecule(bonds_box);
   atom_sel.clear_up();

   return sm;
}
