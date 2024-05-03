/*
 * ideal/refinement-results-t.hh
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
 * General Public License for more details.
 *
 * You should have received a copies of the GNU General Public License and
 * the GNU Lesser General Public License along with this program; if not,
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA, 02110-1301, USA.
 * See http://www.gnu.org/licenses/
 */


#ifndef REFINEMENT_RESULTS_T_HH
#define REFINEMENT_RESULTS_T_HH

#include <vector>
#include <map>
#include "refinement-lights.hh"

#include <clipper/core/coords.h>
#include "geometry/residue-and-atom-specs.hh"

namespace coot {

   // ---------------------------------------------------------------
   // ---------------------------------------------------------------
   //     class refinement_results_t, helper class for sending text
   //     results back to invoking function.  Returned by minimize()
   //     function.
   // ---------------------------------------------------------------
   // ---------------------------------------------------------------

   class refinement_results_for_rama_t {
   public:
      float ball_pos_x, ball_pos_y, ball_pos_z; // include file fun!
      atom_spec_t atom_spec_CA;
      float distortion;
      refinement_results_for_rama_t(mmdb::Atom *at_1,
                                    mmdb::Atom *at_2,
                                    mmdb::Atom *at_3,
                                    mmdb::Atom *at_4,
                                    mmdb::Atom *at_5,
                                    float distortion_in);
   };

   class refinement_results_nbc_baddie_t {
   public:
      atom_spec_t atom_spec_1;
      atom_spec_t atom_spec_2;
      clipper::Coord_orth atom_1_pos;
      clipper::Coord_orth atom_2_pos;
      clipper::Coord_orth mid_point;
      float score;
   };

   class refinement_results_for_chiral_t {
   public:
      atom_spec_t atom_spec;
      clipper::Coord_orth atom_pos;
      float distortion;
      refinement_results_for_chiral_t(const atom_spec_t &spec, const clipper::Coord_orth &p, float s) :
         atom_spec(spec), atom_pos(p), distortion(s) {}
   };

   // to animate the bad NBC contacts removal, I need to know if a contact has
   // disappeared, so I want to store a list of bad NBC contacts which I can
   // use for round-to-round comparison to see if any have disappeared.
   // std::map<mmdb::Atom *, mmdb::Atom *> - I  don't need to check both ways
   // round. Actually, I can do this with indices, can't I?
   // std::map<int, std::vector<int> > nbc_baddies_atom_index_map

   class refinement_results_t {
   public:

      bool found_restraints_flag; // if we found restraints or not.
      int progress; // GSL_CONTINUE, GSL_SUCCESS, GSL_ENOPROG (no progress)
      std::string info_text;
      int n_restraints;
      std::vector<refinement_lights_info_t> lights;
      bool refinement_results_contain_overall_nbc_score;
      bool refinement_results_contain_overall_rama_plot_score;
      float overall_nbc_score;
      // std::vector<std::pair<atom_spec_t, float> > sorted_nbc_baddies;
      std::vector<refinement_results_nbc_baddie_t> sorted_nbc_baddies;
      std::map<int, std::vector<int> > nbc_baddies_atom_index_map;
      std::vector<std::pair<int, int> > hydrogen_bond_atom_index_vec;
      float overall_rama_plot_score;
      std::vector<std::pair<atom_spec_t, float> > sorted_rama_baddies;
      std::vector<refinement_results_for_rama_t> all_ramas;
      float overall_atom_pull_score;
      std::vector<std::pair<atom_spec_t, float> > sorted_atom_pulls; // all of them
      std::vector<refinement_results_for_chiral_t> sorted_chiral_volume_baddies;

      refinement_results_t(bool frf, int prog_in,
                           const std::vector<refinement_lights_info_t> &lights_in) {
         init();
         found_restraints_flag = frf;
         progress = prog_in;
         lights = lights_in;
     }
      refinement_results_t(bool frf, int prog_in, const std::string &info_in) {
         init();
         found_restraints_flag = frf;
         info_text = info_in;
         progress = prog_in;
      }
      refinement_results_t() {
         init();
      }
      explicit refinement_results_t(const std::string &s_in) {
         init();
         info_text = s_in;
         found_restraints_flag = false;
         progress = -1; // unset
      }
      void init() {
         info_text = "";
         found_restraints_flag = false;
         refinement_results_contain_overall_nbc_score = false;
         refinement_results_contain_overall_rama_plot_score = false;
         overall_nbc_score = 0.0;
         overall_rama_plot_score = 0.0;
         overall_atom_pull_score = 0.0;
         progress = 27; // GSL_ENOPROG
         n_restraints = -1; // unset
      }
      void show() const;
      std::pair<bool, std::string> hooray() const;
   };
}

#endif // REFINEMENT_RESULTS_T_HH
