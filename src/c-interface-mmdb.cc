
#include <iostream>
#include <string>

#include "c-interface-mmdb.hh"

CMMDBManager *
mmdb_manager_from_scheme_expression(SCM molecule_expression) {

   CMMDBManager *mol = 0;

#if (SCM_MAJOR_VERSION > 1) || (SCM_MINOR_VERSION > 7)
   
   SCM nmol = scm_length(molecule_expression);
   int inmol = scm_to_int(nmol);

   if (inmol > 0) {
      mol = new CMMDBManager; 
      for(int imol=0; imol<inmol; imol++) {
	 CModel *model_p = new CModel;
	 SCM imol_scm = SCM_MAKINUM(imol);
	 SCM chain_expression = scm_list_ref(molecule_expression, imol_scm);
	 SCM chain_is_list_scm = scm_list_p(chain_expression);
	 if (scm_is_true(chain_is_list_scm)) {
	    // printf("chain_expression is a list\n");
	 } else {
	    printf("chain_expression is not a list\n");
	 }
	 // 	    display_scm(chain_expression);
	 SCM chain_expression_length = scm_length(chain_expression);
	 int len_chain_expr = scm_to_int(chain_expression_length);
	 if (len_chain_expr != 2) {
	    std::cout << "bad chain expression, length "
		      << len_chain_expr << std::endl;
	 } else {
	    // normal case
	    // std::cout << "good chain expression " << std::endl;
	    SCM chain_id_scm = scm_list_ref(chain_expression, SCM_MAKINUM(0));
	    SCM residues_list = scm_list_ref(chain_expression, SCM_MAKINUM(1));
	    if (scm_is_true(scm_list_p(residues_list))) {
	       // printf("residues_list is a list\n");
	    } else {
	       printf("residue_list is not a list\n");
	    }
	    SCM n_residues_scm = scm_length(residues_list);
	    int n_residues = scm_to_int(n_residues_scm);
	    // printf("there were %d residues\n", n_residues);
	    if (n_residues > 0) {
	       CChain *chain_p = new CChain;
	       std::string chain_id = scm_to_locale_string(chain_id_scm);
	       chain_p->SetChainID(chain_id.c_str());
	       for (int ires=0; ires<n_residues; ires++) {
		  SCM ires_scm = SCM_MAKINUM(ires);
		  SCM scm_residue = scm_list_ref(residues_list, ires_scm);
		  SCM scm_len_residue_expr = scm_length(scm_residue);
		  int len_residue_expr = scm_to_int(scm_len_residue_expr);
		  if (len_residue_expr != 4) {
		     std::cout << "bad residue expression, length "
			       << len_residue_expr << std::endl;
		  } else {
		     // normal case
		     // std::cout << "good residue expression" << std::endl;
		     SCM scm_residue_number = scm_list_ref(scm_residue, SCM_MAKINUM(0));
		     SCM scm_residue_inscode = scm_list_ref(scm_residue, SCM_MAKINUM(1));
		     SCM scm_residue_name = scm_list_ref(scm_residue, SCM_MAKINUM(2));
		     SCM atoms_list = scm_list_ref(scm_residue, SCM_MAKINUM(3));

		     if (scm_is_true(scm_list_p(atoms_list))) {
			// printf("atoms_list is a list\n");
		     } else {
			printf("atoms_list is not a list\n");
		     }
		     SCM n_atoms_scm = scm_length(atoms_list);
		     int n_atoms = scm_to_int(n_atoms_scm);
		     if (n_atoms > 0) {
			CResidue *residue_p = new CResidue;
			std::string resname = scm_to_locale_string(scm_residue_name);
			int resno = scm_to_int(scm_residue_number);
			// 			   std::cout << "DEBUG:: Found resno:   " << resno << std::endl;
			// 			   std::cout << "DEBUG:: Found resname: " << resname << std::endl;
			std::string inscode = scm_to_locale_string(scm_residue_inscode);
			residue_p->SetResName(resname.c_str());
			residue_p->seqNum = resno;
			memcpy(residue_p->insCode, inscode.c_str(), sizeof(InsCode));
			for (int iat=0; iat<n_atoms; iat++) {
			   SCM iat_scm = SCM_MAKINUM(iat);
			   SCM atom_expression = scm_list_ref(atoms_list, iat_scm);
			   SCM len_atom_expr_scm = scm_length(atom_expression);
			   int len_atom_expr = scm_to_int(len_atom_expr_scm);
			   if (len_atom_expr != 3) {
			      std::cout << "bad atom expression, length "
					<< len_residue_expr << std::endl;
			      SCM dest = SCM_BOOL_F;
			      SCM mess = scm_makfrom0str("object: ~S\n");
			      SCM bad_scm = scm_simple_format(dest, mess, scm_list_1(atom_expression));
			      std::string bad_str = scm_to_locale_string(bad_scm);
			      std::cout << bad_str << std::endl;
			   } else {
			      // normal case
			      // std::cout << "good atom expression " << std::endl;
			      SCM name_alt_conf_pair = scm_list_ref(atom_expression, SCM_MAKINUM(0));
			      SCM occ_b_ele = scm_list_ref(atom_expression, SCM_MAKINUM(1));
			      SCM pos_expr =  scm_list_ref(atom_expression, SCM_MAKINUM(2));
			      SCM len_name_alt_conf_scm = scm_length(name_alt_conf_pair);
			      int len_name_alt_conf = scm_to_int(len_name_alt_conf_scm);
			      SCM len_occ_b_ele_scm = scm_length(occ_b_ele);
			      int len_occ_b_ele = scm_to_int(len_occ_b_ele_scm);
			      SCM len_pos_expr_scm = scm_length(pos_expr);
			      int len_pos_expr = scm_to_int(len_pos_expr_scm);
			      if (len_name_alt_conf == 2) {
				 if (len_occ_b_ele == 3) {
				    if (len_pos_expr == 3) {
				       SCM atom_name_scm = SCM_CAR(name_alt_conf_pair);
				       std::string atom_name = scm_to_locale_string(atom_name_scm);
				       SCM alt_conf_scm = SCM_CAR(SCM_CDR(name_alt_conf_pair));
				       std::string alt_conf = scm_to_locale_string(alt_conf_scm);
				       SCM occ_scm = scm_list_ref(occ_b_ele, SCM_MAKINUM(0));
				       SCM b_scm   = scm_list_ref(occ_b_ele, SCM_MAKINUM(1));
				       SCM ele_scm = scm_list_ref(occ_b_ele, SCM_MAKINUM(2));
				       float b = scm_to_double(b_scm);
				       float occ = scm_to_double(occ_scm);
				       std::string ele = scm_to_locale_string(ele_scm);
				       float x = scm_to_double(scm_list_ref(pos_expr, SCM_MAKINUM(0)));
				       float y = scm_to_double(scm_list_ref(pos_expr, SCM_MAKINUM(1)));
				       float z = scm_to_double(scm_list_ref(pos_expr, SCM_MAKINUM(2)));
				       CAtom *atom = new CAtom;
				       atom->SetCoordinates(x, y, z, occ, b);
				       atom->SetAtomName(atom_name.c_str());
				       atom->SetElementName(ele.c_str());
				       strncpy(atom->altLoc, alt_conf.c_str(), 2);
				       residue_p->AddAtom(atom);
				       // std::cout << "DEBUG:: adding atom " << atom << std::endl;
				    } else {
				       std::cout << "bad atom (position expression) "
						 << std::endl;
				       SCM bad_scm = display_scm(pos_expr);
				       std::string bad_str = scm_to_locale_string(bad_scm);
				       std::cout << bad_str << std::endl;
				    }
				 } else {
				    std::cout << "bad atom (occ b element expression) "
					      << std::endl;
				    SCM bad_scm = display_scm(occ_b_ele);
				    std::string bad_str = scm_to_locale_string(bad_scm);
				    std::cout << bad_str << std::endl;
				 }
			      } else {
				 std::cout << "bad atom (name alt-conf expression) "
					   << std::endl;
				 SCM bad_scm = display_scm(name_alt_conf_pair);
				 std::string bad_str = scm_to_locale_string(bad_scm);
				 std::cout << bad_str << std::endl;
			      }
			   }
			}
			chain_p->AddResidue(residue_p);
		     }
		  }
	       }
	       model_p->AddChain(chain_p);
	    }
	 }
	 mol->AddModel(model_p);
      }
   }
   return mol;

#else
   std::cout << "Must compile with guile 1.8 or better to use this function"
	     << std::endl;
   return 0;
#endif // SCM version

} 

// This is a common denominator really.  It does not depend on mmdb,
// but it can't be declared in c-interface.h because then we'd have to
// include c-interface.h which would cause (resolvable, I think, not
// checked) problems.
// 
// return a scm string, decode to c++ using scm_to_locale_string();
SCM display_scm(SCM o) {

   SCM dest = SCM_BOOL_F;
   SCM mess = scm_makfrom0str("object: ~s\n");
   return scm_simple_format(dest, mess, scm_list_1(o));
}

