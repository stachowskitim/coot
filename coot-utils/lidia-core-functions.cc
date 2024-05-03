/*
 * coot-utils/lidia-core-functions.cc
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
 * Lesser General Public License for more details.
 *
 * You should have received a copies of the GNU General Public License and
 * the GNU Lesser General Public License along with this program; if not,
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA, 02110-1301, USA.
 * See http://www.gnu.org/licenses/
 *
 */

#ifdef MAKE_ENHANCED_LIGAND_TOOLS

#include "lidia-core/rdkit-interface.hh"
#include "coot-coord-utils.hh"
#include "lidia-core-functions.hh"

#include <RDGeneral/FileParseException.h>
#include <RDGeneral/BadFileException.h>

atom_selection_container_t
coot::mol_to_asc_rdkit(const std::string &file_name) {

   atom_selection_container_t asc;
   try { 

      RDKit::RWMol *m = RDKit::Mol2FileToMol(file_name);

      std::string res_name = "UNL";
      try {
	 m->getProp("_Name", res_name);
      }
      catch (const KeyErrorException &kee) {
	 std::cout << "mol_to_asc_rdkit() no rdkit molecule name for " << m << " " << kee.what() << std::endl;
      }

      if (m) {
	 mmdb::Residue *res = make_residue(*m, 0, res_name);
	 if (res) { 
	    mmdb::Manager *mol = util::create_mmdbmanager_from_residue(res);
	    asc = make_asc(mol);
	 }
      } else {
	 std::cout << "Null rdkit mol ptr m" << std::endl;
      } 
   }
   catch (const RDKit::FileParseException &rte) {
      //   std::cout << "WARNING:: " << rte.message() << std::endl; // no more message()
      std::cout << "WARNING:: " << rte.what() << std::endl;
   }
   catch (const RDKit::BadFileException &e) {
      // std::cout << "WARNING:: Bad file " << file_name << " " << e.message() << std::endl;
      std::cout << "WARNING:: Bad file " << file_name << " " << e.what() << std::endl;
   }
   catch (const std::runtime_error &rte) {
      std::cout << "WARNING runtime_error in mol_to_asc_rdkit() " << rte.what() << std::endl;
   } 
   catch (const std::exception &e) {
      std::cout << "WARNING:: mol_to_asc_rdkit: exception: " << e.what() << std::endl;
   } 
   return asc;
}
#endif 

