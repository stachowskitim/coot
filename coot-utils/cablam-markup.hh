/*
 * coot-utils/cablam-markup.hh
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


#include <vector>
#include <utility>

#include <clipper/core/coords.h>

#include "geometry/residue-and-atom-specs.hh"

namespace coot {

   class cablam_markup_t {
      // don't make invalid markups - check
      // the atoms before constructing this object
   public:
      clipper::Coord_orth O_prev_pos;
      clipper::Coord_orth O_this_pos;
      clipper::Coord_orth O_next_pos;
      clipper::Coord_orth CA_proj_point_prev;
      clipper::Coord_orth CA_proj_point_this;
      clipper::Coord_orth CA_proj_point_next;
      mmdb::Residue *residue;  // pointer to the central residue
      double score;

      cablam_markup_t() { score = -1; residue = 0;}
      cablam_markup_t(mmdb::Atom *O_prev_at,
                      mmdb::Atom *O_this_at,
                      mmdb::Atom *O_next_at,
                      mmdb::Atom *CA_prev_at,
                      mmdb::Atom *CA_this_at,
                      mmdb::Atom *CA_next_at,
                      mmdb::Atom *CA_next_next_at);
   };

   class cablam_like_geometry_stats_t {
   public:
      mmdb::Residue *residue; // central residue
      double dp_prev_to_mid;
      double dp_next_to_mid;
      double dist_proj_point_prev_to_next;
      cablam_like_geometry_stats_t(const cablam_markup_t &cablam_markup);
   };

   // 20220116-PE  for generating distibution data (many PDB files)
   //
   std::vector<cablam_like_geometry_stats_t> get_cablam_like_geometry_stats(mmdb::Manager *mol);
   // which uses calc_cablam - a valid results has a residue data item that's not null.
   cablam_markup_t calc_cablam(mmdb::Chain *chain_p, mmdb::Residue *residue_this_p,
                               int ires, double score=0.0); // score is optional because when we
                                                            // are *generating* stats, we don't have it.


   // for representation from cablam log
   std::vector<cablam_markup_t>
   make_cablam_markups(const std::vector<std::pair<residue_spec_t, double> > &residues,
                       mmdb::Manager *mol);

   // parse this log file and call the above function for each cablam outlier residue
   std::vector<cablam_markup_t>
   make_cablam_markups(mmdb::Manager *mol, const std::string &cablam_output_file_name);
}
