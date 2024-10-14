
#ifndef MOLECULES_CONTAINER_HH
#define MOLECULES_CONTAINER_HH

#ifdef SWIG
#include "Python.h"
#endif

#include <vector>

#ifdef HAVE_SSMLIB
#include <ssm/ssm_align.h>
#endif

#include "compat/coot-sysdep.h"

#include "coords/Cartesian.h"
#include "coords/ramachandran-container.hh"
#include "coot-molecule.hh"
#include "coot-utils/coot-rama.hh"
#include "coot-utils/coot-coord-extras.hh" // the missing atoms type
#include "coot-utils/coot-map-utils.hh"
#include "utils/coot-utils.hh"
#include "utils/setup-syminfo.hh"
#include "ideal/simple-restraint.hh" // needed?
#include "atom-pull.hh"
#include "validation-information.hh"
#include "superpose-results.hh"
#include "lsq-results.hh"
#include "coot-utils/simple-mesh.hh"
#include "coot-utils/texture-as-floats.hh"
#include "phi-psi-prob.hh"
#include "instancing.hh"
#include "coot-colour.hh" // put this in utils
#include "saved-strand-info.hh"
#include "svg-store-key.hh"
#include "moorhen-h-bonds.hh"
#include "header-info.hh"

//! the container of molecules. The class for all **libcootapi** functions.
class molecules_container_t {

   std::vector<coot::molecule_t> molecules;
   coot::protein_geometry geom;
   coot::rotamer_probability_tables rot_prob_tables;
   ramachandrans_container_t ramachandrans_container;
   static std::atomic<bool> on_going_updating_map_lock;
   bool draw_missing_residue_loops_flag;

   class rail_points_t {
   public:
      int model_rail_points_delta; // for the latest change, I mean
      int   map_rail_points_delta;
      float rmsd_of_difference_map;
      explicit rail_points_t(float rmsd) {
         model_rail_points_delta = 0;
         map_rail_points_delta = 0;
         rmsd_of_difference_map = rmsd;
      }
      rail_points_t(float rmsd_diff_map_current, const rail_points_t &rail_points_prev) {
         model_rail_points_delta = 0;
         rmsd_of_difference_map = rmsd_diff_map_current;
         map_rail_points_delta = rail_points_delta(rail_points_prev);
      }
      int rail_points_delta(const rail_points_t &prev) {
         float fudge = 2.4; // 20230117-PE makes 1000 rail points equal ~1% in R-factor for the tutorial data
         return int(100000.0 * fudge * (prev.rmsd_of_difference_map - rmsd_of_difference_map));
      }
      static int total(const std::vector<rail_points_t> &rail_point_history) {
         int sum = 0;
         for (const auto &item : rail_point_history) {
            sum += item.map_rail_points_delta;
         }
         return sum;
      }
   };
   std::vector<rail_points_t> rail_point_history; // map and model (model currently not used)

   class updating_maps_info_f {
   public:
      bool maps_need_an_update;
      int imol_model;
      int imol_2fofc;
      int imol_fofc;
      int imol_with_data_info_attached;
      updating_maps_info_f() {
         maps_need_an_update = false;
         imol_model = -1;
         imol_2fofc = -1;
         imol_fofc = -1;
         imol_with_data_info_attached = -1;
      }
   };
   updating_maps_info_f updating_maps_info;
   void set_updating_maps_need_an_update(int imol); // if model imol was changed, let's update the map when
                                                    // the next contouring mesh is requested.
                                                    // Checks the above information before acting, of course.
                                                    // No action if imol is the the model for updating maps.

   //! update the updating maps without generating a mesh
   void update_updating_maps(int imol); // called from the get_map_contours_mesh() function

   coot::util::sfcalc_genmap_stats_t latest_sfcalc_stats;

   // --------------------- superposition --------------------------

   // --------------------- refinement --------------------------

   // 201803004:
   // refinement now uses references to Xmaps.
   // A dummy_map is created and a reference to that is created. Then
   // the reference is reset to a real xmap in a molecule (imol_for_map).
   // But, for a reason I don't understand, the refinement crashes when I do that.
   // When the initial dummy_xmap doesn't go out of scope, then the refinement is OK.
   // So this static dummy map is the map that doesn't go out of scope.
   // We only need one of it, so it goes here, rather than get created every
   // time we do a refinement. It may need to be public in future.
   //
   // 20221018-PE:
   // Now that we are in api, then I am now no longer sure that this should be static
   // or what static means in WebAssembly.
   static clipper::Xmap<float> *dummy_xmap;
   float map_weight;
   float geman_mcclure_alpha;

   bool use_rama_plot_restraints;
   float rama_plot_restraints_weight;

   bool use_torsion_restraints;
   float torsion_restraints_weight;

   ctpl::thread_pool thread_pool;
   bool show_timings;

   coot::restraints_container_t *last_restraints;
   bool continue_threaded_refinement_loop;
   bool refinement_is_quiet;
   int cif_dictionary_read_number;
   // return the state of having found restraints.
   std::string adjust_refinement_residue_name(const std::string &resname) const;
   bool make_last_restraints(const std::vector<std::pair<bool,mmdb::Residue *> > &local_residues,
			     const std::vector<mmdb::Link> &links,
			     const coot::protein_geometry &geom,
			     mmdb::Manager *mol_for_residue_selection,
			     const std::vector<coot::atom_spec_t> &fixed_atom_specs,
			     coot::restraint_usage_Flags flags,
			     bool use_map_flag,
			     const clipper::Xmap<float> *xmap_p);
   coot::refinement_results_t refine_residues_vec(int imol,
                                                  const std::vector<mmdb::Residue *> &residues,
                                                  const std::string &alt_conf,
                                                  mmdb::Manager *mol);

   int find_serial_number_for_insert(int seqnum_new,
                                     const std::string &ins_code_for_new,
                                     mmdb::Chain *chain_p) const;
   atom_selection_container_t make_moving_atoms_asc(mmdb::Manager *residues_mol,
                                                    const std::vector<mmdb::Residue *> &residues) const;
   // return 0 if any of the residues in selection don't have (at least) bond
   // restraints.  Try to auto-load the dictionary cifs and try again.
   // The vector is a list of residues for which no restraints could be found.
   std::pair<int, std::vector<std::string> >
     check_dictionary_for_residue_restraints(int imol, mmdb::PResidue *SelResidues, int nSelResidues);
   std::pair<int, std::vector<std::string> >
     check_dictionary_for_residue_restraints(int imol, const std::vector<mmdb::Residue *> &residues);
   std::pair<mmdb::Manager *, std::vector<mmdb::Residue *> >
   create_mmdbmanager_from_res_vector(const std::vector<mmdb::Residue *> &residues,
                                      int imol,
                                      mmdb::Manager *mol_in,
                                      std::string alt_conf);
   // simple mmdb::Residue * interface to refinement.  20081216
   coot::refinement_results_t
   generate_molecule_and_refine(int imol,  // needed for UDD Atom handle transfer
                                const std::vector<mmdb::Residue *> &residues,
                                const std::string &alt_conf,
                                mmdb::Manager *mol,
                                bool use_map_flag=true);
   bool refinement_immediate_replacement_flag = true;
   int imol_moving_atoms;
   enum moving_atoms_asc_t {
      NEW_COORDS_UNSET = 0,       // moving_atoms_asc_type values
      NEW_COORDS_ADD = 1,                 // not used?
      NEW_COORDS_REPLACE = 2,
      NEW_COORDS_REPLACE_CHANGE_ALTCONF = 3,
      NEW_COORDS_INSERT = 4,
      NEW_COORDS_INSERT_CHANGE_ALTCONF = 5};
   short int moving_atoms_asc_type;
   static void thread_for_refinement_loop_threaded();

   static std::atomic<bool> restraints_lock;
   static void get_restraints_lock(const std::string &calling_function_name);
   static void release_restraints_lock(const std::string &calling_function_name);
   static std::string restraints_locking_function_name; //  static because it is set by above

   bool particles_have_been_shown_already_for_this_round_flag;

   static std::vector<atom_pull_info_t> atom_pulls;
   static void all_atom_pulls_off();
   static void atom_pull_off(const coot::atom_spec_t &spec);
   static void atom_pulls_off(const std::vector<coot::atom_spec_t> &specs);

   std::vector<std::pair<mmdb::Residue *, std::vector<coot::dict_torsion_restraint_t> > > make_rotamer_torsions(const std::vector<std::pair<bool, mmdb::Residue *> > &local_residues) const;

   //! Real space refinement.
   //!
   //! the `n_cycles` parameter allows partial refinement - so for an animated representation one would call this
   //! with a small number (10, 20, 100?) and call it again if the refine status is still yet to reach completion
   //! GSL_CONTINUE (-2). And then make a call to get the bonds mesh (or other molecular representation).
   //! If n_cycles is negative, this means "refine to completion."
   //!
   //! @return success/progress status

   int refine_direct(int imol, std::vector<mmdb::Residue *> rv, const std::string &alt_loc, int n_cycles);

   double phi_psi_probability(const coot::util::phi_psi_t &phi_psi, const ramachandrans_container_t &rc) const;

   //! read the standard protein, RNA, and DNA dictionaries.
   void read_standard_residues();

   std::map<svg_store_key_t, std::string> ligand_svg_store;

   atom_selection_container_t standard_residues_asc;

   int install_model(const coot::molecule_t &m);

   coot::graph_match_info_t overlap_ligands_internal(int imol_ligand, int imol_ref, const std::string &chain_id_ref,
                                                     int resno_ref, bool apply_rtop_flag);

   superpose_results_t
   superpose_with_atom_selection(atom_selection_container_t asc_ref,
                                 atom_selection_container_t asc_mov,
                                 int imol_mov,
                                 std::string moving_mol_name,
                                 std::string referennce_mol_name,
                                 bool move_copy_of_imol2_flag);

#ifdef HAVE_SSMLIB

   void print_ssm_sequence_alignment(ssm::Align *SSMAlign,
				     atom_selection_container_t asc_ref,
				     atom_selection_container_t asc_mov,
				     mmdb::PAtom *atom_selection1,
				     mmdb::PAtom *atom_selection2,
				     int n_selected_atoms_1, int n_selected_atoms_2,
				     bool move_copy_of_imol2_flag);


   coot::validation_information_t
   make_ssm_sequence_alignment_as_validation_information(ssm::Align *SSMAlign,
                                                    atom_selection_container_t asc_ref,
                                                    atom_selection_container_t asc_mov,
                                                    mmdb::PAtom *atom_selection1, mmdb::PAtom *atom_selection2,
                                                    int n_selected_atoms_1, int n_selected_atoms_2,
                                                    bool move_copy_of_imol2_flag);

   void make_and_print_horizontal_ssm_sequence_alignment(ssm::Align *SSMAlign,
							 atom_selection_container_t asc_ref,
							 atom_selection_container_t asc_mov,
							 mmdb::PAtom *atom_selection1,
							 mmdb::PAtom *atom_selection2,
							 int n_selected_atoms_1, int n_selected_atoms_2) const;

   void map_secondary_structure_headers(ssm::Align *SSMAlign,
					atom_selection_container_t asc_ref,
					atom_selection_container_t asc_mov,
					mmdb::PAtom *atom_selection1,
					mmdb::PAtom *atom_selection2,
					int n_selected_atoms_1, int n_selected_atoms_2) const;
   //
   void print_horizontal_ssm_sequence_alignment(std::pair<std::string, std::string> aligned_sequences) const;

   std::string generate_horizontal_ssm_sequence_alignment_string(const std::pair<std::string, std::string> &aligned_sequences) const;

   std::pair<std::string, std::string>
      get_horizontal_ssm_sequence_alignment(ssm::Align *SSMAlign,
					   atom_selection_container_t asc_ref,
					   atom_selection_container_t asc_mov,
					   mmdb::PAtom *atom_selection1, mmdb::PAtom *atom_selection2,
					   int n_selected_atoms_1, int n_selected_atoms_2) const;

   // for gesampt this will be vector of vector
   std::vector<std::pair<coot::residue_validation_information_t, coot::residue_validation_information_t> >
   get_pairs(ssm::Align *SSMAlign,
             atom_selection_container_t asc_ref,
             atom_selection_container_t asc_mov,
             mmdb::PAtom *atom_selection1, mmdb::PAtom *atom_selection2,
             int n_selected_atoms_1, int n_selected_atoms_2) const;

#endif  // HAVE_SSMLIB


   // for auto-read mtz
   int valid_labels(const std::string &mtz_file_name, const std::string &f_col, const std::string &phi_col,
                    const std::string &weight_col, int use_weights) const;

   // water fitting
   float ligand_water_to_protein_distance_lim_max;
   float ligand_water_to_protein_distance_lim_min;
   float ligand_water_variance_limit;
   float ligand_water_sigma_cut_off;

   // --------------------- init --------------------------

   void init() {

      use_gemmi = true;
      imol_refinement_map = -1;
      imol_difference_map = -1;
      thread_pool.resize(8);
      setup_syminfo();
      mmdb::InitMatType();
      geometry_init_standard(); // do this by default now
      refinement_immediate_replacement_flag = true; // 20221018-PE for WebAssembly for the moment
      imol_moving_atoms = -1;
      refinement_is_quiet = true;
      show_timings = true;
      cif_dictionary_read_number = 40;
      // refinement
      continue_threaded_refinement_loop = false;
      particles_have_been_shown_already_for_this_round_flag = false;
      map_weight = 50.0;
      geman_mcclure_alpha = 0.01;
      map_sampling_rate = 1.8;
      draw_missing_residue_loops_flag = true;
      read_standard_residues();
      interrupt_long_term_job = false;
      contouring_time = 0;
      make_backups_flag = true;

      use_rama_plot_restraints = false;
      rama_plot_restraints_weight = 1.0;

      use_torsion_restraints = false;
      torsion_restraints_weight = 1.0;

      map_is_contoured_using_thread_pool_flag = false;

      ligand_water_to_protein_distance_lim_max = 3.4;
      ligand_water_to_protein_distance_lim_min = 2.4;
      ligand_water_variance_limit = 0.1;
      ligand_water_sigma_cut_off = 1.75; // max moorhen points for tutorial 1.

      // debug();
   }

   void debug() const;

   bool map_is_contoured_using_thread_pool_flag;
   double contouring_time;

public:

   //! the one and only constructor
   explicit molecules_container_t(bool verbose=true) : ramachandrans_container(ramachandrans_container_t()) {
      if (! verbose) geom.set_verbose(false);
      init();
      // std::cout << "in constructor map_sampling_rate: " << map_sampling_rate << std::endl;
   }

   ~molecules_container_t();

   //! the refinement map - direct access. When refinement is performed, this is the map
   //! that will be used. Many (indeed most) of thesee functions explicity take a map. If the map
   //! is not known by the calling function then this map can be used as the map molecule index
   int imol_refinement_map; // direct access
   //! the difference map - direct access
   //!
   //! I am not sure that this is needed - or will ever be.
   int imol_difference_map; // direct access

   bool use_gemmi; // for mmcif and PDB parsing. 20240112-PE set to true by default in init()

   //! Set the state of using gemmi for coordinates parsing. The default is true.
   void set_use_gemmi(bool state) { use_gemmi = state; }

   //! get the state of using GEMMI for coordinates parsing
   bool get_use_gemmi() { return use_gemmi; }

   // -------------------------------- Basic Utilities -----------------------------------
   //! \name Basic Utilities

   //! Allow the user to disable/enable backups
   //!
   //! @param state is `true` to mean that it is enabled. The default is `true`.
   void set_make_backups(bool state) { make_backups_flag = state; }

   //! get the state of the backups
   //!
   //! @return the backup-enabled state
   bool get_make_backups() const { return make_backups_flag; }

   //! the backup-enable state (raw public if needed/prefered)
   bool make_backups_flag;

   //! @return the string of the contents of the given file-name.
   std::string file_name_to_string(const std::string &file_name) const;

   //! @return the number of molecules
   unsigned int get_number_of_molecules() const { return molecules.size(); }

   //! The adds a number of empty molecules to the internal vector of molecules
   //! Note that his is not like `reserve` as it will increase the molecule index
   //! of the next added molecule by `n_empty`.
   void create_empty_molecules(unsigned int n_empty);

   //! set the map used for refinement and fitting
   void set_imol_refinement_map(int i) { imol_refinement_map = i; }
   //! set the map weight
   void set_map_weight(float w) { map_weight = w; }
   //! @return the map weight
   float get_map_weight() const { return map_weight; }

   //! Convert atom cid string to a coot atom specifier.
   //! The test for these failing is `spec.empty()`
   coot::atom_spec_t atom_cid_to_atom_spec(int imol, const std::string &cid) const;

   //! Convert residue cid string to a coot residue specifier.
   //! @return the residues spec.  `spec.empty()` is true on failure.
   coot::residue_spec_t residue_cid_to_residue_spec(int imol, const std::string &cid) const;

   //! this set the show_timings flag. Various (not all) functions in this class can calculate how long
   //! they took to run. Setting this will write the time to taken (in milliseconds) to stdout.
   //! The default is `true`.
   void set_show_timings(bool s) { show_timings = s; }

   coot::protein_geometry & get_geom() { return geom; }

   //! get header info.
   //! @return an object with header info. Sparce at the moment.
   moorhen::header_info_t get_header_info(int imol) const;

   //! get imol_enc_any
   //!
   //! @return the value of imol_enc_any (meaning "the molecule number for dictionary that
   // can be used with any molecule")
   int get_imol_enc_any() const;

   // -------------------------------- generic utils -----------------------------------
   //! \name Generic Utils

   //! @return the name of the molecule
   std::string get_molecule_name(int imol) const;
   //! set the molecule name
   void set_molecule_name(int imol, const std::string &new_name);
   //! debugging function: display the table of molecule and names
   void display_molecule_names_table() const;
   //! @return is this a valid model?
   bool is_valid_model_molecule(int imol) const;
   //! @return is this a valid map?
   bool is_valid_map_molecule(int imol_map) const;
   //! @return is this a difference map?
   bool is_a_difference_map(int imol_map) const;

   //! create an empty molecule
   //! @return the index of the new molecule
   int new_molecule(const std::string &name);

   //! close the molecule (and delete dynamically allocated memory)
   //! @return 1 on successful closure and 0 on failure to close
   int close_molecule(int imol);

   // delete the most recent/last closed molecule in the molecule vector, until the first
   // non-closed molecule is found (working from the end)
   void end_delete_closed_molecules();

   //! delete the most recent/last molecule in the molecule vector
   void pop_back();

   //! delete all molecules
   void clear();

   //! @return the eigenvalues of the atoms in the specified residue
   std::vector<double> get_eigenvalues(int imol, const std::string &chain_id, int res_no, const std::string &ins_code);

   //! @return the mesh of a unit solid cube at the origin
   coot::simple_mesh_t test_origin_cube() const;

#ifdef SWIG
#else
#ifdef DOXYGEN_SHOULD_PARSE_THIS
#else
   //! don't use this in emscript
   coot::molecule_t & operator[] (unsigned int imol) {
      // maybe this should throw an exception on out-of-range?
      return molecules[imol];
   }
#endif
#endif

#ifdef DOXYGEN_SHOULD_PARSE_THIS
#else
   //! don't use this in emscript
   mmdb::Manager *get_mol(unsigned int imol) const { // 20221018-PE function name change
      if (is_valid_model_molecule(imol)) {
         return molecules[imol].atom_sel.mol;
      } else {
         return nullptr;
      }
   }
#endif

   //! fill the rotamer probability tables (currently not ARG and LYS)
   void fill_rotamer_probability_tables() {
      if (! rot_prob_tables.tried_and_failed()) {

         std::string tables_dir = coot::package_data_dir();
         char *data_dir = getenv("COOT_DATA_DIR");
         if (data_dir) {
            tables_dir = data_dir;
         }
         tables_dir += "/rama-data";
         rot_prob_tables.set_tables_dir(tables_dir);
         bool ignore_lys_and_arg_flag = true; // 20221018-PE remove this flag when rotamer probabiity
                                              // tables are read from a binary file (and is fast enough
                                              // to include lys and arg).
         rot_prob_tables.fill_tables(ignore_lys_and_arg_flag);
      }
   }

   //! the caller has access to a compressed file that contains the rotamer probabilities.
   //! libcootapi will fill the rotamer probabilities tables from this compressed data stream.
   //! (placeholder only)
   void accept_rotamer_probability_tables_compressed_data(const std::string &data_stream);

   // -------------------------------- backup and saving -----------------------------------
   //! \name Backup and Saving

   //! @return a flag of unsaved models state - i.e. if any of them are unsaved, then this returns true.
   bool contains_unsaved_models() const {
      for (const auto &m : molecules) {
         if (m.have_unsaved_changes()) return true;
      }
      return false;
   }

   //! Save the unsaved model - this function has not yet been written!
   void save_unsaved_model_changes() {
      for (const auto &m : molecules) {
         if (m.have_unsaved_changes()) {
            // something fun here. - whatever it is though, don't put it in this header.
         }
      }
   }

   // -------------------------------- geometry/dictionaries --------------------------------
   //! \name Geometry and Dictionaries

   //! read the stardard list of residues
   void geometry_init_standard();

   //! @return a vector of non-standard residues (so that they can be used for auxiliary dictionary import)
   std::vector<std::string> non_standard_residue_types_in_model(int imol) const;

#ifdef SWIG
#else
#ifdef MAKE_ENHANCED_LIGAND_TOOLS
   //! Result to be eaten by C++ only.
   //! Extract ligand restraints from the dictionary store and make an rdkit molecule
   //! @return a null pointer on failure.
   RDKit::RWMol get_rdkit_mol(const std::string &residue_name, int imol_enc);
   //! get the 64base-encoded pickled string that represents the given residue/ligand name
   //!
   //! @return a string, return a null string on failure.
   std::string get_rdkit_mol_pickle_base64(const std::string &residue_name, int imol_enc);
#endif
#endif

   // -------------------------------- coordinates utils -----------------------------------
   //! \name Coordinates Utils

   //! read a coordinates file (mmcif or PDB)
   //! @return the new molecule index on success and -1 on failure
   int read_coordinates(const std::string &file_name);

   //! read a PDB file (or mmcif coordinates file, despite the name).
   //! It does the same job as `read_coordinates` but has (perhaps) a more familiar name.
   //! @return the new molecule index on success and -1 on failure
   int read_pdb(const std::string &file_name);

   //! print the secondary structure information to standard out
   void print_secondary_structure_info(int imol) const;

   //! read a PDB file (or mmcif coordinates file, despite the name) to
   //! replace the current molecule. This will only work if the molecules
   //! is already a model molecule
   void replace_molecule_by_model_from_file(int imol, const std::string &pdb_file_name);

   //! split an NMR model into multiple models - all in MODEL 1.
   //! @return the vector of new molecule indices.
   std::vector<int> split_multi_model_molecule(int imol);

   //! make a multi-model molecule given the input molecules
   //!
   //! @param ``model_molecules_list`` is a colon-separated list of molecules, *e.g.* `2:3:4`
   //!
   //! @return the new molecule index - -1 if no models were found in the ``model_molecules_list``
   int make_ensemble(const std::string &model_molecule_list);

   //! get the molecule as a PDB string
   //!
   //! @return the model molecule imol as a string. Return emtpy string on error
   std::string molecule_to_PDB_string(int imol) const;

   //! get the molecule as an mmCIF string
   //!
   //! @return the model molecule imol as a string. Return emtpy string on error
   std::string molecule_to_mmCIF_string(int imol) const;

   //! get the active atom given the screen centre
   //!
   //! ``displayed_model_molecules_list`` is a colon-separated list of molecules, *e.g.* "2:3:4"
   //! @return the molecule index and the atom cid. On failure (no molecules with atoms in them, say) then
   //! return -1 and a blank string.
   std::pair<int, std::string> get_active_atom(float x, float y, float z, const std::string &displayed_model_molecules_list) const;

   //! import a dictionary cif - imol_enc is used to specify to which molecule this dictionary should apply.
   //! Use IMOL_ENC_ANY to mean "it applies to all molecules."
   //!
   //! IMOL_ENC_ANY = -999999
   //! @return 1 on success and 0 on failure
   int import_cif_dictionary(const std::string &cif_file_name, int imol_enc);

   //! get the cif file name
   //!
   //! @return the dictionary read for the give residue type, return an empty string on failure
   //! to lookup the residue type
   std::string get_cif_file_name(const std::string &comp_id, int imol_enc) const;
   //! get the cif restraints as a string
   //!
   //! @return a string that is the contents of a dictionary cif file
   std::string get_cif_restraints_as_string(const std::string &comp_id, int imol_enc) const;
   //! copy the dictionary that is specific for imol_current so that it can be used with imol_new
   bool copy_dictionary(const std::string &monomer_name, int imol_current, int imol_new);
   //! get a monomer
   //! @param[in] monomer_name the name of the monomer
   //! @return the new molecule index on success and -1 on failure
   int get_monomer(const std::string &monomer_name);
   //! get a monomer for a particular molecule - use -999999 (IMOL_ENC_ANY) if no molecule-specific dictionary is needed.
   //! @return the new molecule index on success and -1 on failure
   int get_monomer_from_dictionary(const std::string &comp_id, int imol, bool idealised_flag);
   //! get monomer and place it at the given position for a particular molecule - use -999999 if no molecule-specific dictionary is needed
   //! @return the new molecule index on success and -1 on failure
   int get_monomer_and_position_at(const std::string &comp_id, int imol, float x, float y, float z);

   // 20221030-PE nice to have one day:
   // int get_monomer_molecule_by_network_and_dict_gen(const std::string &text);

   //! get the groups for a vector of monomers
   //!
   //! @return the group for the given list of residue names as a vector of strings
   std::vector<std::string> get_groups_for_monomers(const std::vector<std::string> &residue_names) const;

   //! get the group for a particlar monomer
   //!
   //! @return the group for the given residue name
   std::string get_group_for_monomer(const std::string &residue_name) const;

   //! get the hydrogen bond type of a particular atom in a given residue type
   //!
   //! @return the hb_type for the given atom. On failure return an empty string.
   //! Valid types are: "HB_UNASSIGNED" ,"HB_NEITHER", "HB_DONOR", "HB_ACCEPTOR", "HB_BOTH", "HB_HYDROGEN".
   std::string get_hb_type(const std::string &compound_id, int imol_enc, const std::string &atom_name) const;

   //! get the GPhL extra restraint information (from the input cif file)
   //!
   //! @return a vector of string pairs that were part of a gphl_chem_comp_info.
   //!  return an empty vector on failure to find any such info.
   std::vector<std::pair<std::string, std::string> > get_gphl_chem_comp_info(const std::string &compound_id, int imol_enc);

   //! get a list of atom names and their associated atedrg atom types
   //!
   //! @return a list of atom names and their associated atedrg atom types, return an empty list
   //! on failure (atoms types are not in the dictionary or atom failure to look up the compound id)l
   std::vector<std::pair<std::string, std::string> > get_acedrg_atom_types(const std::string &compound_id, int imol_enc) const;

   //! get acedrg types for ligand bonds
   //! @return a `coot::acedrg_types_for_residue_t` - which contains a vector of bond descriptions.
   coot::acedrg_types_for_residue_t get_acedrg_atom_types_for_ligand(int imol, const std::string &residue_cid) const;

   //! write a PNG for the given compound_id. imol can be IMOL_ENC_ANY
   //!
   //! Currently this function does nothing (drawing is done with the not-allowed cairo)
   void write_png(const std::string &compound_id, int imol, const std::string &file_name) const;

   //! write the coordinate to the give file name
   //!
   //! @return 1 on success and 0 on failure
   int write_coordinates(int imol, const std::string &file_name) const;

   //! set the state for drawing missing loops
   //!
   //! By default missing loops are drawn. This function allows missing loops to not be
   //! drawn. Sometimes that can clarify the representation. This is a lightweight function
   //! that sets a flag that is used by subsequent calls to ``get_bonds_mesh()``.
   void set_draw_missing_residue_loops(bool state);

   //! get the bonds mesh
   //!
   //! ``mode`` is "COLOUR-BY-CHAIN-AND-DICTIONARY", "CA+LIGANDS" or "VDW-BALLS"
   //!
   //! ``against_a_dark_background`` allows the bond colours to be relevant for the background.
   //! When the background is dark, the colours should (as a rule) be bright and pastelly.
   //! When the background is light/white, the colour darker and more saturated.
   //!
   //! ``smoothness_factor`` controls the number of triangles used to make the bond cylinders
   //! and spheres for the atoms - it rises in powers of 4. 1 is the smallest ``smoothness_factor``,
   //! 2 looks nice (but maybe is slower to transfer) and 3 is best.
   //!
   //! ``bond_width`` is the bond width in Angstroms. 0.12 is a reasonable default value.
   //!
   //! ``atom_radius_to_bond_width_ratio`` allows the representation of "ball and stick". To do so use a value
   //! between (say) 1.5 and 3.0. The ratio for "liquorice" representation is 1.0 (of course).
   //!
   //! @return a ``coot::simple_mesh_t``
   coot::simple_mesh_t get_bonds_mesh(int imol, const std::string &mode,
                                      bool against_a_dark_background,
                                      float bond_width, float atom_radius_to_bond_width_ratio,
                                      int smoothness_factor);

   //! get the instanced bonds mesh.
   //!
   //! The arguments are as above:
   //!
   //! ``mode`` is "COLOUR-BY-CHAIN-AND-DICTIONARY" - more modes to follow
   //!
   //! ``against_a_dark_background`` allows the bond colours to be relevant for the background.
   //! When the background is dark, the colours should (as a rule) be bright and pastelly.
   //! When the background is light/white, the colour darker and more saturated.
   //!
   //! ``smoothness_factor`` controls the number of triangles used to make the bond cylinders
   //! and spheres for the atoms - it rises in powers of 4. 1 is the smallest ``smoothness_factor``,
   //! 2 looks nice and 3 is best. Instancing may mean that smoothness factor 3 should
   //! be used by default.
   //!
   //! ``bond_width`` is the bond width in Angstroms. 0.12 is a reasonable default value.
   //!
   //! ``atom_radius_to_bond_width_ratio`` allows the representation of "ball and stick". To do so use a value
   //! between (say) 1.5 and 3.0. The ratio for "liquorice" representation is 1.0 (of course). 1.7 or 1.8
   //! looks nice.
   //!
   //! @return a ``coot::instanced_mesh_t``
   coot::instanced_mesh_t get_bonds_mesh_instanced(int imol, const std::string &mode,
                                                   bool against_a_dark_background,
                                                   float bond_width, float atom_radius_to_bond_width_ratio,
                                                   int smoothness_factor);

   //! As above, but only return the bonds for the atom selection.
   //! Typically one would call this with a wider bond_with than one would use for standards atoms (all molecule)
   //!
   //! @return a ``coot::instanced_mesh_t``
   coot::instanced_mesh_t get_bonds_mesh_for_selection_instanced(int imol, const std::string &atom_selection_cid,
                                                                 const std::string &mode,
                                                                 bool against_a_dark_background,
                                                                 float bond_width, float atom_radius_to_bond_width_ratio,
                                                                 int smoothness_factor);

   //! get the Goodsell style mesh
   //!
   //! @return a ``coot::instanced_mesh_t``
   coot::instanced_mesh_t get_goodsell_style_mesh_instanced(int imol, float colour_wheel_rotation_step,
                                                            float saturation, float goodselliness);

   //! export map molecule as glTF
   //!
   //!  (not const because maps might update?)
   void export_map_molecule_as_gltf(int imol, float pos_x, float pos_y, float pos_z, float radius, float contour_level,
                                    const std::string &file_name);

   //! export model molecule as glTF
   //!
   //! This API will change - we want to specify surfaces and ribbons too.
   void export_model_molecule_as_gltf(int imol,
                                      const std::string &selection_cid,
                                      const std::string &mode,
                                      bool against_a_dark_background,
                                      float bonds_width, float atom_radius_to_bond_width_ratio, int smoothness_factor,
                                      bool draw_hydrogen_atoms_flag, bool draw_missing_residue_loops,
                                      const std::string &file_name);

   void export_molecular_represenation_as_gltf(int imol, const std::string &atom_selection_cid,
                                               const std::string &colour_scheme, const std::string &style,
                                               int secondary_structure_usage_flag,
                                               const std::string &file_name);

   //! return the colur table (for testing)
   std::vector<glm::vec4> get_colour_table(int imol, bool against_a_dark_background) const;

   //! set the colour wheel rotation base for the specified molecule (in degrees)
   void set_colour_wheel_rotation_base(int imol, float r);

   //! set the base colour - to be used as a base for colour wheel rotation
   void set_base_colour_for_bonds(int imol, float r, float g, float b);

   //! add a atom selection cid for atoms and bonds not to be drawn
   void add_to_non_drawn_bonds(int imol, const std::string &atom_selection_cid);

   //! clear the set of non-drawn atoms (so that they can be displayed again)
   void clear_non_drawn_bonds(int imol);

   //! print non-drawn bonds
   void print_non_drawn_bonds(int imol) const;

   //! user-defined colour-index to colour
   void set_user_defined_bond_colours(int imol, const std::map<unsigned int, std::array<float, 4> > &colour_map);

   //! set the user-defined residue selections (CIDs) to colour index
   void set_user_defined_atom_colour_by_selection(int imol, const std::vector<std::pair<std::string, unsigned int> > &indexed_residues_cids,
                                                  bool colour_applies_to_non_carbon_atoms_also);

   //! Add a colour rule for M2T representations
   //
   void add_colour_rule(int imol, const std::string &selection_cid, const std::string &colour);

   //! add multiple colour rules, combined like the following "//A/1^#cc0000|//A/2^#cb0002|//A/3^#c00007"
   //! i.e. "|" is the separator for each rule
   //! and "^" is the separator for the selection string and the colour string
   void add_colour_rules_multi(int imol, const std::string &selections_and_colours_combo_string);

   //! delete the colour rules for the given molecule
   void delete_colour_rules(int imol);
   //! get the colour rules
   std::vector<std::pair<std::string, std::string> > get_colour_rules(int imol) const;

   //! print the colour rules
   void print_colour_rules(int imol) const;

   //! use bespoke carbon atom colour
   void set_use_bespoke_carbon_atom_colour(int imol, bool state);

   //! set bespoke carbon atom colour
   void set_bespoke_carbon_atom_colour(int imol, const coot::colour_t &col);

   //! Update float parameter for MoleculesToTriangles molecular mesh
   void M2T_updateFloatParameter(int imol, const std::string &param_name, float value);

   //! Update int parameter for MoleculesToTriangles molecular mesh
   void M2T_updateIntParameter(int imol, const std::string &param_name, int value);

   //! get ribbon and surface representation
   //!
   //! `imol` is the model molecule index
   //!
   //! ecid` is the atom selection CID
   //!
   //! `colour_scheme` should be one of
   //!    - "colorRampChainsScheme"
   //!    - "colorBySecondaryScheme"
   //!    - "Chain"
   //!
   //! `style` should be one of
   //!    - "Ribbon"
   //!    - "MolecularSurface"
   //!
   //! The `secondary_structure_usage_flag` should  be one of
   //!   - 0 (USE_HEADER),
   //!   - 1 (DONT_USE) or
   //!   - 2 (CALC_SECONDARY_STRUCTURE).
   //!
   //! Mode 2 was the (implicit) usage mode until now (20240912).
   //!
   //! @return a `coot::simple_mesh_t`
   coot::simple_mesh_t get_molecular_representation_mesh(int imol, const std::string &cid, const std::string &colour_scheme,
                                                         const std::string &style, int secondary_structure_usage_flag);

   //! get a Gaussian surface representation
   //!
   //! These values seem to give a reasonable quite smooth surface:
   //!   - `sigma` = 4.4
   //!   - `contour_level` = 4.0
   //!   - `box_radius` = 5.0
   //!   - `grid_scale` = 0.7
   //!   - `b_factor` = 100.0 (use 0.0 for no FFT-B-factor smoothing)
   //!
   //! @return a `coot::simple_mesh_t` composed of a number of Gaussian surfaces (one for each chain)
   coot::simple_mesh_t get_gaussian_surface(int imol, float sigma, float contour_level,
                                            float box_radius, float grid_scale, float b_factor) const;

   //! get chemical feaatures for the specified residue
   //!
   //! @return a `coot::simple_mesh_t`
   coot::simple_mesh_t get_chemical_features_mesh(int imol, const std::string &cid) const;

#ifdef DOXYGEN_SHOULD_PARSE_THIS
#else
   //! @returns either the specified atom or null if not found - don't use this in emscript
   mmdb::Atom *get_atom(int imol, const coot::atom_spec_t &atom_spec) const;
   //! @returns either the specified residue or null if not found - don't use this in emscript
   mmdb::Residue *get_residue(int imol, const coot::residue_spec_t &residue_spec) const;
   //! @returns either the specified atom or null if not found - don't use this in emscript
   mmdb::Atom *get_atom_using_cid(int imol, const std::string &cid) const;
   //! @returns either the specified residue or null if not found - don't use this in emscript
   mmdb::Residue *get_residue_using_cid(int imol, const std::string &cid) const;
   //! get the atom position - don't use this in emscript
   std::pair<bool, coot::Cartesian> get_atom_position(int imol, coot::atom_spec_t &atom_spec);
#endif

   //! @return the number of atoms in the specified model, or 0 on error
   unsigned int get_number_of_atoms(int imol) const;

   //! @return an estimate of the diameter of the model molecule (-1 on failure)
   float get_molecule_diameter(int imol) const;

   //! @return the number of hydrogen atoms in the specified model, or -1 on error
   int get_number_of_hydrogen_atoms(int imol) const;

   //! @return vector of chain-ids for the given molecule
   std::vector<std::string> get_chains_in_model(int imol) const;

   //! Get the chains that are related by NCS or molecular symmetry:
   //! @return a vector of vector of chain ids, e.g. [[A,C], [B,D]] (for hemoglobin).
   std::vector<std::vector<std::string> > get_ncs_related_chains(int imol) const;

   //! @return vector of single letter codes - in a pair with the given residue spec
   std::vector<std::pair<coot::residue_spec_t, std::string> > get_single_letter_codes_for_chain(int imol, const std::string &chain_id) const;

   //! @return a list of residue that don't have a dictionary
   std::vector<std::string> get_residue_names_with_no_dictionary(int imol) const;

   //! get residue name
   //! @return the residue name, return a blank string on residue not found.
   std::string get_residue_name(int imol, const std::string &chain_id, int res_no, const std::string &ins_code) const;

   //! @return an object that has information about residues without dictionaries and residues with missing atom
   //! in the the specified molecule
   std::vector<coot::residue_spec_t> residues_with_missing_atoms(int imol);

   //! Ths function is not const because missing_atoms() takes a non-const pointer to the geometry
   // (20230117-PE I should fix that)
   //!
   //! @return an object that has information about residues without dictionaries and residues with missing atom
   //! in the the specified molecule
   coot::util::missing_atom_info missing_atoms_info_raw(int imol);

   //! @return a list of residues specs that have atoms within dist of the atoms of the specified residue
   std::vector<coot::residue_spec_t> get_residues_near_residue(int imol, const std::string &residue_cid, float dist) const;

   //! superposition (using SSM)
   //!
   //! The specified chain of the moving molecule is superposed onto the chain in the reference molecule (if possible).
   //! There is some alignment screen output that would be better added to the return value.
   // std::pair<std::string, std::string>
   superpose_results_t SSM_superpose(int imol_ref, const std::string &chain_id_ref,
                                     int imol_mov, const std::string &chain_id_mov);

   //! superpose using LSQ - setup the matches
   //! @params `match_type` 0: all, 1: main, 2: CAs, 3: N, CA, C
   void add_lsq_superpose_match(const std::string &chain_id_ref, int res_no_ref_start, int res_no_ref_end,
                                const std::string &chain_id_mov, int res_no_mov_start, int res_no_mov_end,
                                int match_type);

   //! clear any existing lsq matchers
   void clear_lsq_matches();

   std::vector<coot::lsq_range_match_info_t> lsq_matchers;

   //! apply the superposition using LSQ
   void lsq_superpose(int imol_ref, int imol_mov);

   //! return the transformation matrix in a simple class - dont apply it to the coordinates
   lsq_results_t get_lsq_matrix(int imol_ref, int imol_mov) const;

   //! make this private
   std::pair<short int, clipper::RTop_orth> get_lsq_matrix_internal(int imol_ref, int imol_mov) const;

   //! symmetry
   //! now comes in a simple container that also includes the cell
   coot::symmetry_info_t
   get_symmetry(int imol, float symmetry_search_radius, float centre_x, float centre_y, float centre_z) const;

   //! Get the cell
   //!
   //! Check that `is_set` is true before use.
   //! @return a `cell_t`
   ::api::cell_t get_cell(int imol)  const;

   //! Get the middle of the "molecule blob" in cryo-EM reconstruction maps
   //! @return a `coot::util::map_molecule_centre_info_t`.
   coot::util::map_molecule_centre_info_t get_map_molecule_centre(int imol) const;

   //! undo
   //! @return 1 on successful undo, return 0 on failure
   int undo(int imol);

   //! redo
   //! @return 1 on successful redo, return 0 on failure
   int redo(int imol);

   // -------------------------------- map utils -------------------------------------------
   //! \name Map Utils

   //! @return the map sampling rate (default is 1.8)
   float map_sampling_rate;

   //! set the map sampling rate (default is 1.8). Higher numbers mean smoother maps, but they take
   //! longer to generate, longer to transfer, longer to parse and longer to draw
   void set_map_sampling_rate(float msr) { map_sampling_rate = msr; }
   //! Read the given mtz file.
   //! @return the new molecule number or -1 on failure
   int read_mtz(const std::string &file_name, const std::string &f, const std::string &phi, const std::string &weight,
                bool use_weight, bool is_a_difference_map);

   //! replace map
   int replace_map_by_mtz_from_file(int imol, const std::string &file_name, const std::string &f, const std::string &phi,
                                    const std::string &weight, bool use_weight);

   //! class for the information about columns extracted from auto-reading the given mtz file
   class auto_read_mtz_info_t {
   public:
      //! molecule index
      int idx;
      //! F column
      std::string F;
      //! phi column
      std::string phi;
      //! weights column
      std::string w;
      //! flag for weights usage
      bool weights_used;
      //! F_obs column. There were not avaliable if the return value is empty
      std::string F_obs;
      //! sigF_obs column
      std::string sigF_obs;
      //! R-Free column. There were not avaliable if the return value is empty
      std::string Rfree;
      auto_read_mtz_info_t() {idx = -1; weights_used = false; }
      auto_read_mtz_info_t(int index, const std::string &F_in, const std::string &phi_in) :
         idx(index), F(F_in), phi(phi_in), weights_used(false) {}
      void set_fobs_sigfobs(const std::string &f, const std::string &s) {
         F_obs = f;
         sigF_obs = s;
      }
   };

   //! Read the given mtz file.
   //! @return a vector of the maps created from reading the file
   std::vector<auto_read_mtz_info_t> auto_read_mtz(const std::string &file_name);
   //! @return the new molecule number or -1 on failure
   int read_ccp4_map(const std::string &file_name, bool is_a_difference_map);
   //! write a map. This function was be renamed from ``writeMap``
   //! @return 1 on a successful write, return 0 on failure.
   int write_map(int imol, const std::string &file_name) const;
   //! @return the mean of the map or -1 is `imol_map` is not a map molecule index
   float get_map_mean(int imol) const;
   //! @return the map rmsd (epsilon testing is not used). -1 is returned if `imol_map` is not a map molecule index.
   float get_map_rmsd_approx(int imol_map) const;

   //! @return the map histogram
   //! The caller should select the number of bins - 200 is a reasonable default.
   //! The caller should also set the zoom factor (which reduces the range by the given factor)
   //! centred around the median (typically 1.0 but usefully can vary until ~20.0).
   coot::molecule_t::histogram_info_t get_map_histogram(int imol, unsigned int n_bins, float zoom_factor) const;

   //! @return the suggested initial contour level. Return -1 on not-a-map
   float get_suggested_initial_contour_level(int imol) const;

   //! check if a map is an EM map or not
   //!
   //! @return the "EM" status of this molecule. Return false on not-a-map.
   bool is_EM_map(int imol) const;

   //! create a new map that is blurred/sharpened
   //! @return the molecule index of the new map or -1 on failure or if `in_place_flag` was true.
   int sharpen_blur_map(int imol_map, float b_factor, bool in_place_flag);

   //! create a new map that is blurred/sharpened and resampling.
   //! Note that resampling can be slow, a resample_factor of 1.5 is about the limit of the trade of of prettiness for speed.
   //! @return the molecule index of the new map or -1 on failure or if `in_place_flag` was true.
   int sharpen_blur_map_with_resample(int imol_map, float b_factor, float resample_factor, bool in_place_flag);

   //! mask map by atom selection (note the argument order is reversed compared to the coot api).
   //!
   //! ``atom_radius`` is the atom radius (funnily enough). Use a negative number to mean "default".
   //!
   //! the ``invert_flag`` changes the parts of the map that are masked, so to highlight the density
   //! for a ligand one would pass the ``cid`` for the ligand and invert_flag as true, so that the
   //! parts of the map that are not the ligand are suppressed.
   //!
   //! @return the index of the new map - or -1 on failure
   int mask_map_by_atom_selection(int imol_coords, int imol_map, const std::string &cid, float atom_radius, bool invert_flag);

   //! Partition the input map. Each voxel in the map is assigned to the chain
   //! to which it is nearest. Unlike masking, the generated maps are not restricted to be
   //! "close" to the atoms in the atom selection.
   //!
   //! c.f. maskChains for ChimeraX - JiangLab
   //!
   //! @return a vector/list of the molecules indices of the newly created maps
   std::vector<int> partition_map_by_chain(int imol_map, int imol_model);

   //! make a masked map
   //!
   //! @return the index of the newly created mask. Return -1 on failure.
   int make_mask(int imol_map_ref, int imol_model, const std::string &atom_selection_cid, float radius);

   //! generate a new map which is the hand-flipped version of the input map.
   //! @return the molecule index of the new map, or -1 on failure.
   int flip_hand(int imol_map);

   //! Make a vector of maps that are split by chain-id of the input imol
   //! @return a vector of the map molecule indices.
   std::vector<int> make_masked_maps_split_by_chain(int imol, int imol_map);

   //! set the map colour.
   //! The next time a map mesh is requested, it will have this colour.
   //! This does not affect the colour of the difference maps.
   void set_map_colour(int imol, float r, float g, float b);

   //! set the state of the mode of the threading in map contouring
   void set_map_is_contoured_with_thread_pool(bool state);

   //! get the mesh for the map contours.
   //!
   //! This function is not **const** because the internal state of a `coot_molecule_t` is changed.
   //! @return a `simple_mesh_t` for the map contours of the specified map
   coot::simple_mesh_t get_map_contours_mesh(int imol, double position_x, double position_y, double position_z,
                                             float radius, float contour_level);

   //! get the mesh for the map contours using another map for colouring
   //!
   //! @return a `simple_mesh_t` for the map contours of the specified map
   coot::simple_mesh_t get_map_contours_mesh_using_other_map_for_colours(int imol_ref, int imol_map_for_colouring,
                                                                         double position_x, double position_y, double position_z,
                                                                         float radius, float contour_level,
                                                                         float other_map_for_colouring_min_value,
                                                                         float other_map_for_colouring_max_value,
                                                                         bool invert_colour_ramp);
   //! set the map saturation
   void set_map_colour_saturation(int imol, float s);

   //! get the latest sfcalc stats
   //!
   //! @return a sfcalc_genmap_stats_t object
   coot::util::sfcalc_genmap_stats_t get_latest_sfcalc_stats() const { return latest_sfcalc_stats; }

   class r_factor_stats {
      public:
      float r_factor;  // 0 to 1
      float free_r_factor;
      int rail_points_total;
      int rail_points_new;
   };

   //! get the R-factors
   //!
   //! @return a r_factor_stats object
   r_factor_stats get_r_factor_stats();

   //! get the R factor stats as a string
   //!
   //! @return a string with the R-factor stats
   std::string r_factor_stats_as_string(const r_factor_stats &rfs) const;

   // This function does no normalisztion of the scales,
   // presuming that they are pre-normalized.
   // @return the index of the new map, or -1 on failure.
   int average_map(const std::string &imol_maps, std::vector<float> &scales);

   // This function does no normalisztion of the scales, presuming that they are pre-normalized.
   // The number of maps in imol_maps should match the size of the scale vector. If not, nothing
   // will happen and False will be returned.
   // @return success status
   bool regen_map(int imol_map, const std::string &imol_maps, const std::vector<float> &scales);

   // -------------------------------- coordinates modelling -------------------------------
   //! \name Coordinates Modelling

   //! auto-fit rotamer
   //! @return 1 on successful modification, return 0 on failure
   int auto_fit_rotamer(int imol, const std::string &chain_id, int res_no, const std::string &ins_code, const std::string &alt_conf,
                        int imol_map);

   //! change to the next rotamer (rotamer cycling is implicit if needed)
   //!
   //! @return the change information.
   coot::molecule_t::rotamer_change_info_t change_to_next_rotamer(int imol, const std::string &residue_cid, const std::string &alt_conf);
   //! change to the next rotamer (rotamer cycling is implicit if needed)
   //!
   //! @return the change information.
   coot::molecule_t::rotamer_change_info_t change_to_previous_rotamer(int imol, const std::string &residue_cid, const std::string &alt_conf);

   //! change to the first (0th) rotamer
   //!
   //! @returns a `coot::molecule_t::rotamer_change_info_t`
   coot::molecule_t::rotamer_change_info_t change_to_first_rotamer(int imol, const std::string &residue_cid, const std::string &alt_conf);

   //! delete item
   //!
   //! where `scope` is one of the strings: ["ATOM","WATER","RESIDUE","CHAIN","MOLECULE", "LITERAL"]
   //! @return 1 on successful modification, return 0 on failure
   std::pair<int, unsigned int> delete_using_cid(int imol, const std::string &cid, const std::string &scope);

   //! delete atom
   //! @return 1 on successful deletion, return 0 on failure to delete.
   std::pair<int, unsigned int> delete_atom(int imol, const std::string &chain_id, int res_no, const std::string &ins_code,
                   const std::string &atom_name, const std::string &alt_conf);
   //! delete atom using atom cid
   //! @return 1 on successful deletion, return 0 on failure to delete.
   std::pair<int, unsigned int> delete_atom_using_cid(int imol, const std::string &cid);

   //! delete residue
   //! @return 1 on successful deletion, return 0 on failure to delete.
   std::pair<int, unsigned int> delete_residue(int imol, const std::string &chain_id, int res_no, const std::string &ins_code);
   //! delete residue using cid
   //! @return 1 on successful deletion, return 0 on failure to delete.
   std::pair<int, unsigned int> delete_residue_using_cid(int imol, const std::string &cid);

   //! delete residue atoms using alt_conf
   //! @return 1 on successful deletion, return 0 on failure to delete.
   std::pair<int, unsigned int> delete_residue_atoms_with_alt_conf(int imol, const std::string &chain_id, int res_no,
                                                                   const std::string &ins_code, const std::string &alt_conf);
   //! delete residue atoms using cid
   //! @return 1 on successful deletion, return 0 on failure to delete.
   std::pair<int, unsigned int> delete_residue_atoms_using_cid(int imol, const std::string &cid);

   //! delete side chain
   //! @return 1 on successful deletion, return 0 on failure to delete.
   std::pair<int, unsigned int> delete_side_chain(int imol, const std::string &chain_id, int res_no, const std::string &ins_code);

   //! delete side chain
   //! @return 1 on successful deletion, return 0 on failure to delete.
   std::pair<int, unsigned int> delete_side_chain_using_cid(int imol, const std::string &cid);

   //! delete chain.
   //! @return 1 on successful deletion, return 0 on failure to delete.
   std::pair<int, unsigned int> delete_chain_using_cid(int imol, const std::string &cid);

   //! delete the atoms specified in the CID selection
   //! @return 1 on successful deletion, return 0 on failure to delete.
   std::pair<int, unsigned int> delete_literal_using_cid(int imol, const std::string &cid);

   //! add a residue onto the end of the chain by fitting to density
   //! @return a first of 1 on success. Return a useful message in second if the addition did not work
   std::pair<int, std::string> add_terminal_residue_directly(int imol, const std::string &chain_id, int res_no, const std::string &ins_code);

   // std::pair<int, std::string> add_terminal_residue_directly_using_cid(int imol, const std::string &cid);
   //
   //! the cid is for an atom.
   //! This used to return a pair, but I removed it so that I could compile the binding.
   //! @return an status.
   int add_terminal_residue_directly_using_cid(int imol, const std::string &cid);

   //! the cid is for an atom.
   //! buccaneer building
   int add_terminal_residue_directly_using_bucca_ml_growing_using_cid(int imol, const std::string &cid);

   //! buccaneer building, called by the above
   int add_terminal_residue_directly_using_bucca_ml_growing(int imol, const coot::residue_spec_t &spec);

   //! parameter for `add_waters()` default  2.4
   void set_add_waters_water_to_protein_distance_lim_min(float d) {
      ligand_water_to_protein_distance_lim_min = d;
   }

   //! parameter for `add_waters()` default 3.4
   void set_add_waters_water_to_protein_distance_lim_max(float d) {
      ligand_water_to_protein_distance_lim_max = d;
   }

   //! parameter for `add_waters()` - default 0.1
   void set_add_waters_variance_limit(float d) {
      ligand_water_variance_limit = d;
   }

   //! parameter for `add_waters()` - default 1.75
   void set_add_waters_sigma_cutoff(float d) {
      ligand_water_sigma_cut_off = d;
   }

   //! add waters, updating imol_model (of course)
   //! @return the number of waters added on a success, -1 on failure.
   int add_waters(int imol_model, int imol_map);

   //! add hydrogen atoms, updating imol_model (of course)
   //! @return 1 on success, 0 on failure.
   int add_hydrogen_atoms(int imol_model);

   //! delete hydrogen atoms, updating imol_model (of course)
   //! @return 1 on a successful deletion, 0 on failure.
   int delete_hydrogen_atoms(int imol_model);

   //! add an alternative conformation for the specified residue
   //! @return 1 on a successful addition, 0 on failure.
   int add_alternative_conformation(int imol_model, const std::string &cid);

   //! fill the specified residue
   //! @return 1 on a successful fill, 0 on failure.
   int fill_partial_residue(int imol, const std::string &chain_id, int res_no, const std::string &ins_code);

   //! fill the specified residue
   //! @return 1 on a successful fill, 0 on failure.
   int fill_partial_residue_using_cid(int imol, const std::string &cid);

   //! fill all the the partially-filled residues in the molecule
   //! @return 1 on a successful fill, 0 on failure.
   int fill_partial_residues(int imol);

   //! flip peptide
   //! @return 1 on a successful flip
   int flip_peptide(int imol, const coot::atom_spec_t &atom_spec, const std::string &alt_conf);
   //! flip peptide using an atom CID
   //! @return 1 on a successful flip
   int flip_peptide_using_cid(int imol, const std::string &atom_cid, const std::string &alt_conf);

   //! eigen-flip ligand
   void eigen_flip_ligand(int imol, const std::string &chain_id, int res_no, const std::string &ins_code);

   //! eigen-flip ligand using CID
   void eigen_flip_ligand_using_cid(int imol, const std::string &residue_cid);

   //! mutate residue
   //! @return 1 on a successful move, 0 on failure.
   int mutate(int imol, const std::string &cid, const std::string &new_residue_type);

   //! rotate last chi angle of the side chain by 180 degrees
   //! @return 1 on a successful move, 0 on failure.
   int side_chain_180(int imol, const std::string &atom_cid);

   //! JED-Flip the ligand (or residue) at the specified atom.
   //! @return a non-blank message if there is a problem
   std::string jed_flip(int imol, const std::string &atom_cid, bool invert_selection);

   //! move the molecule to the given centre
   //! @return 1 on a successful move, 0 on failure.
   int move_molecule_to_new_centre(int imol, float x, float y, float z);

   //! Interactive B-factor refinement (fun).
   //! "factor" might typically be say 0.9 or 1.1
   void multiply_residue_temperature_factors(int imol, const std::string &cid, float factor);

   //! get molecule centre
   //! @return the molecule centre
   coot::Cartesian get_molecule_centre(int imol) const;

   //! copy a fragment given the multi_cid selection string.
   //! @return the new molecule number (or -1 on no atoms selected)
   int copy_fragment_using_cid(int imol, const std::string &multi_cid);

   //! copy a fragment - use this in preference to `copy_fragment_using_cid()` when copying
   //! a molecule fragment to make a molten zone for refinement.
   //! That is because this version quietly also copies the residues near the residues of the selection.
   //! so that those residues can be used for links and non-bonded contact restraints.
   //! `multi_cids" is a "||"-separated list of residues CIDs, e.g. "//A/12-52||//A/14-15||/B/56-66"
   //! @return the new molecule number (or -1 on no atoms selected)
   int copy_fragment_for_refinement_using_cid(int imol, const std::string &multi_cid);

   //! copy a residue-range fragment
   //! @return the new molecule number (or -1 on no atoms selected)
   int copy_fragment_using_residue_range(int imol, const std::string &chain_id, int res_no_start, int res_no_end);

   //! apply transformation to atom selection in the given molecule.
   //! @return the number of atoms moved.
   int apply_transformation_to_atom_selection(int imol, const std::string &atoms_selection_cid,
                                              int n_atoms, // for validation of the atom selection, (int because mmdb atom type)
                                              float m00, float m01, float m02,
                                              float m10, float m11, float m12,
                                              float m20, float m21, float m22,
                                              float c0, float c1, float c2, // the centre of the rotation
                                              float t0, float t1, float t2); // translation

   //! update the positions of the atoms in the residue
   int new_positions_for_residue_atoms(int imol, const std::string &residue_cid, std::vector<coot::molecule_t::moved_atom_t> &moved_atoms);

   //! update the positions of the atoms in the residues
   int new_positions_for_atoms_in_residues(int imol, const std::vector<coot::molecule_t::moved_residue_t> &moved_residues);

   //! ``list_of_other_molecules`` is a colon-separated list of molecules, *e.g.* "2:3:4"
   //! @return the first is a flag set to 1 if a merge occurred (and 0 if it did not)
   //! the second is a vector of merge results, i.e. if you merged a ligand, what is the new
   //! residue spec of the ligand, and if you merged a (polymer) chain, what is the new chain-id of
   //! that chain.
   std::pair<int, std::vector<merge_molecule_results_info_t> >
   merge_molecules(int imol, const std::string &list_of_other_molecules);

   //! this is called by the above function and is useful for other non-api functions (such as add_compound()).
   std::pair<int, std::vector<merge_molecule_results_info_t> >
   merge_molecules(int imol, std::vector<mmdb::Manager *> mols);

   //! Convert a cis peptide to a trans or vice versa.
   //! @return 1 on a successful conversion.
   int cis_trans_convert(int imol, const std::string &atom_cid);

   //! Replace a residue.
   //!
   //! This has a different meaning of "replace" to `replace_fragment`. In this function
   //! the atoms are not note merely moved/"slotted in to place", but the residue type is
   //! changed - new atoms are introduce and others are deleted (typically).
   //!
   //! Change the type of a residue (for example, "TYR" to "PTY")
   //! The algorithm will superpose the mainchain CA, C and N and try to set matching torsion
   //! to the angle that they were in the reference structure.
   void replace_residue(int imol, const std::string &residue_cid, const std::string &new_residue_type, int imol_enc);

   //! replace a fragment
   //!
   //! _i.e._ replace the atoms of ``imol_base`` by those of the atom selection ``atom_selection`` in ``imol_reference``
   //! (``imol_base`` is the molecule that is modified).
   //!
   //! @return the success status
   int replace_fragment(int imol_base, int imol_reference, const std::string &atom_selection);

   //! Rigid-body fitting
   //!
   //! `multi_cids" is a "||"-separated list of residues CIDs, e.g. "//A/12-52||//A/14-15||/B/56-66"
   int rigid_body_fit(int imol, const std::string &multi_cid, int imol_map);

   //! change the chain id
   //! @return -1 on a conflict
   //! 1 on good.
   //! 0 on did nothing
   //! return also an information/error message
   std::pair<int, std::string> change_chain_id(int imol, const std::string &from_chain_id,
                                               const std::string &to_chain_id,
                                               bool use_resno_range,
                                               int start_resno, int end_resno);

   //! associate a sequence with a molecule
   void associate_sequence(int imol, const std::string &name_or_chain_id, const std::string &sequence);
   //! assign a sequence to a molecule. Often one might copy out a fragment from a more complete
   //! molecule (and then copy it back after the sequence has been added). This runs
   //! backrub_rotamer() on the newly assigned residues.
   void assign_sequence(int imol_model, int imol_map);

   // -------------------------------- Coordinates Refinement ------------------------------
   //! \name Coordinates Refinement

   //! refine the residues
   //
   //! ``mode`` is one of {SINGLE, TRIPLE, QUINTUPLE, HEPTUPLE, SPHERE, BIG_SPHERE, CHAIN, ALL};
   //! @returns a value of 1 if the refinement was performed and 0 if it was not.
   int refine_residues_using_atom_cid(int imol, const std::string &cid, const std::string &mode, int n_cycles);

   //! refine the residues
   //! @returns a value of 1 if the refinement was performed and 0 if it was not.
   int refine_residues(int imol, const std::string &chain_id, int res_no, const std::string &ins_code,
                       const std::string &alt_conf, const std::string &mode, int n_cycles);

   //! refine residue range
   //! @returns a value of 1 if the refinement was performed and 0 if it was not.
   int refine_residue_range(int imol, const std::string &chain_id, int res_no_start, int res_no_end, int n_cycles);

   // minimize/optimize the geometry of the specified residue(s).
   // If you pass n_cycles = 100 (or some such) then you can get the mesh for the partially optimized ligand/residues
   // (like `refine_residues()` and `refine()`) is nice for animation.
   // @return the success status 1 if the minimization was performed and 0 if it was not.
   std::pair<int, coot::instanced_mesh_t>
   minimize_energy(int imol, const std::string &atom_selection_cid,
                   int n_cycles,
                   bool do_rama_plot_restraints, float rama_plot_weight,
                   bool do_torsion_restraints, float torsion_weight, bool refinement_is_quiet);

   //! fix atoms during refinement. Does nothing at the moment.
   void fix_atom_selection_during_refinement(int imol, const std::string &atom_selection_cid);

   //! add or update (if it has a pull restraint already)
   void add_target_position_restraint(int imol, const std::string &atom_cid, float pos_x, float pos_y, float pos_z);

   //! clear target_position restraint
   void clear_target_position_restraint(int imol, const std::string &atom_cid);

   //! clear target_position restraint if it is (or they are) close to their target position
   void turn_off_when_close_target_position_restraint(int imol);

   //! turn on or off rama restraints
   void set_use_rama_plot_restraints(bool state) { use_rama_plot_restraints = state; }

   //! get the state of the rama plot restraints usage in refinement.
   //! @return the state
   bool get_use_rama_plot_restraints() const { return use_rama_plot_restraints; }

   //! set the Ramachandran plot restraints weight
   void set_rama_plot_restraints_weight(float f) { rama_plot_restraints_weight = f; }

   //! get the Ramachandran plot restraints weight
   //! @return the Ramachandran plot restraints weight
   float get_rama_plot_restraints_weight() const { return rama_plot_restraints_weight; }

   //! turn on or off torsion restraints
   void set_use_torsion_restraints(bool state) { use_torsion_restraints = state; }

   //! get the state of the rama plot restraints usage in refinement.
   //! @return the state
   bool get_use_torsion_restraints() const { return use_torsion_restraints; }

   //! set the Ramachandran plot restraints weight
   void set_torsion_restraints_weight(float f) { torsion_restraints_weight = f; }

   //! get the Ramachandran plot restraints weight
   //! @return the Ramachandran plot restraints weight
   float get_torsion_restraints_weight() const { return torsion_restraints_weight; }

   //! initialise the refinement of (all of) molecule `imol_frag`
   void init_refinement_of_molecule_as_fragment_based_on_reference(int imol_frag, int imol_ref, int imol_map);

   //! Run some cycles of refinement and return a mesh.
   //! That way we can see the molecule animate as it refines
   //! @return a pair: the first of which is the status of the refinement: GSL_CONTINUE, GSL_SUCCESS, GSL_ENOPROG (no progress).
   //! i.e. don't call thus function again unless the status is GSL_CONTINUE (-2);
   //! The second is a `coot::instanced_mesh_t`
   std::pair<int, coot::instanced_mesh_t> refine(int imol, int n_cycles);

   //! Create a new position for the given atom and create a new bonds mesh based on that.
   //! This is currently "heavyweight" as the bonds mesh is calculated from scratch (it is not (yet) merely a distortion
   //! of an internally-stored mesh).
   //! `n_cycles` specifies the number of refinement cyles to run after the target position of the atom has been applied.
   //! If n_cycles is -1 then, no cycles are done and the mesh is bonds merely calculated.
   //! @return a `coot::instanced_mesh_t`
   coot::instanced_mesh_t add_target_position_restraint_and_refine(int imol, const std::string &atom_cid,
                                                                   float pos_x, float pos_y, float pos_z,
                                                                   int n_cycles);
   //! clear any and all drag-atom target position restraints
   void clear_target_position_restraints(int imol);

   //! call this after molecule refinement has finished (say when the molecule molecule is accepted into the
   //! original molecule)
   void clear_refinement(int imol);

   //! for debugging the refinement - write out some diagnositics - some might be useful.
   //! API change 20240226 - this function now takes a boolean argument
   void set_refinement_is_verbose(bool state) { refinement_is_quiet = !state; }

   //! set the refinement Geman-McClure alpha
   void set_refinement_geman_mcclure_alpha(float a) { geman_mcclure_alpha = a; }

   //! set the refinement Geman-McClure alpha
   float get_geman_mcclure_alpha() const { return geman_mcclure_alpha; }

   //! generate GM self restraints for the whole molecule
   //! @return nothing useful.
   int generate_self_restraints(int imol, float local_dist_max);

   //! generate GM self restraints for the given chain
   void generate_chain_self_restraints(int imol, float local_dist_max,
                                       const std::string &chain_id);

   //! generate GM self restraints for the given residues.
   //! `residue_cids" is a "||"-separated list of residues, e.g. "//A/12||//A/14||/B/56"
   void generate_local_self_restraints(int imol, float local_dist_max,
                                       const std::string &residue_cids);

   //! generate parallel plane restraints (for RNA and DNA)
   void add_parallel_plane_restraint(int imol,
                                     const std::string &residue_cid_1,
                                     const std::string &residue_cid_2);

   //! get the mesh for extra restraints
   //! currently mode is unused.
   coot::instanced_mesh_t get_extra_restraints_mesh(int imol, int mode);

   //! read extra restraints (e.g. from ProSMART)
   void read_extra_restraints(int imol, const std::string &file_name);

   //! clear the extra restraints
   void clear_extra_restraints(int imol);

   // -------------------------------- Coordinates validation ------------------------------
   //! \name Coordinates Validation

   //! get the rotamer dodecs for the model, not const because it regenerates the bonds.
   //! @return a `coot::simple_mesh_t`
   coot::simple_mesh_t get_rotamer_dodecs(int imol);

   //! get the rotamer dodecs for the model, not const because it regenerates the bonds.
   //! @return an `instanced_mesh_t`
   coot::instanced_mesh_t get_rotamer_dodecs_instanced(int imol);

   //! get the ramachandran validation markup mesh
   //!
   //! 20221126-PE: the function was renamed from ``ramachandran_validation_markup_mesh()``.
   //! @return a `coot::simple_mesh_t`
   coot::simple_mesh_t get_ramachandran_validation_markup_mesh(int imol) const;
   //! get the data for Ramachandran validation, which importantly contains probability information
   //! @return a vector of `phi_psi_prob_t`
   std::vector<coot::phi_psi_prob_t> ramachandran_validation(int imol) const;

   //! Recently (20230202) the smoothness factor has been added as an extra argument
   //! `smoothness_factor` is 1, 2 or 3 (3 is the most smooth).
   //! @return the instanced mesh for the specified ligand
   coot::instanced_mesh_t contact_dots_for_ligand(int imol, const std::string &cid, unsigned int smoothness_factor) const;

   //! Recently (20230202) the smoothness factor has been added as an extra argument
   //! `smoothness_factor` is 1, 2 or 3 (3 is the most smooth).
   //! @return the instanced mesh for the specified molecule.
   coot::instanced_mesh_t all_molecule_contact_dots(int imol, unsigned int smoothness_factor) const;

   //! @return a `simple::molecule_t` for the specified residue.
   //! @note this function is not const because we pass a pointer to the protein_geometry geom.
   coot::simple::molecule_t get_simple_molecule(int imol, const std::string &residue_cid, bool draw_hydrogen_atoms_flag);

   //! @return a vector of lines for non-bonded contacts and hydrogen bonds
   generic_3d_lines_bonds_box_t
   make_exportable_environment_bond_box(int imol, coot::residue_spec_t &spec);

   //! `mcdonald_and_thornton_mode` turns on the McDonald & Thornton algorithm - using explicit hydrogen atoms
   //! @return a vector of hydrogen bonds around the specified residue (typically a ligand)
   std::vector<moorhen::h_bond> get_h_bonds(int imol, const std::string &cid_str, bool mcdonald_and_thornton_mode) const;

   //! get the mesh for ligand validation vs dictionary, coloured by badness.
   //! greater then 3 standard deviations is fully red.
   //! Less than 0.5 standard deviations is fully green.
   // Function is not const because it might change the protein_geometry geom.
   coot::simple_mesh_t get_mesh_for_ligand_validation_vs_dictionary(int imol, const std::string &ligand_cid);

   //! ligand validation
   //!
   //! @return a vector of interesting geometry
   std::vector<coot::geometry_distortion_info_container_t>
   get_ligand_validation_vs_dictionary(int imol, const std::string &ligand_cid, bool include_non_bonded_contacts);

   //! match ligand torsions - return the success status
   bool match_ligand_torsions(int imol_ligand, int imol_ref, const std::string &chain_id_ref, int resno_ref);

   //! match ligand positions
   //!
   //! i.e. do a least-squares superposition of the atoms that match in the graphs of the
   //! two specified ligands - typically one would use this function after matching ligand torsions.
   //!
   //! @return the success status
   bool match_ligand_position(int imol_ligand, int imol_ref, const std::string &chain_id_ref, int resno_ref);

   //! match ligand torsions and positions
   //!
   //! @return the success status.
   bool match_ligand_torsions_and_position(int imol_ligand, int imol_ref, const std::string &chain_id_ref, int resno_ref);

   //! match ligand torsions and positions, different api
   bool match_ligand_torsions_and_position_using_cid(int imol_ligand, int imol_ref, const std::string &cid);

   //! not const because it can dynamically add dictionaries
   coot::atom_overlaps_dots_container_t get_overlap_dots(int imol);

   //! not const because it can dynamically add dictionaries
   coot::atom_overlaps_dots_container_t get_overlap_dots_for_ligand(int imol, const std::string &cid_ligand);

   //! not const because it can dynamically add dictionaries
   std::vector<coot::plain_atom_overlap_t> get_overlaps(int imol);

   //! not const because it can dynamically add dictionaries
   std::vector<coot::plain_atom_overlap_t> get_overlaps_for_ligand(int imol, const std::string &cid_ligand);

   // -------------------------------- Coordinates and map validation ----------------------
   //! \name Coordinates and Map Validation

   //! density fit validation information
   //! @returns a `coot::validation_information_t`
   coot::validation_information_t density_fit_analysis(int imol_model, int imol_map) const;

   //! Get the density correlation validation information
   //!
   //! @returns a `coot::validation_information_t` object
   coot::validation_information_t density_correlation_analysis(int imol_model, int imol_map) const;

   //! Get the rotamer validation information
   //!
   //! @returns a `coot::validation_information_t` object
   coot::validation_information_t rotamer_analysis(int imol_model) const;

   //! Get the ramachandran validation information (formatted for a graph, not 3d)
   //!
   //! @returns a `coot::validation_information_t` object
   coot::validation_information_t ramachandran_analysis(int imol_model) const;

   //! ramachandran validation information (formatted for a graph, not 3d) for a given chain in a given molecule
   //! 20230127-PE This function does not exist yet.
   //!
   //! @returns a `coot::validation_information_t`
   coot::validation_information_t ramachandran_analysis_for_chain(int imol_model, const std::string &chain_id) const;

   //! peptide omega validation information
   //! @returns a `coot::validation_information_t`
   coot::validation_information_t peptide_omega_analysis(int imol_model) const;

   //! get the median temperature factor for the model
   //! @return a negative number on failure.
   float get_median_temperature_factor(int imol) const;

   //! get interesting places (does not work yet)
   //! @return a vector of `coot::validation_information_t`
   std::vector<coot::molecule_t::interesting_place_t> get_interesting_places(int imol, const std::string &mode) const;

   //! get difference map peaks
   //! @return a vector of `coot::validation_information_t`
   std::vector<coot::molecule_t::interesting_place_t> difference_map_peaks(int imol_map, int imol_protein, float n_rmsd) const;

   //! get pepflips based on the difference map
   //! @return a vector of `coot::validation_information_t`
   std::vector<coot::molecule_t::interesting_place_t> pepflips_using_difference_map(int imol_coords, int imol_difference_map, float n_sigma) const;

   //! unmodelled blobs
   //! @return a vector of `coot::validation_information_t`
   std::vector<coot::molecule_t::interesting_place_t> unmodelled_blobs(int imol_model, int imol_map) const;

   //! Check waters, using implicit logical OR
   //!
   //! typical values for `b_factor_lim` is 60.0
   //! typical values for `outlier_sigma_level` is 0.8
   //! typical values for `min_dist` is 2.3
   //! typical values for `max_dist` is 3.5
   //!
   //! @return a vector of atom specifiers
   //! Use the string_user_data of the spec for the button label
   std::vector <coot::atom_spec_t>
   find_water_baddies(int imol_model, int imol_map,
                      float b_factor_lim,
                      float outlier_sigma_level,
                      float min_dist, float max_dist,
                      bool ignore_part_occ_contact_flag,
                      bool ignore_zero_occ_flag);

   //! Calculate the MMRRCC for the residues in the chain
   //! Multi Masked Residue Range Corellation Coefficient
#ifdef SWIG
#else
   std::pair<std::map<coot::residue_spec_t, coot::util::density_correlation_stats_info_t>,
             std::map<coot::residue_spec_t, coot::util::density_correlation_stats_info_t> >
   mmrrcc(int imol, const std::string &chain_id, int imol_map) const;
#endif

   //! calculate the MMRRCC for the residues in the chain
   //! Multi Masked Residue Range Corellation Coefficient
#ifdef SWIG
#else
   std::pair<std::map<coot::residue_spec_t, coot::util::density_correlation_stats_info_t>,
             std::map<coot::residue_spec_t, coot::util::density_correlation_stats_info_t> >
   mmrrcc_internal(const atom_selection_container_t &asc,
                   const std::string &chain_id,
                   const clipper::Xmap<float> &xmap) const;
#endif

   //! Fourier Shell Correlation (FSC) between maps
   //!
   //! @return a vector or pairs of graph points (resolution, correlation). The resolution is in inverse Angstroms squared.
   //!  An empty list is returned on failure
   std::vector<std::pair<double, double> > fourier_shell_correlation(int imol_map_1, int imol_map_2) const;

   //! Make a FSC-scaled map
   //!
   //! @return the molecule index of the new map
   int make_power_scaled_map(int imol_ref, int imol_map_for_scaling);

   //! Get the Pintile et al. Q Score
   //!
   //! @return a coot::validation_information_t object
   coot::validation_information_t get_q_score(int imol_model, int imol_map) const;

   // -------------------------------- Rail Points ------------------------------------------
   //! \name Rail Points!

   //! calling this adds to the rail_points history. Make this pairs when we add model scoring.
   //! @returns the new rail points (since last modification)
   int calculate_new_rail_points();

   //! the total rail points
   //! @returns the sum of all rail points accumulated since the maps were connected.
   int rail_points_total() const;

   // -------------------------------- Updating Maps ---------------------------------------
   //! \name Updating Maps

   //! associate a data mtz file with a molecule
   //!
   //! call this before calling connect_updating_maps().
   void associate_data_mtz_file_with_map(int imol, const std::string &data_mtz_file_name,
                                         const std::string &f_col, const std::string &sigf_col,
                                         const std::string &free_r_col);

   //! reset the rail_points (calls reset_the_rail_points()), updates the maps (using internal/clipper SFC)
   //! so, update your contour lines meshes after calling this function.
   //! @return 1 if the connection was successful.
   int connect_updating_maps(int imol_model, int imol_with_data_info_attached, int imol_map_2fofc, int imol_map_fofc);

   //! sfcalc and re-generate maps. This is a low-level function - generally one would use the updating maps
   //! method rather than this
   void sfcalc_genmap(int imol_model,
                      int imol_map_with_data_attached,
                      int imol_updating_difference_map);

   //! sfcalc and re-generate maps (low-level function). This functions uses bulk solvent.
   //!
   //! Call this function after connecting maps for updating maps to set the initial R-factor
   //! and store the initial map flatness.
   //!
   //! @return a class of interesting statistics.
   //!
   //! On failure to calculate SFS and generate the maps the returned r_factor
   //! in the returned stats will be set to -1.
   coot::util::sfcalc_genmap_stats_t
   sfcalc_genmaps_using_bulk_solvent(int imol_model,
                                     int imol_2fofc_map,
                                     int imol_updating_difference_map,
                                     int imol_map_with_data_attached);

   //! shift_field B-factor refinement. This function presumes that the Fobs,sigFobs
   //! and RFree data have been filled in the `imol_map_with_data_attached` molecule.
   //! @return success status
   bool shift_field_b_factor_refinement(int imol, int imol_with_data_attached);

   //! get density at position
   //! @return density value
   float get_density_at_position(int imol_map, float x, float y, float z) const;

   //! @return a vector the position where the differenc map has been flattened.
   //! The associated float value is the ammount that the map has been flattened.
   //!
   //! This is a light-weight fetch, the values have already been computed, here
   //! were are merely copying them.
   std::vector<std::pair<clipper::Coord_orth, float> > get_diff_diff_map_peaks(int imol_diff_map,
                                                                               float screen_centre_x,
                                                                               float screen_centre_y,
                                                                               float screen_centre_z) const;


   //! the stored data set file name
   std::string get_data_set_file_name(int imol) const;

   // -------------------------------- Go To Blob ---------------------------------------
   //! \name Go to Blob

   //! Given a point on the front clipping plane (x1, y1, z1) and a point on the back clipping plane (x2, y2, z2)
   //! this function searches imol_refinement_map (if set) to find a the centre of a blob above the contour level.
   //! Blobs at the "front" are selected in preference to blobs at the back.
   //! If no blob is found, then the first of the pair is false.
   //! If it is found, then the second is (obviously) the centre of the blob.
   //!
   //! 20221022-PE: in future, this function should/will be provided with a list of displayed maps and their
   //! contour levels - but for now, it uses (only) imol_refinement_map.
   //! Use a string to pass the map information (map index and contour level), something like "0 0.45:1 1.2:2 0.1"
   std::pair<bool, clipper::Coord_orth> go_to_blob(float x1, float y1, float z1, float x2, float y2, float z2,
                                                   float contour_level);


   // -------------------------------- Ligand Functions ---------------------------------------
   //! \name Ligand Functions

   //! Ligand Fitting
   //!
   //! I am not yet clear what extra cut-offs and flags need to be added here.
   //! You can expect this to take about 20 seconds.
   //!
   //! For trivial (i.e non-flexible) ligands you should instead use the jiggle-fit algorithm, which
   //! takes a fraction of a second. (That is the algorithm used for "Add Other Solvent Molecules" in Coot.)
   //!
   //! @return a vector of indices of molecules for the best fitting ligands to this blob.
   std::vector<int> fit_ligand_right_here(int imol_protein, int imol_map, int imol_ligand, float x, float y, float z,
                                          float n_rmsd, bool use_conformers, unsigned int n_conformers);

   //! Ligand Fitting
   //!
   //! @return a vector of indices of molecules for the best fitting ligands each of the "possible ligand" blobs.

   class fit_ligand_info_t {
   public:
      int imol; // the imol of the fitted ligand
      int cluster_idx;  // the index of the cluster
      int ligand_idx;  // the ligand idx for a given cluster
      float fitting_score;
      float cluster_volume;
      fit_ligand_info_t(int i, int c, int l) : imol(i), cluster_idx(c), ligand_idx(l) { fitting_score = -1.0; cluster_volume = -1.0;}
      fit_ligand_info_t() { imol = -1; cluster_idx = -1; ligand_idx = -1; fitting_score = 1.0; cluster_volume = -1.0; }
      //! @return the fitting score
      float get_fitting_score() const { return fitting_score; }
      float get_cluster_volume() const { return cluster_volume; }
   };

   //! Fit ligand
   //! @return a vector of interesting information about the fitted ligands
   std::vector<fit_ligand_info_t> fit_ligand(int imol_protein, int imol_map, int imol_ligand,
                                             float n_rmsd, bool use_conformers, unsigned int n_conformers);

   //! Fit ligands (place-holder)
   //! ``multi_ligand_molecule_number_list`` is a colon-separated list of molecules, *e.g.* "2:3:4"
   //! @return an empty vector (at the moment)
   std::vector<fit_ligand_info_t> fit_ligand_multi_ligand(int imol_protein, int imol_map, const std::string &multi_ligand_molecule_number_list,
                                                          float n_rmsd, bool use_conformers, unsigned int n_conformers);

   //! "Jiggle-Fit Ligand"
   //! if n_trials is 0, then a sensible default value will be used.
   //! if translation_scale_factor is negative then a sensible default value will be used.
   //! @return a value less than -99.9 on failure to fit.
   float fit_to_map_by_random_jiggle(int imol, const coot::residue_spec_t &res_spec, int n_trials, float translation_scale_factor);

   //! "Jiggle-Fit Ligand" with a different interface - one that can use any atom selection (instead of just a ligand).
   //! As above, if n_trials is 0, then a sensible default value will be used.
   //! if translation_scale_factor is negative then a sensible default value will be used.
   //! @return a value less than -99.9 on failure to fit.
   float fit_to_map_by_random_jiggle_using_cid(int imol, const std::string &cid, int n_trials, float translation_scale_factor);

   //! Jiggle fit an atom selection, typically a whole molecule or a chain
   //! As above, if n_trials is 0, then a sensible default value will be used.
   //! if translation_scale_factor is negative then a sensible default value will be used.
   //! @return a value less than -99.9 on failure to fit.
   float fit_to_map_by_random_jiggle_with_blur_using_cid(int imol, int imol_map, const std::string &cid, float b_factor,
                                                         int n_trials, float translation_scale_factor);

   //! This is a ligand function, not really a ligand-fitting function.
   //!
   //! It won't work unless the dictionary for that ligand has been imported.
   //! The output renderings are not very good at the moment.
   //!
   //! Except for unusual cases, ``imol`` will be IMOL_ENC_ANY (-666666)
   //!
   //! ``dark_background_flag`` returns a representation suitable for rendering on a dark background (funnily enough).
   //!
   //! This function is not const because it caches the svgs if it can.
   //!
   //! @return the string for the SVG representation.
   std::string get_svg_for_residue_type(int imol, const std::string &comp_id, bool use_rdkit_svg, bool dark_background_flag);

   //! This function is for adding compounds/molecules like buffer agents and precipitants or anions and cations.
   //! _i.e._ those ligands that can be positioned without need for internal torsion angle manipulation.
   //!
   //! ``tlc`` is the three-letter-code/compound-id
   //!
   //! ``imol_dict`` is the molecule to which the ligand is attached (if any). Typically this will be IMOL_ENC_ANY (-666666).
   //!
   //! ``imol_map`` is the molecule number of the map that will be used for fitting.
   //!
   //! @return the success status, 1 or good, 0 for not good.
   int add_compound(int imol, const std::string &tlc, int imol_dict, int imol_map, float x, float y, float z);

   //! @return a vector of residue specifiers for the ligand residues - the residue name is encoded
   //! in the `string_user_data` data item of the residue specifier
   std::vector<coot::residue_spec_t> get_non_standard_residues_in_molecule(int imol) const;

   //! Get the conformers that can be generated by variation around rotatable bonds as described in the dictionary.
   //! Torsions that are marked as "const" are excluded from the variation, as are pyranose ring torsions
   //! and torsions that rotate hydrogen atoms.
   //! @return a vector of indices of the new molecules
   std::vector<int> get_dictionary_conformers(const std::string &comp_id, int imol_enc, bool remove_internal_clash_conformers);

   //! The new arguments, `data_value_for_top`, `data_value_for_bottom` should be pre-calculated (don't
   //! calculate them for every call to this function).
   //! @return a texture_as_floats_t object for the given section
   //! On failure, the image_data vector is empty.
   texture_as_floats_t get_map_section_texture(int imol, int section_id, int axis,
                                               float data_value_for_bottom, float data_value_for_top) const;

   //! @return the number of section in the map along the give axis.
   //! (0 for X-axis, 1 for y-axis, 2 for Z-axis).
   //! return -1 on failure.
   int get_number_of_map_sections(int imol_map, int axis_id) const;

   // -------------------------------- Others -------------------------------------
   //! \name Other Features

   //! Make a m `coot::simple_mesh_t` from a file
   //!
   //! @params `file_name` the gltf file
   //! @return a `simple_mesh_t` from the given file.
   coot::simple_mesh_t make_mesh_from_gltf_file(const std::string &file_name);

   //! @params `n_divisions` is a number divisble by 2, at least 4 (typically 16)
   //! @return a unit-vector end-cap octohemisphere mesh
   coot::simple_mesh_t get_octahemisphere(unsigned int n_divisions) const;

   //! @return a string of a png
   std::string pae_png(const std::string &pae_file_name) const;

   // -------------------------------- Testing -------------------------------------
   //! \name Testing functions

   class ltj_stats_t {
   public:
      unsigned int count;
      float function_value;
      std::chrono::time_point<std::chrono::high_resolution_clock> timer_start;
      std::chrono::time_point<std::chrono::high_resolution_clock> timer;
      ltj_stats_t() : timer(std::chrono::high_resolution_clock::now()) {
         count = 0;
         function_value = 0;
         timer_start = timer;
      }
      //! This function is called by the long-term job, udating the timer and count
      void update_timer() {
         timer = std::chrono::high_resolution_clock::now();
         count += 1;
      }
      //! This function is called by the interrogation function - and is to help
      //! the uer know how the job is going.
      //!
      //! 20230127-PE A nice graph of the change of the function value seems like a good idea
      double time_difference() {
         timer = std::chrono::high_resolution_clock::now();
         auto d10 = std::chrono::duration_cast<std::chrono::microseconds>(timer - timer_start).count();
         return d10;
      }
   };

   //! long term job
   ltj_stats_t long_term_job_stats;

   // not for user-control
   bool interrupt_long_term_job;

   //! start a long-term job.
   //!
   //! if `n_seconds` is 0, then run forever (or until interrupted)
   //!
   void testing_start_long_term_job(unsigned int n_seconds);

   //! stop the long-term job runnning (testing function)
   void testing_stop_long_term_job();

   //! get the stats for the long-term job (testing function)
   ltj_stats_t testing_interrogate_long_term_job() { return long_term_job_stats; }

   //! get the time for conntouring in milliseconds
   double get_contouring_time() const { return contouring_time; }

   //! set the maximum number of threads for both the thread pool and the vector of threads
   void set_max_number_of_threads(unsigned int n_threads);

   //! deprecated name for the above function
   void set_max_number_of_threads_in_thread_pool(unsigned int n_threads);

   //! get the time to run a test function in milliseconds
   double test_the_threading(int n_threads);

   //! @return the time per batch in microseconds
   double test_launching_threads(unsigned int n_threads_per_batch, unsigned int n_batches) const;

   //! @return time in microsections
   double test_thread_pool_threads(unsigned int n_threads);

   //! a test for mmdb/gemmi/mmcif functionality
   //
   //! @param last_test_only is `true` to mean that only that last test should be run.
   //! The default is `false`.
   //! This is useful to set to `true` while a test is being developed.
   //!
   //! @return the success status: 1 means that all the tests passed.
   int mmcif_tests(bool last_test_only);

   // get acces to protein geometry
   coot::protein_geometry & get_geometry() {
      return geom;
   }


   // -------------------------------- Blender Interface ---------------------------------------

   //! \name Functions for Blender Interface

   void make_mesh_for_map_contours_for_blender(int imol, float x, float y, float z, float level, float radius);
   void make_mesh_for_bonds_for_blender(int imol, const std::string &mode, bool against_a_dark_background,
                                      float bond_width, float atom_radius_to_bond_width_ratio,
                                      int smoothness_factor);
   void make_mesh_for_molecular_representation_for_blender(int imol,
                                                           const std::string &cid,
                                                           const std::string &colour_scheme,
                                                           const std::string &style,
                                                           int secondary_structure_usage_flag);
   void make_mesh_for_gaussian_surface_for_blender(int imol, float sigma, float contour_level, float box_radius, float grid_scale, float b_factor);

   void make_mesh_for_goodsell_style_for_blender(int imol, float colour_wheel_rotation_step,
                                                 float saturation, float goodselliness);

   std::vector<float> get_colour_table_for_blender(int imol);
   std::vector<float> get_vertices_for_blender(int imol);
   std::vector<int>   get_triangles_for_blender(int imol);

   // -------------------------------- Other ---------------------------------------

#ifdef SWIG
#if NB_VERSION_MAJOR
   // skip this (old) block for nanobinds
#else
#ifdef DOXYGEN_SHOULD_PARSE_THIS
#else
   //! \name Old Python functions

   //! old mesh mode: do not use with nanobind
   enum mesh_mode_t { UNKNOWN, SINGLE_COLOUR, MULTI_COLOUR };
   //! old function: do not use with nanobind
   PyObject *simple_mesh_to_pythonic_mesh(const coot::simple_mesh_t &mesh, int mesh_mode);
   //! old function: do not use with nanobind
   PyObject *get_pythonic_bonds_mesh(int imol, const std::string &mode, bool against_a_dark_background,
                                     float bond_width, float atom_radius_to_bond_width_ratio,
                                     int smoothness_factor);
   //! old function: do not use with nanobind
   PyObject *get_pythonic_map_mesh(int imol, float x, float y, float z, float radius, float contour_level);
   //! old function: do not use with nanobind
   PyObject *get_pythonic_molecular_representation_mesh(int imol, const std::string &atom_selection,
                                                        const std::string &colour_sheme,
                                                        const std::string &style,
                                                        int secondary_structure_usage_flag);
   //! old function: do not use with nanobind get Gaussion surface mesh
   PyObject *get_pythonic_gaussian_surface_mesh(int imol, float sigma, float contour_level,
                                                float box_radius, float grid_scale, float fft_b_factor);
   //! old function: do not use with nanobind: get a pythonic mesh of the molecule (bonds)
   //!
   //! @return a pair - the first of which (index 0) is the list of atoms, the second (index 1) is the list of bonds.
   //! An atom is a list:
   //!
   //! 0: atom-name
   //!
   //! 1: atom-element
   //!
   //! 2: position (a list of 3 floats)
   //!
   //! 3: formal charge (an integer)
   //!
   //! 4: aromaticity flag (boolean)
   //!
   //! make a "proper" simple  molecule python class one day.
   PyObject *get_pythonic_simple_molecule(int imol, const std::string &cid, bool include_hydrogen_atoms_flag);

#endif
#endif
#endif

};

#endif // MOLECULES_CONTAINER_HH
