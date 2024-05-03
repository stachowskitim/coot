/* ligand/ideal-rna.cc
 * 
 * Copyright 2006 The University of York
 * Copyright 2009 by the University of Oxford
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include <iostream>

#include "coot-utils/coot-coord-utils.hh"
#include "ideal-rna.hh"

// The sequence passed must be in lower case.
// 
coot::ideal_rna::ideal_rna(const std::string &RNA_or_DNA, const std::string &form,
			   short int single_stranged_flag_in,
			   const std::string &sequence, mmdb::Manager *standard_residues_in) {

   RNA_or_DNA_ = RNA_or_DNA;
   form_ = form;
   seq = sequence;
   standard_residues = standard_residues_in;
   single_stranged_flag = single_stranged_flag_in;
   use_standard_refmac_names = 1; // Ad, Gr etc

} 

// return a null pointer on something bad happened.
mmdb::Manager *
coot::ideal_rna::make_molecule() {

   mmdb::Manager *mol = 0; 
   mmdb::Residue *ur;
   bool is_dna_flag;
   coot::ideal_rna::form_t form_flag = A_FORM;

   if (seq.length() == 0)
      return mol; // NULL

   if (RNA_or_DNA_ == "RNA") { 
      // in a special place
      is_dna_flag = 0;
   } else {
      is_dna_flag = 1;
   }

   if (form_ == "B") { 
      ur = get_standard_residue_instance("DG", standard_residues);


      if (! is_dna_flag) {
	 // Add a O2' to RNA
	 // 
	 std::string new_atom_name = " O2'";
	 std::vector<std::string> at; // reference atom names
	 add_o2_prime(ur);
      }
      form_flag = B_FORM;
   } else {
      form_flag = A_FORM;
      ur = get_standard_residue_instance("G", standard_residues);
      if (is_dna_flag) {
	 delete_o2_prime(ur);
      }
   }

   if (ur) {

      mol = new mmdb::Manager;
      mmdb::Model *model_p = new mmdb::Model;
      mmdb::Chain *sense_chain_p = new mmdb::Chain;
      sense_chain_p->SetChainID("A");
      model_p->AddChain(sense_chain_p);

      clipper::Mat33<double> antisense_base_mat(1, 0, 0, 0, -1, 0, 0, 0, -1);
      clipper::RTop_orth antisense_base_rtop(antisense_base_mat, clipper::Coord_orth(0,0,0));
      mmdb::Residue *antisense_ref = coot::util::deep_copy_this_residue(ur);
      // now transform antisense base to the right place:
      coot::util::transform_atoms(antisense_ref, antisense_base_rtop);
      for(unsigned int iseq=0; iseq<seq.length(); iseq++) {
	 if (is_valid_base(seq[iseq])) {
	    // sense residue
	    mmdb::Residue *res = coot::util::deep_copy_this_residue(ur);
	    res->seqNum = 1 + iseq ;
	    clipper::RTop_orth o = n_turns(iseq, seq.length(), form_flag);
	    coot::util::transform_atoms(res, o);
	    int success = mutate_res(res, seq[iseq], is_dna_flag);
	    if (success) { 
	       sense_chain_p->AddResidue(res);
	       if (! use_standard_refmac_names) { // "standard" means v2
		  fix_up_residue_and_atom_names(res, is_dna_flag);
	       } 
	    }
	 } else {
	    std::cout << "ERROR:: oops not a valid base sequence-position: " << iseq
		      << " type: " << seq[iseq] << std::endl;
	 }
      }

      if (! single_stranged_flag) { 
	 mmdb::Chain *antisense_chain_p = new mmdb::Chain;
	 // People complain that the resdiues of the antisense chain
	 // appear in the pdb file in the wrong order (high residue
	 // numbers first). So lets just make a vector of them, push
	 // them back and add them backwards at the end.
	 //
	 std::vector<mmdb::Residue *> residues_v;
	 antisense_chain_p->SetChainID("B");

	 model_p->AddChain(antisense_chain_p);
	 for(unsigned int iseq=0; iseq<seq.length(); iseq++) {
	    if (is_valid_base(seq[iseq])) { 

	       // antisense residue
	       mmdb::Residue *res = coot::util::deep_copy_this_residue(antisense_ref);
	       res->seqNum = seq.length() - iseq;
	       // antisense_chain_p->AddResidue(res);  "backwards in pdb"
	       clipper::RTop_orth o = n_turns(iseq, seq.length(), form_flag);
	       coot::util::transform_atoms(res, o);
	       char complimentary_base = antisense_base(seq[iseq], is_dna_flag);
	       if (complimentary_base) {
		  int success = mutate_res(res, complimentary_base, is_dna_flag);
		  if (success) { 
		     residues_v.push_back(res);  // modern
		  }
	       }
	    }
	 }
 	 if (residues_v.size() > 0) {
	    // test must fail, unsigned is bad then
 	    for (int i=int(residues_v.size()-1); i>=0; i--) {
 	       antisense_chain_p->AddResidue(residues_v[i]);
	       if (! use_standard_refmac_names) {
		  fix_up_residue_and_atom_names(residues_v[i], is_dna_flag);
	       } 
 	    }
 	 } else {
	    std::cout << "WARNING:: OOpps no backwards residues " << std::endl;
	 }
      }
      mol->AddModel(model_p);
      mol->FinishStructEdit();
      mol->PDBCleanup(mmdb::PDBCLEAN_SERIAL|mmdb::PDBCLEAN_INDEX);
      
   } else {
      std::cout << "WARNING:: Bad standard residue Ur/Td" << std::endl;
   }

   return mol;
}

// Fix up to v3 names, that is.
void
coot::ideal_rna::fix_up_residue_and_atom_names(mmdb::Residue *residue_p, bool is_dna_flag) { 

   std::string res_name = residue_p->GetResName();
   std::string new_name = res_name; // residue_name_old_to_new(res_name, is_dna_flag); no longer needed

   residue_p->SetResName(new_name.c_str());

   mmdb::PPAtom residue_atoms = 0;
   int n_residue_atoms;
   residue_p->GetAtomTable(residue_atoms, n_residue_atoms);
   for (int iat=0; iat<n_residue_atoms; iat++) {
      mmdb::Atom *at = residue_atoms[iat];
      std::string atom_name = at->name;
      if (atom_name.length() > 3) {
	 if (atom_name[3] == '*') { 
	    atom_name[3] = '\'';
	    at->SetAtomName(atom_name.c_str());
	 }

	 if (atom_name == " O1P") at->SetAtomName(" OP1");
	 if (atom_name == " O2P") at->SetAtomName(" OP2");
      }
   }

   // fix the atom name C5M->C7 on a T in DNA [Grr, !@#$!@#$% PDB...]
   if (new_name == "DT") {
      for (int iat=0; iat<n_residue_atoms; iat++) {
	 mmdb::Atom *at = residue_atoms[iat];
	 std::string atom_name = at->name;
	 if (atom_name == " C5M") {
	    at->SetAtomName(" C7 ");
	 }
      }
   }

}



clipper::RTop_orth
coot::ideal_rna::n_turns(int nbase, int n_in_chain, coot::ideal_rna::form_t form_flag) const {

   clipper::Mat33<double> m_identity(1, 0, 0, 0, 1, 0, 0, 0, 1);
   clipper::Coord_orth t_identity(0, 0, 0);
   clipper::RTop_orth o(m_identity, t_identity);

   // A form
   clipper::Mat33<double> base_mat(0.8415, -0.5402, 0,
				   0.5402, 0.8415, 0,
				   0, 0, 1);
   clipper::Coord_orth base_trans(0, 0, 2.81);
   clipper::RTop_orth one_base(base_mat, base_trans);

   if (form_flag == coot::ideal_rna::B_FORM) {
      // change one_base
      clipper::Mat33<double> b_base_mat(0.809,   -0.5878, 0,
					0.5878,   0.809,  0,
					0, 0, 1);
      clipper::Coord_orth b_base_trans(0, 0, 3.38);
      one_base = clipper::RTop_orth(b_base_mat, b_base_trans);
   } 

   for(int ibase=0; ibase<nbase; ibase++) {
      clipper::RTop_orth o1(one_base * o);
      o = o1;
   }

   return o;

}

bool
coot::ideal_rna::is_valid_base(char base) const {

   if (base == 'g') {
      return 1;
   } else {
      if (base == 'c') {
	 return 1;
      } else {
	 if (base == 'a') {
	    return 1;
	 } else {
	    if (base == 't') {
	       return 1;
	    } else {
	       if (base == 'u') {
		  return 1;
	       }
	    }
	 }
      }
   }
   return 0;
}

char
coot::ideal_rna::antisense_base(char base, bool is_dna_flag) const {

   if (base == 'g') {
      return 'c';
   } else {
      if (base == 'c') {
	 return 'g';
      } else {
	 if (base == 'a') {
	    if (is_dna_flag) 
	       return 't';
	    else 
	       return 'u';
	 } else {
	    if (base == 't') {
	       return 'a';
	    } else {
	       if (base == 'u') {
		  return 'a';
	       }
	    }
	 }
      }
   }
   return 0;
}


//
// Get a deep copy:
// return NULL on failure
// 
mmdb::Residue *
coot::ideal_rna::get_standard_residue_instance(const std::string &residue_type_in,
					       mmdb::Manager *standard_residues) const {

   // convert new names: "A", "DA" to old ones (the ones in the
   // standard residues file), "Ar", "Ad"
   // 
   std::string residue_name = residue_name_from_type(residue_type_in);
   std::cout << "get_standard_residue_instance(): in :" << residue_type_in << ": out :" << residue_name << ":" << std::endl;

   mmdb::Residue *std_residue = 0;
   int selHnd = standard_residues->NewSelection();
   standard_residues->Select (selHnd, mmdb::STYPE_RESIDUE, 1, // .. TYPE, iModel
			      "*",
			      mmdb::ANY_RES, "*",  // starting res
			      mmdb::ANY_RES, "*",  // ending res
			      residue_name.c_str(),  // residue name
			      "*",  // Residue must contain this atom name?
			      "*",  // Residue must contain this Element?
			      "*",  // altLocs
			      mmdb::SKEY_NEW // selection key
			      );
   mmdb::PPResidue SelResidue;
   int nSelResidues;
   standard_residues->GetSelIndex(selHnd, SelResidue, nSelResidues);
   
   if (nSelResidues != 1) {
      std::cout << "This should never happen - ";
      std::cout << "badness in ideal_rna::get_standard_residue_instance(), we selected "
		<< nSelResidues << " residues looking for residues of type :"
		<< residue_name << ": from :" << residue_type_in << ":\n";
   } else {
      std_residue = coot::util::deep_copy_this_residue(SelResidue[0]);
   }
   standard_residues->DeleteSelection(selHnd);
   return std_residue;
}

// convert new names: "A", "DA" to old ones (the ones in the
// standard residues file), "Ar", "Ad"
// 
std::string
coot::ideal_rna::residue_name_from_type(const std::string &residue_type_in) const {

   // 20240420-PE this need a rethink.

   std::string r = "A";

   if (use_standard_refmac_names) {
      return residue_type_in;
   } else {
      if (residue_type_in.length() == 2)
	 if (residue_type_in[1] == 'r'|| residue_type_in[1] == 'd')
	    return residue_type_in;
      if (residue_type_in == "A")
	 return "A";
      if (residue_type_in == "G")
	 return "G";
      if (residue_type_in == "C")
	 return "C";
      if (residue_type_in == "U")
	 return "U";
      if (residue_type_in == "DA")
	 return "DA";
      if (residue_type_in == "DG")
	 return "DG";
      if (residue_type_in == "DC")
	 return "DC";
      if (residue_type_in == "DT")
	 return "DT";
   }
   return r;
}

std::string
coot::ideal_rna::residue_name_old_to_new(const std::string &residue_type_in, bool is_dna_flag) const {

   std::string r = "";
   if (is_dna_flag) {
      if (residue_type_in == "Ad") 
	 return "DA";
      if (residue_type_in == "Gd") 
	 return "DG";
      if (residue_type_in == "Cd") 
	 return "DC";
      if (residue_type_in == "Td") 
	 return "DT";
      if (residue_type_in == "Ud") 
	 return "DU";
   } else {
      if (residue_type_in == "Ar") 
	 return "A";
      if (residue_type_in == "Gr") 
	 return "G";
      if (residue_type_in == "Cr") 
	 return "C";
      if (residue_type_in == "Tr") 
	 return "T";
      if (residue_type_in == "Ur") 
	 return "U";
   } 
   return r; // fail
} 


// return a status: 0 for bad, 1 for good.
int 
coot::ideal_rna::mutate_res(mmdb::Residue *res, char base, bool is_dna_flag) const {

   // we need to get instances of bases from the standard residues
   int status = 0;

   std::string residue_type = "None";

   if (is_dna_flag) { 
      if (base == 'a')
	 residue_type = "DA";
      if (base == 'g')
	 residue_type = "DG";
      if (base == 't')
	 residue_type = "DT";
      if (base == 'c')
	 residue_type = "DC";
   } else {
      if (base == 'a')
	 residue_type = "A";
      if (base == 'g')
	 residue_type = "G";
      if (base == 'u')
	 residue_type = "U";
      if (base == 'c')
	 residue_type = "C";
   }

   if (residue_type != "None") {
      mmdb::Residue *std_res = get_standard_residue_instance(residue_type, standard_residues);
      if (std_res) {
	 coot::util::mutate_base(res, std_res, 1);
	 status = 1;
      }
   }
   return status;
}

void
coot::ideal_rna::delete_o2_prime(mmdb::Residue *res) const {

   int natoms;
   mmdb::PPAtom residue_atoms;
   short int deleted = 0;

   if (res) { 
      res->GetAtomTable(residue_atoms, natoms);
      for (int i=0; i<natoms; i++) {
	 std::string atname(residue_atoms[i]->name);
	 if (atname == " O2*") {
	    res->DeleteAtom(i);
	    deleted=1;
	 } 
	 if (atname == " O2'") {
	    res->DeleteAtom(i);
	    deleted=1;
	 }
      }
      if (deleted)
	 res->TrimAtomTable();
   }
}

// Tinker with res
void
coot::ideal_rna::add_o2_prime(mmdb::Residue *res) const {

   if (res) {
      std::vector<clipper::Coord_orth> mov_pts; // in order
      
      mov_pts.push_back(clipper::Coord_orth(6.853,  -5.219,   1.725));  // C1'
      mov_pts.push_back(clipper::Coord_orth(7.473,  -4.888,   3.087));  // C2'
      mov_pts.push_back(clipper::Coord_orth(6.678,  -5.807,   4.033));  // C3'

      // The coordinate that gets moved by the rtop returned from LSQing.
      // Later, it gets turned into an atom and added to res
      // 
      clipper::Coord_orth o2p(8.870, -5.158, 3.018);

      int natoms;
      mmdb::PPAtom residue_atoms;
      mmdb::Atom *c1p = NULL;
      mmdb::Atom *c2p = NULL;
      mmdb::Atom *c3p = NULL;
      res->GetAtomTable(residue_atoms, natoms);
      for (int i=0; i<natoms; i++) {
	 std::string atname(residue_atoms[i]->name);
	 if (atname == " C1'" || atname == " C1*")
	    c1p = residue_atoms[i];
	 if (atname == " C2'" || atname == " C2*")
	    c2p = residue_atoms[i];
	 if (atname == " C3'" || atname == " C3*")
	    c3p = residue_atoms[i];
      }

      if (c1p && c2p && c3p) { // add o
	 std::vector<clipper::Coord_orth> ref_pts;
	 ref_pts.push_back(clipper::Coord_orth(c1p->x, c1p->y, c1p->z));
	 ref_pts.push_back(clipper::Coord_orth(c2p->x, c2p->y, c2p->z));
	 ref_pts.push_back(clipper::Coord_orth(c3p->x, c3p->y, c3p->z));

	 clipper::RTop_orth rtop(mov_pts, ref_pts);
	 clipper::Coord_orth pos = o2p.transform(rtop);

	 float new_atom_occ = 1.0;
	 float new_atom_b_factor = 30.0;
	 std::string new_atom_name = " O2*";
	 std::string new_atom_ele = " O";

	 mmdb::Atom *new_at = new mmdb::Atom;
	 new_at->SetCoordinates(pos.x(), pos.y(), pos.z(), new_atom_occ, new_atom_b_factor);
	 new_at->SetAtomName(new_atom_name.c_str());
	 new_at->SetElementName(new_atom_ele.c_str());
	 res->AddAtom(new_at);
      }
   }
} 
