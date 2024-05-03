/* ligand/test-side-chain-densities.cc
 * 
 * Author: Paul Emsley
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

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>

#include <clipper/ccp4/ccp4_map_io.h>

#include "utils/coot-utils.hh"
#include "coot-utils/atom-selection-container.hh"
#include "coot-utils/xmap-stats.hh"
#include "ligand/side-chain-densities.hh"

void
make_useable_grid_points(int n_steps, float grid_box_radius,
			 int res_no, const std::string &file_name) {
   
   std::string pdb_file_name("test.pdb");
   atom_selection_container_t asc = get_atom_selection(pdb_file_name, false, true, false);
   coot::side_chain_densities scd(n_steps, grid_box_radius, "");
   coot::residue_spec_t spec_this("A", res_no, "");
   coot::residue_spec_t spec_next("A", res_no+1, "");
   mmdb::Residue *residue_this_p = coot::util::get_residue(spec_this, asc.mol);
   mmdb::Residue *residue_next_p = coot::util::get_residue(spec_next, asc.mol);
   if (residue_this_p && residue_next_p) {
      // residue_next_p is only needed when we are making exclusion points
      // (or vice versa) the acceptable grid points. In that case the N
      // is needed to exclude some grid points.
      scd.gen_useable_grid_points(residue_this_p, residue_next_p, n_steps, grid_box_radius, file_name);
   }
}

void
check_useable_grid_points(int n_steps, float grid_box_radius,
			  int res_no,
			  const std::string &useable_grid_points_file_name,
			  const std::string &useable_grid_points_mapped_to_residue) {

   std::string pdb_file_name("test.pdb");
   atom_selection_container_t asc = get_atom_selection(pdb_file_name, false, true, false);
   coot::residue_spec_t spec_this("A", res_no, "");
   mmdb::Residue *residue_p = coot::util::get_residue(spec_this, asc.mol);
   if (residue_p) {
      coot::side_chain_densities scd(n_steps, grid_box_radius, useable_grid_points_file_name);
      scd.check_useable_grid_points(residue_p, useable_grid_points_mapped_to_residue);
   }
}

void
check_stats(int n_steps, float grid_box_radius, const std::string &res_name,
	    const std::string &rot_name,
	    const std::string &pdb_file_name, const std::string &chain_id, int res_no,
	    const std::string &file_name) {

   // do the stats look like the side chain that they are supposed to be?

   // Show me a dotted grids where the size is proportional to the mean

   // make sure that you've made the link from the source side-chain-data directory
   // to the current directory

   atom_selection_container_t asc = get_atom_selection(pdb_file_name, false, true, false);
   coot::residue_spec_t spec_this(chain_id, res_no, "");
   mmdb::Residue *residue_p = coot::util::get_residue(spec_this, asc.mol);
   if (residue_p) {
      std::string grid_points_file_name = file_name;
      coot::side_chain_densities scd(n_steps, grid_box_radius, grid_points_file_name);
      scd.check_stats(residue_p, res_name, rot_name);
   }
}

void
test_residue_vs_likelihoods(int n_steps, float grid_box_radius,
			    const std::string &grid_points_file_name,
			    const std::string &map_file_name,
			    const std::string &pdb_file_name,
			    const std::string &chain_id,
			    int res_no) {

   // Does this look like a TYR m-85 (say)?

   bool limit_to_correct_rotamer_flag = false;

   clipper::CCP4MAPfile file;
   try {
      file.open_read(map_file_name);
      clipper::Xmap<float> xmap;
      file.import_xmap(xmap);
      atom_selection_container_t asc = get_atom_selection(pdb_file_name, false, true, false);
      if (asc.read_success) {
	 std::string id_1 = coot::util::name_sans_extension(pdb_file_name);
	 std::string id = coot::util::file_name_non_directory(id_1);

	 coot::side_chain_densities test_ll(n_steps, grid_box_radius, grid_points_file_name);

	 test_ll.set_data_dir("side-chain-data");
	 coot::residue_spec_t spec_this(chain_id, res_no, "");
	 mmdb::Residue *residue_this_p = coot::util::get_residue(spec_this, asc.mol);
	 if (residue_this_p) {
	    // residue_next_p is only needed when we are making exclusion points
	    // (or vice versa) the acceptable grid points. In that case the N
	    // is needed to exclude some grid points.

	    // when we use with limit_to_correct_rotamer_flag = true
	    // then we should turn on the "engine" debugging - to see all the
	    // individual contributions to the LLR to see if there are systematic baddies
	    //
	    std::vector<mmdb::Residue *> r;
	    r.push_back(residue_this_p);
	    test_ll.fill_residue_blocks(r, xmap);
	    test_ll.get_rotamer_likelihoods(residue_this_p, xmap, limit_to_correct_rotamer_flag);
	 } else {
	    std::cout << "residue " << spec_this << " not found in molecule from "
		      << pdb_file_name << std::endl;
	 }
      }
   }
   catch (const clipper::Message_base &exc) {
      std::cout << "WARNING:: failed to open " << pdb_file_name << std::endl;
      std::cout << "WARNING:: failed to open " << map_file_name << std::endl;
   }
}

void find_probabilities_of_rotamers(int n_steps, float grid_box_radius,
				    const std::string &useable_grid_points_file_name,
				    const std::string &pdb_file_name,
				    const std::string &chain_id,
				    int resno_start,
				    int resno_end,
				    const std::string &map_file_name) {

   // "Guess the residue type"

   try {
      clipper::CCP4MAPfile file;
      file.open_read(map_file_name);
      clipper::Xmap<float> xmap;
      file.import_xmap(xmap);
      atom_selection_container_t asc = get_atom_selection(pdb_file_name, false, true, false);
      if (asc.read_success) {
	 // "analysis" constructor
	 // coot::side_chain_densities scd(n_steps, grid_box_radius, useable_grid_points_file_name);
	 coot::side_chain_densities scd;
	 // scd.set_data_dir("side-chain-data");
         scd.fill_residue_blocks(asc.mol, chain_id, resno_start, resno_end, xmap);
         std::string guessed_sequence =
            scd.guess_the_sequence(asc.mol, chain_id, resno_start, resno_end, xmap);
         std::cout << "guessed sequence " << guessed_sequence << std::endl;
      }
   }
   catch (const clipper::Message_base &exc) {
      std::cout << "WARNING:: failed to open " << pdb_file_name << std::endl;
      std::cout << "WARNING:: failed to open " << map_file_name << std::endl;
   }
}

#include <thread>
#include "utils/coot-fasta.hh"
#include "utils/split-indices.hh"

void test_sequence(int n_steps, float grid_box_radius,
		   const std::string &pdb_file_name,
		   const std::string &chain_id,
		   int resno_start,
		   int resno_end,
		   const std::string &map_file_name,
		   const std::string &multi_sequence_file_name) {

   auto proc_threads = [] (const std::pair<unsigned int, unsigned int> &start_stop_pair,
                           const std::string &chain_id,
                           int resno_start,
                           int resno_end,
                           mmdb::Manager *mol,
                           const clipper::Xmap<float> &xmap,
                           const coot::fasta_multi &fam,
                           coot::side_chain_densities &scd) { // fill scd

      std::cout << "................... proc_threads() " << std::endl;

      for(unsigned int idx=start_stop_pair.first; idx!=start_stop_pair.second; ++idx) {
         // the interface has changed.
         // scd.test_sequence(mol, chain_id, resno_start, resno_end, xmap, fam[idx].name, fam[idx].sequence);
      }
                          
   };

   std::cout << "------------------ test_sequence() --- start --- " << std::endl;

   coot::fasta_multi fam(multi_sequence_file_name);
   if (fam.size() > 0) {

      unsigned int n_sequences = fam.size();
      try {
         clipper::CCP4MAPfile file;
         file.open_read(map_file_name);
         clipper::Xmap<float> xmap;
         file.import_xmap(xmap);
         atom_selection_container_t asc = get_atom_selection(pdb_file_name, false, true, false);
         if (asc.read_success) {
            // "analysis" constructor
            // coot::side_chain_densities scd(n_steps, grid_box_radius, useable_grid_points_file_name);
            // scd.set_data_dir("side-chain-data");
            coot::side_chain_densities scd;

#if 0 // threaded version - reinstate when multisequence comparison is fast
            unsigned int n_threads = coot::get_max_number_of_threads();
            std::vector<std::pair<unsigned int, unsigned int> > index_vector =
               coot::atom_index_ranges(n_sequences, n_threads);
            std::vector<std::thread> threads;

            for (unsigned int i=0; i<index_vector.size(); i++) {
               std::pair<unsigned int, unsigned int> start_stop_pair = index_vector[i];
               threads.push_back(std::thread(proc_threads, std::cref(start_stop_pair),
                                             std::cref(chain_id), resno_start, resno_end, asc.mol,
                                             std::cref(xmap),
                                             std::cref(fam), std::ref(scd)));
            }

            for (unsigned int i=0; i<index_vector.size(); i++)
               threads[i].join();
#endif
#if 1 // the single threaded way

            std::cout << "single-threaded way calling get_fragment_sequence_scores()" << std::endl;
            coot::get_fragment_sequence_scores(asc.mol, fam, xmap);
#endif
         }
      }
      catch (const clipper::Message_base &exc) {
         std::cout << "WARNING:: failed to open " << pdb_file_name << std::endl;
         std::cout << "WARNING:: failed to open " << map_file_name << std::endl;
      }
   }

   // Now, what's best in fam?
}

// generate "stats.table" for every rotamer
void
combine(int n_steps) {

   // Set these from parsing the command line.
   // When optimizing, we need only re-run this combine stage - the grid data
   // generation stage need not be run multiple times.
   //
   unsigned int mn_unreliable_minimum_counts;
   unsigned int mn_unreliable_minimum_counts_for_low_variance;
   double mn_unreliable_minimum_variance;
   double mn_use_this_variance_for_unreliable;
   mn_unreliable_minimum_counts = 10;
   mn_unreliable_minimum_counts_for_low_variance = 20;
   mn_unreliable_minimum_variance = 0.1;
   mn_use_this_variance_for_unreliable = 4.0;

   std::cout << "combine() " << std::endl;
   std::string dir = "side-chain-data";

   // put the globbing in the side_chain_densities class
   //
   std::string glob_pattern = "*";
   std::vector<std::string> dirs = coot::util::glob_files(dir, glob_pattern);
   std::cout << "found " << dirs.size() << " directories in " << dir << std::endl;

   for (std::size_t idir=0; idir<dirs.size(); idir++) {
      const std::string &dir = dirs[idir];
      // std::cout << "dir " << dir << std::endl;

      std::vector<std::string> rot_dirs = coot::util::glob_files(dir, glob_pattern);
      std::cout << "found " << rot_dirs.size() << " rotamer-directories in " << dir << std::endl;
      for (std::size_t jdir=0; jdir<rot_dirs.size(); jdir++) {
	 const std::string &dir = rot_dirs[jdir];
	 std::cout << "combining for rotamer dir: " << dir << std::endl;

	 coot::side_chain_densities::combine_directory(dir, n_steps,
						       mn_unreliable_minimum_counts,
						       mn_unreliable_minimum_counts_for_low_variance,
						       mn_unreliable_minimum_variance,
						       mn_use_this_variance_for_unreliable);
      }
   }
}

void
check_map_mean(const std::string &map_file_name) {

   clipper::CCP4MAPfile file;
   try {
      clipper::Xmap<float> xmap;
      file.open_read(map_file_name);
      file.import_xmap(xmap);
      mean_and_variance<float> mv_0 = map_density_distribution(xmap, 400, false, false);
      mean_and_variance<float> mv_1 = map_density_distribution(xmap, 400, false, true);
      float n_sds_0 = mv_0.mean/sqrt(mv_0.variance);
      float n_sds_1 = mv_1.mean/sqrt(mv_1.variance);
      std::cout << map_file_name << " "
                << std::fixed << std::right << std::setprecision(12)
		<< mv_0.mean << " " << sqrt(mv_0.variance) << " " << n_sds_0 << "    "
		<< mv_1.mean << " " << sqrt(mv_1.variance) << " " << n_sds_1
		<< std::endl;
   }
   catch (const clipper::Message_base &exc) {
      std::cout << "WARNING:: failed to open " << map_file_name << std::endl;
   }

}

void validate_chain(const std::string &map_file_name,
                    const std::string &pdb_file_name,
                    const std::string &chain_id,
                    const std::string &multi_sequence_file_name,
                    const std::string &output_table_file_name_prefix) {

   clipper::CCP4MAPfile file;
   try {
      clipper::Xmap<float> xmap;
      file.open_read(map_file_name);
      file.import_xmap(xmap);

      atom_selection_container_t asc = get_atom_selection(pdb_file_name, false, true, false);
      if (asc.read_success) {
         coot::side_chain_densities scd;
         coot::fasta_multi fam(multi_sequence_file_name);
         coot::get_fragment_by_fragment_scores(asc.mol, fam, xmap, output_table_file_name_prefix);
      }
   }
   catch (const clipper::Message_base &exc) {
      std::cout << "WARNING:: failed to open " << map_file_name << std::endl;
   }

}

int main(int argc, char **argv) {

   bool done = false;

   int n_steps = 5;
   float grid_box_radius = 5.0; // half the width/length of the box (not diagonal)

   std::cout << "debug:: argc " << argc << std::endl;

   if (argc == 2) {
      // generate the stats from the sampled maps.
      std::string a1(argv[1]);
      if (a1 == "combine") {
          combine(n_steps);
          done = true;
      }
   }

   if (argc == 4) {
      std::string a1(argv[1]);
      // make a file that includes grid_idx x y z
      if (a1 == "generate-useable-grid-points") {
         int res_no = coot::util::string_to_int(argv[2]);
         std::string grid_points_file_name(argv[3]);
         make_useable_grid_points(n_steps, grid_box_radius, res_no, grid_points_file_name);
         done = true;
      }
   }

   if (argc == 5) {
      std::string a1(argv[1]);
      if (a1 == "check-generated-useable-grid-points") {
	 int res_no = coot::util::string_to_int(argv[2]);
	 std::string grid_points_file_name(argv[3]);
	 std::string useable_grid_points_mapped_to_residue(argv[4]);
	 check_useable_grid_points(n_steps, grid_box_radius, res_no,
				   grid_points_file_name, useable_grid_points_mapped_to_residue);
	 done = true;
      }
   }

   if (argc == 6) {
      std::string a1(argv[1]);

   }

   if (argc == 7) {
      std::string a1(argv[1]);
      if (a1 == "test-residue") {
         // what are the probabilities that this residue is any of the rotamers
         // using test.pdb and blurred-test.map
         std::string map_file_name(argv[2]);
         std::string pdb_file_name(argv[3]);
         std::string chain_id(argv[4]);
         std::string res_no_str(argv[5]);
         std::string grid_points_file_name(argv[6]);
         try {
            int res_no = coot::util::string_to_int(res_no_str);
            test_residue_vs_likelihoods(n_steps, grid_box_radius,
                                        grid_points_file_name,
                                        map_file_name,
                                        pdb_file_name,
                                        chain_id, res_no);
         }
         catch (const std::runtime_error &rte) {
            std::cout << "" << rte.what() << std::endl;
         }
         done = true;
      }

      // We have a model and a sequence, we want to check that the residue types of the model fit the
      // side-chain densities.
      // This is done in fragments of size 21 (or so).
      //
      if (a1 == "validate-chain") {
         std::cout << "---------- validate chain " << std::endl;
         std::string map_file_name(argv[2]);
         std::string pdb_file_name(argv[3]);
         std::string chain_id(argv[4]);
         std::string multi_sequence_file_name(argv[5]); // one sequence - or one sequence for each different chain
         std::string output_file_name_prefix(argv[6]);
         validate_chain(map_file_name, pdb_file_name, chain_id, multi_sequence_file_name, output_file_name_prefix);
         done = true;
      }
   }

   if (argc == 8) {
      // Make a 3D dot plot - representing the stats for the given rotamer
      // point (size) represent values in stats.table
      // different rotamers should look different
      std::string a1(argv[1]);
      if (a1 == "check-stats") {
	 std::string res_name(argv[2]);
	 std::string rot_name(argv[3]);
	 std::string pdb_file_name(argv[4]);
	 std::string chain_id(argv[5]);
	 std::string res_no_str(argv[6]);
	 std::string grid_points_file_name(argv[7]);
	 try {
	    int res_no = coot::util::string_to_int(res_no_str);
	    // the residue to which the grid is matched is set in the function
	    check_stats(n_steps, grid_box_radius, res_name, rot_name,
			pdb_file_name, chain_id, res_no,
			grid_points_file_name);
	 }
	 catch (const std::runtime_error &rte) {
	    std::cout << "" << rte.what() << std::endl;
	 }
	 done = true;
      }

      if (a1 == "find-probabilities-of-rotamers") {
	 try {
	    std::string pdb_file_name(argv[2]); // poly-ALA model
	    std::string chain_id(argv[3]);
	    int resno_start = coot::util::string_to_int(argv[4]);
	    int resno_end   = coot::util::string_to_int(argv[5]);
	    std::string map_file_name(argv[6]);
	    std::string useable_grid_points_file_name(argv[7]);
	    find_probabilities_of_rotamers(n_steps, grid_box_radius, useable_grid_points_file_name,
					   pdb_file_name, chain_id, resno_start, resno_end,
					   map_file_name);
	 }
	 catch (const std::runtime_error &rte) {
	    std::cout << "ERROR:: " << rte.what() << std::endl;
	 }
	 done = true;
      }


      if (a1 == "test-sequence") {
         std::cout << "test-sequence mode " << std::endl;
	 try {
	    std::string map_file_name(argv[2]);
	    std::string pdb_file_name(argv[3]); // poly-ALA model
	    std::string chain_id(argv[4]);
	    int resno_start = coot::util::string_to_int(argv[5]);
	    int resno_end   = coot::util::string_to_int(argv[6]);

	    std::string multi_sequence_file_name(argv[7]);
            std::cout << "testing sequence..." << std::endl;
	    test_sequence(n_steps, grid_box_radius,
			  pdb_file_name, chain_id, resno_start, resno_end,
			  map_file_name, multi_sequence_file_name);
	 }
	 catch (const std::runtime_error &rte) {
	    std::cout << "ERROR:: " << rte.what() << std::endl;
	 }
	 done = true;
      }
   }


   if (argc == 3) {
      std::string a1(argv[1]);
      if (a1 == "test-grid-interconversion") {
	 std::string useable_grid_points_file_name(argv[2]);
	 coot::side_chain_densities scd(n_steps, grid_box_radius, useable_grid_points_file_name);
	 scd.set_data_dir("side-chain-data");
	 bool success = scd.test_grid_point_to_coords_interconversion();
	 if (success)
	    std::cout << "Correct" << std::endl;
	 done = true;
      }
   }

   if (argc == 3) {
      std::string a1(argv[1]);
      if (a1 == "calc-mean") {
	 std::string map_file_name(argv[2]);
	 std::cout << "### check means for map " << map_file_name << std::endl;
	 check_map_mean(map_file_name);
	 done = true;
      }
   }

   if (argc == 2) {

      std::string a1(argv[1]);
      if (a1 == "check-vs-flat-density") {
         coot::side_chain_densities scd;
         scd.check_vs_flat_density();
      }
   }

   if (! done) {
      // The engine of this program.
      //
      // Make the grid samples for each residue of each structure and map
      //
      if (argc == 5) {
	 std::string a1(argv[1]);
	 if (a1 == "sample") {
	    std::string pdb_file_name(argv[2]);
	    std::string map_file_name(argv[3]);
	    std::string useable_grid_points_file_name(argv[4]);
	    clipper::CCP4MAPfile file;
	    try {
	       file.open_read(map_file_name);
	       clipper::Xmap<float> xmap;
	       file.import_xmap(xmap);
	       atom_selection_container_t asc = get_atom_selection(pdb_file_name, false, true, false);
	       if (asc.read_success) {
		  std::string id_1 = coot::util::name_sans_extension(pdb_file_name);
		  std::string id = coot::util::file_name_non_directory(id_1);
		  coot::side_chain_densities(id, asc.mol, n_steps, grid_box_radius, xmap,
					     useable_grid_points_file_name);
	       }
	    }
	    catch (const clipper::Message_base &exc) {
	       std::cout << "WARNING:: failed to open " << pdb_file_name << std::endl;
	       std::cout << "WARNING:: failed to open " << map_file_name << std::endl;
	    }
	    done = true;
	 }
      }
   }

   if (! done) std::cout << "Nothing done\n";
   return 0;
} 
