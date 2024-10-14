/* ligand/base-pairing.cc
 * 
 * Copyright 2006 by The University of York
 * Copyright 2009 by The University of Oxford
 * Author: Paul Emsley
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
 * 02110-1301, USA.
 */

#include "coot-utils/coot-coord-utils.hh"
#include "ideal-rna.hh"
#include "base-pairing.hh"
#include "coot-utils/coot-lsq-types.hh"

mmdb::Residue *
coot::watson_crick_partner(mmdb::Residue *res_ref, mmdb::Manager *standard_residues) {

   mmdb::Residue *res = NULL;
   std::string nucleic_acid_type = "DNA";
   std::string form = "A"; // overriden later if nucleic_acid_type is "RNA";

   std::string resname = res_ref->GetResName();
   std::string seq;

   if (resname == "Gd") { 
      seq = "g";
      form = "B";
   } 
   if (resname == "Ad") { 
      seq = "a";
      form = "B";
   } 
   if (resname == "Td") { 
      seq = "t";
      form = "B";
   } 
   if (resname == "Cd") { 
      seq = "c";
      form = "B";
   }
   if (resname == "DG") { 
      seq = "g";
      form = "B";
   } 
   if (resname == "DA") { 
      form = "B";
      seq = "a";
   } 
   if (resname == "DT") { 
      seq = "t";
      form = "B";
   } 
   if (resname == "DC") { 
      seq = "c";
      form = "B";
   }
   if (resname == "Gr") { 
      nucleic_acid_type = "RNA";
      seq = "g";
   } 
   if (resname == "Ar") { 
      nucleic_acid_type = "RNA";
      seq = "a";
   } 
   if (resname == "Tr") { 
      nucleic_acid_type = "RNA";
      seq = "t";
   } 
   if (resname == "Cr") {
      nucleic_acid_type = "RNA";
      seq = "c";
   } 
   if (resname == "G") {
      nucleic_acid_type = "RNA";
      seq = "g";
   }
   if (resname == "A") { 
      nucleic_acid_type = "RNA";
      seq = "a";
   } 
   if (resname == "T") { 
      nucleic_acid_type = "RNA";
      seq = "t";
   } 
   if (resname == "C") {
      nucleic_acid_type = "RNA";
      seq = "c";
   } 
   if (resname == "U") {
      nucleic_acid_type = "RNA";
      seq = "u";
   } 
   if (resname == "Ur") { 
      nucleic_acid_type = "RNA";
      seq = "u";
   }

   if (nucleic_acid_type == "RNA")
      form = "A";

   if (seq == "") {
      std::cout << "blank sequence " << std::endl;
      return res; // NULL
   }
   
   coot::ideal_rna ir(nucleic_acid_type, form, 0, seq, standard_residues);
   ir.use_v3_names();
   std::cout << "::::::::::: nucleic_acid_type " << nucleic_acid_type << std::endl;
   std::cout << "::::::::::: nucleic_acid form " << form << std::endl;
   mmdb::Manager *mol = ir.make_molecule();

   clipper::RTop_orth rtop;
   bool rtop_is_good = 0;

   if (mol) { 
      int imod = 1;
      mmdb::Model *model_p = mol->GetModel(imod);
      mmdb::Chain *chain_p;
      // run over chains of the existing mol
      int nchains = model_p->GetNumberOfChains();
      if (nchains == 2) { 
	 chain_p = model_p->GetChain(0);
	 int nres = chain_p->GetNumberOfResidues();
	 if (nres == 1) { 
	    mmdb::Residue *res_moving = chain_p->GetResidue(0);
	    // we need to match res_moving onto res_ref atom by atom
	    std::pair<bool, clipper::RTop_orth> m =
	       coot::base_pair_match_matix(res_ref, res_moving);
	    if (m.first) { 
	       rtop = m.second;
	       rtop_is_good = 1;
	    } 
	 }
      } else {
	 std::cout << "oops - not 2 chains " << nchains << " for molecule "
		   << mol << "   and model " << model_p << std::endl;
      }

      if (rtop_is_good) {
	 coot::util::transform_mol(mol, rtop);

	 // mol->WritePDBASCII("debug.pdb");
	 
	 chain_p = model_p->GetChain(1);
	 int nres = chain_p->GetNumberOfResidues();
	 if (nres == 1) { 
	    res = chain_p->GetResidue(0);
	 }
      }
   }

   mmdb::Residue *rres = NULL;
   if (res) {
      rres = coot::util::deep_copy_this_residue(res);
   }
   delete mol;
   return rres;
} 


std::pair<bool, clipper::RTop_orth> 
coot::base_pair_match_matix(mmdb::Residue *res_ref, mmdb::Residue *res_mov) {
   
   std::string res_name = res_ref->GetResName();
   bool is_pyrimidine = 0;
   bool is_purine = 0;

   std::vector<std::string> purine_atom_names;
   std::vector<std::string> pyrimidine_atom_names;
   
   pyrimidine_atom_names.push_back(" N9 ");
   pyrimidine_atom_names.push_back(" C8 ");
   pyrimidine_atom_names.push_back(" N7 ");
   pyrimidine_atom_names.push_back(" C5 ");
   pyrimidine_atom_names.push_back(" C4 ");
   // 
   pyrimidine_atom_names.push_back(" N1 ");
   pyrimidine_atom_names.push_back(" C2 ");
   pyrimidine_atom_names.push_back(" N3 ");
   pyrimidine_atom_names.push_back(" C6 ");
   pyrimidine_atom_names.push_back(" N6 ");

   purine_atom_names.push_back(" N1 ");
   purine_atom_names.push_back(" C2 ");
   purine_atom_names.push_back(" N3 ");
   purine_atom_names.push_back(" C4 ");
   purine_atom_names.push_back(" C5 ");
   
   if (res_name == "G" || res_name == "Gd" || res_name == "Gr")
      is_pyrimidine = 1;
   if (res_name == "A" || res_name == "Ad" || res_name == "Ar")
      is_pyrimidine = 1;
   if (res_name == "DA" || res_name == "DG")
      is_pyrimidine = 1;

   if (res_name == "T" || res_name == "Td" || res_name == "Tr")
      is_purine = 1;
   if (res_name == "C" || res_name == "Cd" || res_name == "Cr")
      is_purine = 1;
   if (res_name == "U" || res_name == "Ud" || res_name == "Ur")
      is_purine = 1;
   if (res_name == "DT" || res_name == "DC" || res_name == "DT")
      is_purine = 1;

   std::vector<std::string> base_atom_names;

   if (is_pyrimidine)
      base_atom_names = pyrimidine_atom_names;
   if (is_purine)
      base_atom_names = purine_atom_names;

   clipper::Mat33<double> m_dum(1,0,0,0,1,0,0,0,1);
   clipper::Coord_orth pt_dum(0,0,0);
   clipper::RTop_orth rtop(m_dum, pt_dum);
   bool rtop_is_good = 0;
   if (base_atom_names.size() == 0) {
      std::cout << "  Oops neither pyrimidine nor purine" << std::endl;
   } else {

      std::vector<clipper::Coord_orth> ref_pts;
      std::vector<clipper::Coord_orth> mov_pts;
      int found_atoms = 0;
      mmdb::PAtom *ref_atoms = NULL;
      mmdb::PAtom *mov_atoms = NULL;
      int n_ref_atoms = 0;
      int n_mov_atoms = 0;
      res_ref->GetAtomTable( ref_atoms, n_ref_atoms);
      res_mov->GetAtomTable( mov_atoms, n_mov_atoms);
      for (unsigned int i=0; i<base_atom_names.size(); i++) {
	 mmdb::Atom *at_ref = NULL;
	 mmdb::Atom *at_mov = NULL;
	 for (int iref=0; iref<n_ref_atoms; iref++) {
	    std::string ref_atom_name = ref_atoms[iref]->name;
	    if (ref_atom_name == base_atom_names[i]) { 
	       at_ref = ref_atoms[iref];
	       break;
	    } 
	 } 
	 for (int imov=0; imov<n_mov_atoms; imov++) {
	    std::string mov_atom_name = mov_atoms[imov]->name;
	    if (mov_atom_name == base_atom_names[i]) { 
	       at_mov = mov_atoms[imov];
	       break;
	    } 
	 }
	 if (at_ref && at_mov) {
	    ref_pts.push_back(clipper::Coord_orth(at_ref->x, at_ref->y, at_ref->z));
	    mov_pts.push_back(clipper::Coord_orth(at_mov->x, at_mov->y, at_mov->z));
	 }
      }

      if (ref_pts.size() > 3) {
	 rtop = clipper::RTop_orth(mov_pts, ref_pts);
	 rtop_is_good = 1;

      } else {
	 std::cout << "   WARNING:: base_pair_match_matrix found only "
		   << ref_pts.size() << " matching points in the base " << std::endl;
      } 
   } 
   return std::pair<bool, clipper::RTop_orth> (rtop_is_good, rtop);
} 
