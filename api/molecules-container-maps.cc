/*
 * api/molecules-container-maps.cc
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

#include "molecules-container.hh"

// This function does no normalisztion of the scales,
// presuming that they are pre-normalized.
// @return the index of the new map, or -1 on failure.
int
molecules_container_t::average_map(const std::string &imol_maps_string, std::vector<float> &scales) {

   int imol_new = -1;
   if (! scales.empty()) {
      std::vector<std::string> number_strings = coot::util::split_string(imol_maps_string, ":");
      std::vector<std::pair<int, float> > weights_and_maps;
      unsigned int scales_index = 0;
      for (const auto &item : number_strings) {
         int idx = coot::util::string_to_int(item);
         if (is_valid_map_molecule(idx)) {
            if (scales_index < scales.size()) {
               weights_and_maps.push_back(std::make_pair(idx, scales[scales_index]));
               scales_index++;
            }
         }
      }

      if (weights_and_maps.size() == scales.size()) {
         // now check that the grids are the same
         int idx = weights_and_maps[0].first;
         clipper::Grid_sampling gs_ref = molecules[idx].xmap.grid_sampling();
         unsigned int n_match = 0;
         for (unsigned int i=0; i<weights_and_maps.size(); i++) {
            idx = weights_and_maps[i].first;
            bool match = false;
            clipper::Grid_sampling gs_this = molecules[idx].xmap.grid_sampling();
            if (gs_ref.nu() == gs_this.nu())
               if (gs_ref.nv() == gs_this.nv())
                  if (gs_ref.nw() == gs_this.nw())
                     match = true;
            if (match) n_match++;
         }
         if (n_match == weights_and_maps.size()) {
            idx = weights_and_maps[0].first;
            std::string name = "Average map " + imol_maps_string;
            bool is_em_map = molecules[idx].is_EM_map();
            imol_new = molecules.size();
            std::vector<std::pair<clipper::Xmap<float>, float> > maps_and_weights(scales.size());
            // heavyweight stuff going on here
            for (unsigned int i=0; i<weights_and_maps.size(); i++) {
               idx = weights_and_maps[i].first;
               std::pair<clipper::Xmap<float>, float> p(molecules[idx].xmap, weights_and_maps[i].second);
               maps_and_weights[i] = p;
            }
            for (unsigned int i=0; i<weights_and_maps.size(); i++) {
	      std::cout << "debug map and weight " << weights_and_maps[i].first << " "
			<< weights_and_maps[i].second << "\n";
	    }
            clipper::Xmap<float> xmap_new = coot::util::average_map(maps_and_weights);
            molecules.push_back(coot::molecule_t(name, imol_new, xmap_new, is_em_map));
         }
      }
   }
   return imol_new;
}

// This function does no normalisztion of the scales, presuming that they are pre-normalized.
// @return success status
bool
molecules_container_t::regen_map(int imol_map, const std::string &imol_maps_string, const std::vector<float> &scales) {

   bool status = false;
   // molecules.push_back(coot::molecule_t(name, imol_new, xmap_ref, is_em_map));

   if (is_valid_map_molecule(imol_map)) {
      if (! scales.empty()) {
         std::vector<std::string> number_strings = coot::util::split_string(imol_maps_string, ":");
         std::vector<std::pair<clipper::Xmap<float> *, float> > maps_and_scales_vec;
         unsigned int scales_index = 0;
         for (const auto &item : number_strings) {
            int idx = coot::util::string_to_int(item);
            if (is_valid_map_molecule(idx)) {
               if (scales_index < scales.size()) {
                  maps_and_scales_vec.push_back(std::make_pair(&molecules[idx].xmap, scales[scales_index]));
                  scales_index++;
               }
            }
         }

         if (maps_and_scales_vec.size() == scales.size()) {
            coot::util::regen_weighted_map(&molecules[imol_map].xmap, maps_and_scales_vec);
            status = true;
         }
      }
   }

   return status;
}

texture_as_floats_t
molecules_container_t::get_map_section_texture(int imol, int section_index, int axis,
                                               float data_value_for_bottom, float data_value_for_top) const {

   texture_as_floats_t t;
   if (is_valid_map_molecule(imol)) {
       t = molecules[imol].get_map_section_texture(section_index, axis, data_value_for_bottom, data_value_for_top);
   }
   return t;
}

//! @return the number of section in the map along the give axis.
//! (0 for X-axis, 1 for y-axis, 2 for Z-axis).
//! return -1 on failure.
int
molecules_container_t::get_number_of_map_sections(int imol_map, int axis_id) const {

   int n = -1;
   if (is_valid_map_molecule(imol_map)) {
      n = molecules[imol_map].get_number_of_map_sections(axis_id);
   } else {
      std::cout << "WARNING:: " << __FUNCTION__ << "(): not a valid model molecule " << imol_map << std::endl;
   }
   return n;
}

#include "coot-utils/xmap-stats.hh"
#include "coot-utils/q-score.hh"

//! Get the Pintile et al. Q Score
//!
//! @return a coot::validation_information_t object
coot::validation_information_t
molecules_container_t::get_q_score(int imol_model, int imol_map) const {

   // The Q score is stored as a UDD per atom.
   //
   // We need to extract the Q Score for each atom and average them
   // to make scores for each residue

   coot::validation_information_t vi;
   bool do_per_atom = false; // 20240629-PE we currently don't have a (good)_container for this.
                             //  Per residue will do for now.

   if (is_valid_model_molecule(imol_model)) {
      if (is_valid_map_molecule(imol_map)) {
         const clipper::Xmap<float> &xmap = molecules[imol_map].xmap;
         mean_and_variance<float> mv = map_density_distribution(xmap, 1000, false, true);
         mmdb::Manager *mol = molecules[imol_model].atom_sel.mol;
         coot::q_score_t q_score(mol);
         q_score.calc(xmap, mv.mean, std::sqrt(mv.variance));

         int udd_q_score = mol->GetUDDHandle(mmdb::UDR_ATOM, "Q Score");
         int imod = 1;
         mmdb::Model *model_p = mol->GetModel(imod);
         if (model_p) {
            int n_chains = model_p->GetNumberOfChains();
            for (int ichain=0; ichain<n_chains; ichain++) {
               mmdb::Chain *chain_p = model_p->GetChain(ichain);
               coot::chain_validation_information_t cvi(chain_p->GetChainID());
               int n_res = chain_p->GetNumberOfResidues();
               for (int ires=0; ires<n_res; ires++) {
                  mmdb::Residue *residue_p = chain_p->GetResidue(ires);
                  if (residue_p) {
                     int n_atoms = residue_p->GetNumberOfAtoms();

                     if (n_atoms > 0) {
                        coot::residue_spec_t residue_spec(residue_p);
                        coot::atom_spec_t spec_for_atom_in_residue(residue_p->GetAtom(0)); // say
                        double qed_residue_sum = 0;
                        unsigned int n_qed_atoms = 0;

                        // accumulate QED for the residue
                        for (int iat=0; iat<n_atoms; iat++) {
                           mmdb::Atom *at = residue_p->GetAtom(iat);
                           if (! at->isTer()) {
                              mmdb::realtype q;
                              if (at->GetUDData(udd_q_score, q) == mmdb::UDDATA_Ok) {
                                 if (q > -1000.0) { // test for not invalid QED on atom
                                    qed_residue_sum += q;
                                    n_qed_atoms++;
                                 }
                              }
                           }
                        }

                        if (n_qed_atoms > 0) {
                           double qed_residue = qed_residue_sum / static_cast<double>(n_qed_atoms);
                           std::string fs = coot::util::float_to_string_using_dec_pl(qed_residue, 2);
                           std::string label = residue_spec.chain_id + " " + std::to_string(residue_spec.res_no) + " " + fs;
                           coot::residue_validation_information_t rvi(residue_spec, spec_for_atom_in_residue, qed_residue, label);
                           cvi.add(rvi);

                           if (do_per_atom) {
                              // now the per-atom score
                              for (int iat=0; iat<n_atoms; iat++) {
                                 mmdb::Atom *at = residue_p->GetAtom(iat);
                                 if (! at->isTer()) {
                                    mmdb::realtype q;
                                    at->GetUDData(udd_q_score, q);
                                    if (false)
                                       std::cout << " " << coot::atom_spec_t(at) << " B " << at->tempFactor
                                                 << "  Q-Score: " << q << std::endl;
                                 }
                              }
                           }
                        }
                     }
                  }
               }
               vi.add(cvi);
            }
         }
         q_score.close();
      }
   }
   return vi;
}

//! Make a FSC-scaled map
//!
//! @return the molecule index of the new map
int
molecules_container_t::make_power_scaled_map(int imol_ref, int imol_map_for_scaling) {

  int idx = -1;
  if (is_valid_map_molecule(imol_ref)) {
    if (is_valid_map_molecule(imol_map_for_scaling)) {
      clipper::Xmap<float> &xmap_ref   = molecules[imol_ref].xmap;
      clipper::Xmap<float> &xmap_other = molecules[imol_map_for_scaling].xmap;
      clipper::Xmap<float> scaled = coot::util::power_scale(xmap_ref, xmap_other);
      bool is_em_map = molecules[idx].is_EM_map();
      int imol_new = molecules.size();
      std::string name = std::string("Copy of map ") + std::to_string(imol_map_for_scaling) + " scaled to " + std::to_string(imol_ref);
      molecules.push_back(coot::molecule_t(name, imol_new, scaled, is_em_map));
      idx = imol_new;
    }
  }
  return idx;
}


std::vector<int>
molecules_container_t::partition_map_by_chain(int imol_map, int imol_model) {

   std::vector<int> v;
   if (is_valid_map_molecule(imol_map)) {
      if (is_valid_model_molecule(imol_model)) {
         const clipper::Xmap<float> &xmap = molecules[imol_map].xmap;
         mmdb::Manager *mol = molecules[imol_model].atom_sel.mol;
         std::string state_string;
         std::vector<std::pair<std::string, clipper::Xmap<float> > > maps_info =
            coot::util::partition_map_by_chain(xmap, mol, &state_string);
         if (! maps_info.empty()) {
            bool is_em_map = molecules[imol_map].is_EM_map();
            for (const auto &mi : maps_info) {
               std::string chain_id = mi.first;
               const clipper::Xmap<float> &xmap_new = mi.second;
               int imol_for_map = molecules.size();
               std::string label = "Partitioned map Chain " + chain_id;
               molecules.push_back(coot::molecule_t(label, imol_for_map, xmap_new, is_em_map));
               v.push_back(imol_for_map);
            }
         }
      }
   }
   return v;
}


//! make a masked map
//!
//! @return the index of the newly created mask. Return -1 on failure.
int
molecules_container_t::make_mask(int imol_map_ref, int imol_model, const std::string &atom_selection_cid, float radius) {

   int imol_map_new = -1;
   if (is_valid_map_molecule(imol_map_ref)) {
      if (is_valid_model_molecule(imol_model)) {
         const clipper::Xmap<float> &xmap = molecules[imol_map_ref].xmap;
         mmdb::Manager *mol = molecules[imol_model].atom_sel.mol;
         clipper::Cell cell = xmap.cell();
         clipper::Spacegroup spacegroup = xmap.spacegroup();
         clipper::Grid_sampling gs = xmap.grid_sampling();

         int selhandle = mol->NewSelection(); // d
         mol->Select(selhandle, mmdb::STYPE_ATOM, atom_selection_cid.c_str(), mmdb::SKEY_NEW);
         clipper::Xmap<float> xmap_new = coot::util::make_map_mask(spacegroup, cell, gs, mol, selhandle, radius, 1.0f);
         mol->DeleteSelection(selhandle);

         imol_map_new = molecules.size();
         std::string label = "Mask created by selection " + atom_selection_cid;
         bool is_em_map = molecules[imol_map_ref].is_EM_map();
         molecules.push_back(coot::molecule_t(label, imol_map_new, xmap_new, is_em_map));
      }
   }
   return imol_map_new;

}
