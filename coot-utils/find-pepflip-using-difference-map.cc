/*
 * coot-utils/find-pepflip-using-difference-map.cc
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

#include "find-pepflip-using-difference-map.hh"

coot::pepflip_using_difference_map::pepflip_using_difference_map(mmdb::Manager *mol_in,
                                                                 const clipper::XMap<float> &xmap) {
                                                                    
   diff_map = xmap;
   mol = mol_in;
   
}


std::vector<coot::residue_spec_t>
coot::pepflip_using_difference_map::get_suggested_flips(float n_sigma) const {

   std::vector<residue_spec_t> rv;

   std::vector<flip_atom_triplet_t> triplets = get_peptide_atom_triplets();

   unsigned int n_others = 200;
   std::vector<std::pair<clipper::Coord_orth, clipper::Coord_orth> >
      randos = make_random_other_pairs(n_others);

   stats::single data;

   for (std::size_t i=0; i<randos.size(); i++) {
      const clipper::Coord_orth pt_1(randos[i].first);
      const clipper::Coord_orth pt_2(randos[i].second);
      float d1 = density_at_point(xmap, pt_1);
      float d2 = density_at_point(xmap, pt_2);
      float delta = d2-d1;
      data.add(delta);
   }
   float mean = data.mean();
   float sd   = sqrt(data.variance());
   float cut = mean + n_sigma * sd;

   for (std::size_t i=0; i<triplets.size(); i++) {
      flip_atom_triplet_t &t = triplets[i];
      clipper::Coord_orth pt_1 = t.current_O_position();
      clipper::Coord_orth pt_2 = t.flipped_O_position();
      float d1 = density_at_point(xmap, pt_1);
      float d2 = density_at_point(xmap, pt_2);
      float delta = d2-d1;
      if (delta > cut)
         rv.push_back(residue_spec_t(t.CA_this->residue));
   }

   return rv;
}
                                                                 
std::vector<coot::flip_atom_triplet_t>
coot::pepflip_using_difference_map::get_peptide_atom_triplets() const {
   
   // Plan:
   //
   // Find ~ 100 atom pairs random in the structure
   // and look at the difference map differences.
   // Find the mean and sd from them 

   std::vector<flip_atom_triplet_t> rps;

   int imod = 1;
   mmdb::Model *model_p = mol->GetModel(imod);
   if (model_p) {
      int n_chains = model_p->GetNumberOfChains();
      for (int ichain=0; ichain<n_chains; ichain++) {
         mmdb::Chain *chain_p = model_p->GetChain(ichain);
         int nres = chain_p->GetNumberOfResidues();
         for (int ires=0; ires<(nres-1); ires++) {
            mmdb::Residue *residue_this_p = chain_p->GetResidue(ires);
            mmdb::Residue *residue_next_p = chain_p->GetResidue(ires+1);
            std::string res_name_this = residue_this_p->GetResName();
            std::string res_name_next = residue_next_p->GetResName();
            int res_no_this = residue_this_p->GetSeqNum();
            int res_no_next = residue_next_p->GetSeqNum();
            if (res_no_next != (res_no_this + 1)) continue;
            if (is_standard_amino_acid_name(res_name_this)) {
               if (is_standard_amino_acid_name(res_name_next)) {
                  int n_atoms_this = residue_this_p->GetNumberOfAtoms();
                  mmdb::Atom *O_this = 0;
                  mmdb::Atom *CA_this = 0;
                  mmdb::Atom *CA_next = 0;
                  for (int iat=0; iat<n_atoms_this; iat++) {
                     mmdb::Atom *at = residue_p->GetAtom(iat);
                     if (! at->isTer()) {
                        std::string atom_name(at->GetAtomName());
                        if (atom_name == " O  ") {
                           std::string alt_loc(at->altLoc);
                           if (alt_loc == "") {
                              O_this = at;
                           }
                        }
                        if (atom_name == " CA ") {
                           std::string alt_loc(at->altLoc);
                           if (alt_loc == "") {
                              CA_this = at;
                           }
                        }
                     }
                  }
                  if (CA_this && O_this) {
                     // Find CA of next residue
                     int n_atoms_next = residue_next_p->GetNumberOfAtoms();
                     for (int iat=0; iat<n_atoms_next; iat++) {
                        mmdb::Atom *at = residue_next_p->GetAtom(iat);
                        if (! at->isTer()) {
                           std::string atom_name(at->GetAtomName());
                           if (atom_name == " CA ") {
                              std::string alt_loc(at->altLoc);
                              if (alt_loc == "") {
                                 CA_next = at;
                                 break;
                              }
                           }
                        }
                     }
                     if (CA_next) {
                        flip_atom_triplet_t p(CA_this, O_this, CA_next);
                        rps.push_back(p);
                     }
                  }
               }
            }
         }
      }
   }

   return rps;

}

clipper::Coord_orth
coot::flip_atom_triplet_t::current_O_position() const {

   clipper::Coord_orth co(O_this);
   return co;
}

clipper::Coord_orth
coot::flip_atom_triplet_t::flipped_O_position(const flip_atom_triplet_t &t) const {

   clipper::Coord_orth pt_CA_1(CA_this);
   clipper::Coord_orth pt_CA_2(CA_next);
   clipper::Coord_orth pt_O(O_this);

   clipper::Coord_orth dir = pt_CA_2 - pt_CA_1;
   clipper::Coord_orth pos = pt_O;
   clipper::Coord_orth os = pt_CA_1;
   double angle = M_PI;

   clipper::Coord_orth r = util:rotate_around_vector(dir, pos, os, angle);
   return r;

}

std::vector<std::pair<clipper::Coord_orth, clipper::Coord_orth> >
coot::pepflip_using_difference_map::make_random_other_pairs(unsigned int n_others) const {

   std::vector<std::pair<clipper::Coord_orth, clipper::Coord_orth> > v;

   if (mol) {
      mmdb::realtype max_dist = 3.0;
      mmdb::realtype min_dist = 4.0;
      int i_sel_hnd = mol->NewSelection(); // d
      mol->SelectAtoms (i_sel_hnd, 0, "*",
                        mmdb::ANY_RES, // starting resno, an int
                        "*", // any insertion code
                        mmdb::ANY_RES, // ending resno
                        "*", // ending insertion code
                        "*", // any residue name
                        "*", // atom name
                        "*", // elements
                        "*"  // alt loc.
                        );

      long i_contact_group = 1;
      mmdb::mat44 my_matt;
      mmdb::SymOps symm;
      for (int i=0; i<4; i++)
         for (int j=0; j<4; j++)
            my_matt[i][j] = 0.0;
      for (int i=0; i<4; i++) my_matt[i][i] = 1.0;

      mmdb::Atom **atom_selection = 0;
      int n_selected_atoms;
      mol->GetSelIndex(i_sel_hnd, atom_selection, n_selected_atoms);
      setup_env_residue_atoms_radii(i_sel_hnd); // fills neighb_atom_radius
      mmdb::Contact *pscontact = NULL;
      int n_contacts;
      mol->SeekContacts(atom_selection, n_selected_atoms,
                        atom_selection, n_selected_atoms,
                        0.01, max_dist,
                        0, // 0: in same residue also?
                        pscontact, n_contacts,
                        0, &my_matt, i_contact_group);
      if (n_contacts > 0) {

         // now make a filter. Choose a random number so that we select only about
         // n_others from these contacts, factor 2 because we filter on ii < jj
         // and n_contacts contacts contact symmetry
         //
         float filter = 1.0;
         if (n_contacts > n_others)
            filter = 2.0 * static_cast<int>(n_others)/static_cast<float>(n_contacts);
         float rmi = 1.0f/float(RAND_MAX);
         
         if (pscontact) {
            float f = coot::util::random() * rmi;
            if (f < filter) {
               for (int i=0; i<n_contacts; i++) {
                  const int &ii = pscontact[i].id1;
                  const int &jj = pscontact[i].id2;
                  if (ii < jj) {
                     mmdb::Atom *at_1 = atom_selection[ii];
                     mmdb::Atom *at_2 = atom_selection[jj];
                     clipper::Coord_orth pt_1 = co(at_1);
                     clipper::Coord_orth pt_2 = co(at_2);
                     std::pair<clipper::Coord_orth, clipper::Coord_orth> p(pt_1, pt_2);
                     v.push_back(p):
                  }
               }
            }
         }
      }
      mol->DeleteSelection(i_sel_hnd);
   }
   return v;
}
