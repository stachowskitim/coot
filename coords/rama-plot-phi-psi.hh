/*
 * coords/rama-plot-phi-psi.hh
 * 
 * Copyright 2022 by Medical Research Council
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
 */


#ifndef RAMA_PLOT_PHI_PSI_HH
#define RAMA_PLOT_PHI_PSI_HH

#include <map>
#include <string>
#include <mmdb2/mmdb_manager.h>
#include <clipper/core/ramachandran.h>
#include "geometry/residue-and-atom-specs.hh"

// #include <goocanvas.h>

namespace rama_plot {

   // yet another phi,psi container

   class phi_psi_t {
      void init() {
         residue_prev = 0;
         residue_this = 0;
         residue_next = 0;
         res_no = 0;
         is_pre_pro = false;
         // item = 0;
         imol = -1;
         type = clipper::Ramachandran::All;
      }
   public:
      double phi; // in degrees
      double psi; // in degrees
      bool is_pre_pro;
      std::string label;
      int imol;
      std::string residue_name;
      std::string chain_id;
      int res_no;
      std::string ins_code;
      mmdb::Residue *residue_prev;
      mmdb::Residue *residue_this;
      mmdb::Residue *residue_next;
      // GooCanvasItem *item;
      clipper::Ramachandran::TYPE type;
      phi_psi_t(const float &phi_in, const float &psi_in) : phi(phi_in), psi(psi_in) {
         init();
      }
      // this can throw an exception (e.g. bonding atoms too far
      // apart).  Uses get_phi_psi() below
      phi_psi_t(mmdb::Residue *prev_res, mmdb::Residue *this_res, mmdb::Residue *next_res);
      phi_psi_t() : phi(0), psi(0) { init(); }
      phi_psi_t(double phi, double psi, const std::string &res_name, const std::string &label,
                int res_no, const std::string &ins_code, const std::string &chain_id_in, bool is_pre_pro);
      float get_phi() const { return phi; }
      float get_psi() const { return psi; }
      std::string get_label() const { return label; }
      std::string get_residue_name() const { return residue_name; }
      void update_self(); // atoms are moved, update phi and psi
   };

   namespace util {
      std::pair<bool, phi_psi_t> get_phi_psi(mmdb::Residue *prev, mmdb::Residue *this_res, mmdb::Residue *next);
   }

   class phi_psis_for_model_t {

      void generate_phi_psis(mmdb::Manager *mol_in);
   
   public:
      int model_number;
      std::map<coot::residue_spec_t, phi_psi_t> phi_psi;
      explicit phi_psis_for_model_t(int model_number_in) : model_number(model_number_in)  {}
      explicit phi_psis_for_model_t(mmdb::Manager *mol) {
         generate_phi_psis(mol);
      }
      void add_phi_psi(const coot::residue_spec_t &spec, const phi_psi_t &phi_psi_in) {
	 phi_psi[spec] = phi_psi_in;
      }
      phi_psi_t operator[](const coot::residue_spec_t &spec) {
 	 return phi_psi[spec];
      }
      unsigned int size() { return phi_psi.size(); } 
   };

}


#endif // RAMA_PLOT_PHI_PSI_HH
