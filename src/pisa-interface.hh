/*
 * src/pisa-interface.hh
 *
 * Copyright 2010 by University of York
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

#ifndef HAVE_PISA_INTERFACE_HH
#define HAVE_PISA_INTERFACE_HH

#ifndef HAVE_STRING
#define HAVE_STRING
#include <string>
#endif // HAVE_STRING


namespace coot {

   class pisa_residue_t {
      int ser_no;
      int seq_num;
      std::string ins_code;
      float asa;
      float bsa;
      float solv_en;
      residue_spec_t residue_spec(const std::string &chain_id) const;
   };

   class pisa_molecule_t {
   public:
      int id;
      std::string chain_id;
      std::string molecule_class;
      int symop_no;
      int natoms;
      int nres;
      float area;
      float solv_en;
      double pvalue;
      std::vector<pisa_residue_t> residues;
      mmdb::Manager *mol;
   };

   class pisa_interface_t {
   public:
      int imol_1;
      int imol_2;
      clipper::Coord_orth centre;
      int n_h_bonds;
      int n_salt_bridges;
      int n_cov_bonds;
      int n_ss_bonds;
      float interface_area;
      float interface_solv_en;
      float interface_pvalue;
      float interface_stab_en;
      float asa;
      std::string chain_id_1; 
      std::string chain_id_2; 
      std::string symop;
      pisa_interface_t(int i1, int i2,
		       const std::string &chain_id_1_in,
		       const std::string &chain_id_2_in,
		       const std::string &symop_str,
		       const clipper::Coord_orth &centre_in,
		       float interface_area_in,
		       float interface_solv_en_in,
		       float interface_pvalue_in,
		       float interface_stab_en_in,
		       int n_h_bonds_in, int n_salt_bridges_in, int n_cov_bonds_in,
		       int n_ss_bonds_in) {
	 imol_1 = i1;
	 imol_2 = i2;
	 chain_id_1 = chain_id_1_in;
	 chain_id_2 = chain_id_2_in;
	 symop = symop_str;
	 n_h_bonds = n_h_bonds_in;
	 n_salt_bridges = n_salt_bridges_in;
	 n_ss_bonds = n_ss_bonds_in;
	 n_cov_bonds = n_cov_bonds_in;
	 interface_pvalue  = interface_pvalue_in;
	 interface_stab_en = interface_stab_en_in;
	 interface_area    = interface_area_in;
	 interface_solv_en = interface_solv_en_in;
	 centre = centre_in;
      }
   };

   void pisa_interfaces_gui(const std::vector<pisa_interface_t> &gui_info);
#if (GTK_MAJOR_VERSION > 1)
   void add_pisa_interfaces_cell_renderer(GtkTreeView *tree_view,
					  const std::string &column_title,
					  int pos);
   void on_pisa_interfaces_seletion_changed(GtkTreeSelection *treeselection,
					    gpointer          user_data);

      
#endif   

}

#endif // HAVE_PISA_INTERFACE_HH
