def res_name_from_atom_spec(atom_spec):
  imol = atom_spec[1]
  res_name = residue_name(imol,
                          atom_spec[2],
                          atom_spec[3],
                          atom_spec[4])
  return res_name

def user_defined_add_single_bond_restraint():
  
  add_status_bar_text("Click on 2 atoms to define the additional bond restraint")
  def make_restr(*args):
    atom_spec_1 = args[0]
    atom_spec_2 = args[1]
    imol = atom_spec_1[1]
    print("BL DEBUG:: imol: %s spec 1: %s and 2: %s" %(imol, atom_spec_1, atom_spec_2))
    add_extra_bond_restraint(imol,
                             atom_spec_1[2],
                             atom_spec_1[3],
                             atom_spec_1[4],
                             atom_spec_1[5],
                             atom_spec_1[6],
                             atom_spec_2[2],
                             atom_spec_2[3],
                             atom_spec_2[4],
                             atom_spec_2[5],
                             atom_spec_2[6],
                             1.54, 0.02)  # generic distance?!
    
  user_defined_click(2, make_restr)

  
def user_defined_add_arbitrary_length_bond_restraint(bond_length=2.0):

  # maybe make a generic one....
  def make_restr(text_list, continue_qm):
    s = "Now click on 2 atoms to define the additional bond restraint"
    add_status_bar_text(s)
    dist = text_list[0]
    try:
      bl = float(dist)
    except:
      bl = False
      add_status_bar_text("Must define a number for the bond length")
    if bl:
      # save distance for future use?!
      def make_restr_dist(*args):
            atom_spec_1 = args[0]
            atom_spec_2 = args[1]
            imol = atom_spec_1[1]
            print("BL DEBUG:: imol: %s spec 1: %s and 2: %s" %(imol, atom_spec_1, atom_spec_2))
            add_extra_bond_restraint(imol,
                                     atom_spec_1[2],
                                     atom_spec_1[3],
                                     atom_spec_1[4],
                                     atom_spec_1[5],
                                     atom_spec_1[6],
                                     atom_spec_2[2],
                                     atom_spec_2[3],
                                     atom_spec_2[4],
                                     atom_spec_2[5],
                                     atom_spec_2[6],
                                     bl, 0.035)
      user_defined_click(2, make_restr_dist)
      if continue_qm:
        user_defined_add_arbitrary_length_bond_restraint(bl)
      
  def stay_open(*args):
    pass
  #generic_single_entry("Add a User-defined extra distance restraint",
  #                     "2.0",
  #                     "OK...",
  #                     lambda text: make_restr(text))
  coot_gui.generic_multiple_entries_with_check_button(
    [["Add a User-defined extra distance restraint",
      str(bond_length)]],
    ["Stay open?", lambda active_state: stay_open(active_state)],
    "OK...",
    lambda text, stay_open_qm: make_restr(text, stay_open_qm))

# spec_1 and spec_2 are 7-element coot_utils.atom_specs
#
def add_base_restraint(imol, spec_1, spec_2, atom_name_1, atom_name_2, dist):

  """
  spec_1 and spec_2 are 7-element coot_utils.atom_specs
  """

  print("add_base_restraint", imol, spec_1, spec_2, atom_name_1, atom_name_2, dist)
  
  add_extra_bond_restraint(imol,
                           spec_1[2],
                           spec_1[3],
                           spec_1[4],
                           atom_name_1,
                           spec_1[6],
                           spec_2[2],
                           spec_2[3],
                           spec_2[4],
                           atom_name_2,
                           spec_2[6],
                           dist, 0.035)

def a_u_restraints(spec_1, spec_2):

  imol = spec_1[1]
  print("BL DEBUG:: add_base_restraint a u",imol, spec_1, spec_2, " N6 ", " O4 ", 3.12)
  add_base_restraint(imol, spec_1, spec_2, " N6 ", " O4 ", 3.12)
  add_base_restraint(imol, spec_1, spec_2, " N1 ", " N3 ", 3.05)
  add_base_restraint(imol, spec_1, spec_2, " C2 ", " O2 ", 3.90)
  add_base_restraint(imol, spec_1, spec_2, " N3 ", " O2 ", 5.12)
  add_base_restraint(imol, spec_1, spec_2, " C6 ", " O4 ", 3.92)
  add_base_restraint(imol, spec_1, spec_2, " C4 ", " C6 ", 8.38)

def g_c_restraints(spec_1, spec_2):

  imol = spec_1[1]
  print("BL DEBUG:: add_base_restraint gc", imol, spec_1, spec_2, " O6 ", " N4 ", 3.08)
  add_base_restraint(imol, spec_1, spec_2, " N6 ", " O4 ", 3.12)
  add_base_restraint(imol, spec_1, spec_2, " N1 ", " N3 ", 3.04)
  add_base_restraint(imol, spec_1, spec_2, " N2 ", " O2 ", 3.14)
  add_base_restraint(imol, spec_1, spec_2, " C4 ", " N1 ", 7.73)
  add_base_restraint(imol, spec_1, spec_2, " C5 ", " C5 ", 7.21)

def dna_a_t_restraints(spec_1, spec_2):

  imol = spec_1[1]
  add_base_restraint(imol, spec_1, spec_2, " C2 ", " O2 ", 3.49)
  add_base_restraint(imol, spec_1, spec_2, " N1 ", " N3 ", 2.85)
  add_base_restraint(imol, spec_1, spec_2, " N6 ", " O4 ", 3.23)
  add_base_restraint(imol, spec_1, spec_2, " C6 ", " C8 ", 9.94)
  add_base_restraint(imol, spec_1, spec_2, " N1 ", " O2 ", 3.52)

def dna_g_c_restraints(spec_1, spec_2):

  imol = spec_1[1]
  add_base_restraint(imol, spec_1, spec_2, " O6 ", " N4 ", 2.72)
  add_base_restraint(imol, spec_1, spec_2, " N1 ", " N3 ", 2.81)
  add_base_restraint(imol, spec_1, spec_2, " N2 ", " O2 ", 2.83)
  add_base_restraint(imol, spec_1, spec_2, " N9 ", " N1 ", 8.83)

  
def user_defined_RNA_A_form():
  def make_restr(*args):
    spec_1 = args[0]
    spec_2 = args[1]
    print("BL DEBUG:: have specs", spec_1, spec_2)
    res_name_1 = res_name_from_atom_spec(spec_1)
    res_name_2 = res_name_from_atom_spec(spec_2)
    print("BL DEBUG:: have resnames", res_name_1, res_name_2)
    # just check the first letter, should be save
    if (res_name_1[0] == "G" and
        res_name_2[0] == "C"):
      g_c_restraints(spec_1, spec_2)

    if (res_name_1[0] == "C" and
        res_name_2[0] == "G"):
      g_c_restraints(spec_2, spec_1)

    if (res_name_1[0] == "A" and
        res_name_2[0] == "U"):
      a_u_restraints(spec_1, spec_2)

    if (res_name_1[0] == "U" and
        res_name_2[0] == "A"):
      a_u_restraints(spec_2, spec_1)

  user_defined_click(2, make_restr)

def user_defined_DNA_B_form():
  def make_restr(*args):
    spec_1 = args[0]
    spec_2 = args[1]
    res_name_1 = res_name_from_atom_spec(spec_1)
    res_name_2 = res_name_from_atom_spec(spec_2)
    if (res_name_1 == "DG" and
        res_name_2 == "DC"):
      dna_g_c_restraints(spec_1, spec_2)

    if (res_name_1 == "DC" and
        res_name_2 == "DG"):
      dna_g_c_restraints(spec_2, spec_1)

    if (res_name_1 == "DA" and
        res_name_2 == "DT"):
      dna_a_t_restraints(spec_1, spec_2)

    if (res_name_1 == "DT" and
        res_name_2 == "DA"):
      dna_a_t_restraints(spec_2, spec_1)

  user_defined_click(2, make_restr)

    
def user_defined_add_helix_restraints():
  def make_restr(*args):
    spec_1 = args[0]
    spec_2 = args[1]
    chain_id_1 = spec_1[2]
    chain_id_2 = spec_2[2]
    res_no_1   = spec_1[3]
    res_no_2   = spec_2[3]
    imol       = spec_1[1]

    if (chain_id_1 == chain_id_2):
      # if backwards, swap 'em
      if res_no_2 < res_no_1:
        tmp = res_no_1
        res_no_1 = res_no_2
        res_no_2 = tmp
      for rn in range(res_no_1, res_no_2 - 2):
        add_extra_bond_restraint(imol,
                                 chain_id_1, rn    , "", " O  ", "",
                                 chain_id_1, rn + 3, "", " N  ", "",
                                 3.18, 0.035)
        if (rn + 4 <= res_no_2):
          add_extra_bond_restraint(imol,
                                   chain_id_1, rn    , "", " O  ", "",
                                   chain_id_1, rn + 4, "", " N  ", "",
                                   2.91, 0.035)
        
  user_defined_click(2, make_restr)

def user_defined_delete_restraint():
  def del_restr(*args):
    spec_1 = args[0]
    spec_2 = args[1]
    imol   = spec_1[1]
    delete_extra_restraint(imol, ["bond", spec_1[2:], spec_2[2:]])
  
  user_defined_click(2, del_restr)


# exte dist first chain A resi 19 ins . atom  N   second chain A resi 19 ins . atom  OG  value 2.70618 sigma 0.4
#
def extra_restraints2refmac_restraints_file(imol, file_name):
  restraints = list_extra_restraints(imol)
  
  if restraints:
    fin = open(file_name, 'w')
    for restraint in restraints:
      if (restraint[0] == 'bond'):
        chain_id_1 = restraint[1][1]
        resno_1    = restraint[1][2]
        inscode_1  = restraint[1][3]
        atom_1     = restraint[1][4]
        chain_id_2 = restraint[2][1]
        resno_2    = restraint[2][2]
        inscode_2  = restraint[2][3]
        atom_2     = restraint[2][4]
        value      = restraint[3]
        esd        = restraint[4]
        fin.write("EXTE DIST FIRST CHAIN %s RESI %i INS %s ATOM %s " \
                  %(chain_id_1 if (chain_id_1 != "" and chain_id_1 != " ") else ".",
                    resno_1,
                    inscode_1 if (inscode_1 != "" and inscode_1 != " ") else ".",
                    atom_1))
        fin.write(" SECOND CHAIN %s RESI %i INS %s ATOM %s " \
                  %(chain_id_2 if (chain_id_2 != "" and chain_id_2 != " ") else ".",
                    resno_2,
                    inscode_2 if (inscode_2 != "" and inscode_2 != " ") else ".",
                    atom_2))
        fin.write("VALUE %f SIGMA %f\n" %(value, esd))
    fin.close()

# target is my molecule, ref is the homologous (high-res) model
#
# extra arg: include_side_chains=False
#
def run_prosmart(imol_target, imol_ref, include_side_chains=False):
  """
  target is my molecule, ref is the homologous (high-res) model

  extra arg: include_side_chains=False
  """

  dir_stub = "coot-ccp4"
  make_directory_maybe(dir_stub)
  target_pdb_file_name = os.path.join(dir_stub,
                                      molecule_name_stub(imol_target, 0).replace(" ", "_") + \
                                      "-prosmart.pdb")
  reference_pdb_file_name = os.path.join(dir_stub,
                                         molecule_name_stub(imol_ref, 0).replace(" ", "_") + \
                                         "-prosmart-ref.pdb")
  prosmart_out = os.path.join("ProSMART_Output",
                              molecule_name_stub(imol_target, 0).replace(" ", "_") + \
                              "-prosmart.txt")

  write_pdb_file(imol_target, target_pdb_file_name)
  write_pdb_file(imol_ref, reference_pdb_file_name)
  prosmart_exe = coot_utils.find_exe("prosmart")
  if prosmart_exe:
    l = ["-p1", target_pdb_file_name,
         "-p2", reference_pdb_file_name,
         "-restrain_seqid", "30"]
    if include_side_chains:
      l += ["-side"]
    coot_utils.popen_command(prosmart_exe,
                  l,
                  [],
                  os.path.join(dir_stub, "prosmart.log"),
                  False)
    if (not os.path.isfile(prosmart_out)):
      print("file not found", prosmart_out)
    else:
      print("Reading ProSMART restraints from", prosmart_out)
      add_refmac_extra_restraints(imol_target, prosmart_out)

def res_name2plane_atom_name_list(res_name):

  if not (isinstance(res_name, str)):
    return False
  else:
    if (res_name == "DG"):
      return ["N1", "C6", "O6", "C2", "N2", "N3", "C5", "C4", "N9", "N7", "C8", "C1'"]
    elif (res_name == "DA"):
      return ["N1", "C6", "C2", "N6", "N3", "C5", "C4", "N9", "N7", "C8", "C1'"]
    elif (res_name == "DT"):
      return ["N3", "C2", "O2", "C4", "O4", "C5", "C7", "C6", "N1", "C1'"]
    elif (res_name == "DC"):
      return ["N3", "C2", "O2", "C4", "N4", "C5", "C6", "N1", "C1'"]

    elif (res_name == "G"):
      return ["N1", "C6", "O6", "C2", "N2", "N3", "C5", "C4", "N9", "N7", "C8", "C1'"]
    elif (res_name == "A"):
      return ["N1", "C6", "C2", "N6", "N3", "C5", "C4", "N9", "N7", "C8", "C1'"]
    elif (res_name == "T"):
      return ["N3", "C2", "O2", "C4", "O4", "C5", "C7", "C6", "N1", "C1'"]
    elif (res_name == "U"):
      return ["N3", "C2", "O2", "C4", "O4", "C5", "C6", "N1", "C1'"]
    elif (res_name == "C"):
      return ["N3", "C2", "O2", "C4", "N4", "C5", "C6", "N1", "C1'"]
    else:
      return []


# example?
# exte stac plan 1  firs resi 99 chai A atoms { CB CG CD1 CD2 CE1 CE2 CZ OH }  plan 2  firs resi 61 chai B atoms { CB CG CD1 CD2 CE1 CE2 CZ }  dist 3.4 sddi 0.2  sdan 6.0 type 1
#
def write_refmac_parallel_plane_restraint(file_name,
                                          res_spec_0, res_spec_1,
                                          atom_list_0, atom_list_1):

  fout = open(file_name, 'w')
  fout.write("EXTE STACK PLAN 1 FIRST RESIDUE ")
  fout.write(str(residue_spec_to_res_no(res_spec_0)))
  fout.write(" INS . ") # hack
  fout.write(" CHAIN ")
  fout.write(residue_spec_to_chain_id(res_spec_0))
  fout.write(" ATOMS { ")
  for atom_name in atom_list_0:
    fout.write(" " + atom_name + " ")
  fout.write(" } PLAN 2 FIRST RESIDUE ")
  fout.write(str(residue_spec_to_res_no(res_spec_1)))
  fout.write(" INS . ") # hack
  fout.write(" CHAIN ")
  fout.write(residue_spec_to_chain_id(res_spec_1))
  fout.write(" ATOMS { ")
  for atom_name in atom_list_1:
    fout.write(" " + atom_name + " ")
  fout.write(" } DIST 3.4 SDDI 0.2 SDAN 6.0 TYPE 1 \n")
  fout.close()
  # maybe some return value that it worked to write at some point


def add_parallel_planes_restraint(imol, rs_0, rs_1):

  print("in add_parallel_planes_restraint: rs_0: %s rs_1 %s" %(rs_0, rs_1))

  rn_0 = coot_utils.residue_spec_to_residue_name(imol, rs_0)
  rn_1 = coot_utils.residue_spec_to_residue_name(imol, rs_1)
  atom_ls_0 = res_name2plane_atom_name_list(rn_0)
  atom_ls_1 = res_name2plane_atom_name_list(rn_1)

  write_refmac_parallel_plane_restraint("tmp.rst", rs_0, rs_1, atom_ls_0, atom_ls_1)

  add_refmac_extra_restraints(imol, "tmp.rst")


def user_defined_add_planes_restraint():

  add_status_bar_text("Click on 2 atoms to define the additional parallel planes restraint")
  
  def make_restr(*args):
    atom_0 = args[0]
    atom_1 = args[1]
    rs_0 = coot_utils.atom_spec_to_residue_spec(atom_0)
    rs_1 = coot_utils.atom_spec_to_residue_spec(atom_1)
    imol = coot_utils.atom_spec_to_imol(atom_0)

    rn_0 = residue_name(imol,
                        res_spec_utils.residue_spec_to_chain_id(atom_spec_to_residue_spec(atom_0)),
                        res_spec_utils.residue_spec_to_res_no(atom_spec_to_residue_spec(atom_0)),
                        coot_utils.residue_spec_to_ins_code(atom_spec_to_residue_spec(atom_0)))
    rn_1 = residue_name(imol,
                        res_spec_utils.residue_spec_to_chain_id(atom_spec_to_residue_spec(atom_1)),
                        res_spec_utils.residue_spec_to_res_no(atom_spec_to_residue_spec(atom_1)),
                        coot_utils.residue_spec_to_ins_code(atom_spec_to_residue_spec(atom_1)))

    print("BL DEBUG:: got resname 0", rn_0)
    print("BL DEBUG:: got resname 1", rn_1)

    atom_ls_0 = res_name2plane_atom_name_list(rn_0)
    atom_ls_1 = res_name2plane_atom_name_list(rn_1)

    write_refmac_parallel_plane_restraint("tmp.rst",
                                          rs_0, rs_1,
                                          atom_ls_0, atom_ls_1)
    add_refmac_extra_restraints(imol, "tmp.rst")
    
  user_defined_click(2, make_restr)
    
if True:
  if coot_gui_api.main_menubar():
    
    menu = coot_gui.coot_menubar_menu("Restraints")

    coot_gui.add_simple_coot_menu_menuitem(
      menu,
      "Add Simple C-C Single Bond Restraint...",
      lambda func: user_defined_add_single_bond_restraint())

    coot_gui.add_simple_coot_menu_menuitem(
      menu,
      "Add Distance Restraint...",
      lambda func: user_defined_add_arbitrary_length_bond_restraint())

    coot_gui.add_simple_coot_menu_menuitem(
      menu,
      "Add Helix Restraints...",
      lambda func: user_defined_add_helix_restraints())

    coot_gui.add_simple_coot_menu_menuitem(
      menu,
      "RNA A form bond restraints...",
      lambda func: user_defined_RNA_A_form())

    coot_gui.add_simple_coot_menu_menuitem(
      menu,
      "DNA B form bond restraints...",
      lambda func: user_defined_DNA_B_form())

    def launch_prosmart_gui():
      def go_button_cb(*args):
        imol_tar = coot_gui.get_option_menu_active_molecule(*option_menu_mol_list_pair_tar)
        imol_ref = coot_gui.get_option_menu_active_molecule(*option_menu_mol_list_pair_ref)
        do_side_chains = check_button.get_active()
        run_prosmart(imol_tar, imol_ref, do_side_chains)
        window.destroy()
      window = gtk.Window(gtk.WINDOW_TOPLEVEL)
      hbox = gtk.HBox(False, 0)
      vbox = gtk.VBox(False, 0)
      h_sep = gtk.HSeparator()
      chooser_hint_text_1 = " Target molecule "
      chooser_hint_text_2 = " Reference (high-res) molecule "
      go_button = gtk.Button(" ProSMART ")
      cancel_button = gtk.Button("  Cancel  ")
      check_button = gtk.CheckButton("Include Side-chains")

      option_menu_mol_list_pair_tar = coot_gui.generic_molecule_chooser(vbox,
                                                               chooser_hint_text_1)
      option_menu_mol_list_pair_ref = coot_gui.generic_molecule_chooser(vbox,
                                                               chooser_hint_text_2)

      vbox.pack_start(check_button, False, False, 2)
      vbox.pack_start(h_sep, False, False, 2)
      vbox.pack_start(hbox, False, False, 2)
      hbox.pack_start(go_button, False, False, 6)
      hbox.pack_start(cancel_button, False, False, 6)
      window.add(vbox)

      cancel_button.connect("clicked", lambda w: window.destroy())

      go_button.connect("clicked", go_button_cb, option_menu_mol_list_pair_tar,
                        option_menu_mol_list_pair_ref)
      window.show_all()
      
    coot_gui.add_simple_coot_menu_menuitem(
      menu,
      "ProSMART...",
      lambda func: launch_prosmart_gui()
      )


    coot_gui.add_simple_coot_menu_menuitem(
      menu,
      "Read Refmac Extra Restraints...",
      lambda func:
      coot_gui.generic_chooser_and_file_selector("Apply restraints to molecule",
                                        coot_utils.valid_model_molecule_qm,
                                        "File:", "",
                                        lambda imol, file_name:
                                        add_refmac_extra_restraints(imol, file_name)))

    coot_gui.add_simple_coot_menu_menuitem(
      menu,
      "Delete Extra Restraints...",
      lambda func:
      coot_gui.molecule_chooser_gui("Delete Extra Restraints for Molecule:",
                           lambda imol:
                           delete_all_extra_restraints(imol)))
      

    
    def set_prosmart_sigma_limit_func(low, high):
      with UsingActiveAtom() as [aa_imol, aa_chain_id, aa_res_no,
                                 aa_ins_code, aa_atom_name, aa_alt_conf]:
        set_extra_restraints_prosmart_sigma_limits(aa_imol, low, high)
    
    coot_gui.add_simple_coot_menu_menuitem(
      menu,
      "ProSMART restraints interesting limit to 0.5...",
      lambda func: set_prosmart_sigma_limit_func(-0.5, 0.5)
      )

    coot_gui.add_simple_coot_menu_menuitem(
      menu,
      "ProSMART restraints interesting limit to 2.5...",
      lambda func: set_prosmart_sigma_limit_func(-2.5, 2.5)
      )


    def set_prosmart_display_func(state):
      with UsingActiveAtom() as [aa_imol, aa_chain_id, aa_res_no,
                                 aa_ins_code, aa_atom_name, aa_alt_conf]:
        set_show_extra_restraints(aa_imol, state)
    
    coot_gui.add_simple_coot_menu_menuitem(
      menu,
      "Undisplay Extra Restraints",
      lambda func: set_prosmart_display_func(0)
      )

    coot_gui.add_simple_coot_menu_menuitem(
      menu,
      "Display ProSMART Extra Restraints",
      lambda func: set_prosmart_display_func(1)
      )


    def set_prosmart_display_CA_func(state):
      with UsingActiveAtom() as [aa_imol, aa_chain_id, aa_res_no,
                                 aa_ins_code, aa_atom_name, aa_alt_conf]:
        set_extra_restraints_representation_for_bonds_go_to_CA(aa_imol, state)
      
    coot_gui.add_simple_coot_menu_menuitem(
      menu,
      "Extra Restraints to CA",
      lambda func: set_prosmart_display_CA_func(1))

    coot_gui.add_simple_coot_menu_menuitem(
      menu,
      "Extra Restraints Standard Representation",
      lambda func: set_prosmart_display_CA_func(0))

    coot_gui.add_simple_coot_menu_menuitem(
      menu,
      "Delete an Extra Restraint...",
      lambda func: user_defined_delete_restraint())

    def delete_restraints_func():
      with UsingActiveAtom() as [aa_imol, aa_chain_id, aa_res_no, aa_ins_code, aa_atom_name, aa_alt_conf]:
        delete_extra_restraints_for_residue(aa_imol, aa_chain_id, aa_res_no, aa_ins_code)
        
    coot_gui.add_simple_coot_menu_menuitem(
      menu,
      "Delete Restraints for this residue",
      lambda func: delete_restraints_func()
      )

    def del_deviant_restr_func(text):
      try:
        n = float(text)
        with UsingActiveAtom() as [aa_imol, aa_chain_id, aa_res_no, aa_ins_code, aa_atom_name, aa_alt_conf]:
          delete_extra_restraints_worse_than(aa_imol, n)
      except:
        print("BL WARNING:: no float given")
      
    coot_gui.add_simple_coot_menu_menuitem(
      menu,
      "Delete Deviant Extra Restraints...",
      lambda func: coot_gui.generic_single_entry("Delete Restraints worse than ",
                                        "4.0", " Delete Outlying Restraints ",
                                        lambda text: del_deviant_restr_func(text))
      )
    
    coot_gui.add_simple_coot_menu_menuitem(
      menu,
      "Save as REFMAC restraints...",
      lambda func:
      coot_gui.generic_chooser_and_file_selector("Save REFMAC restraints for molecule",
                                        coot_utils.valid_model_molecule_qm,
                                        " Restraints file name:  ", 
                                        "refmac-restraints.txt",
                                        lambda imol, file_name:
                                          extra_restraints2refmac_restraints_file(imol, file_name)))
    

    coot_gui.add_simple_coot_menu_menuitem(
      menu,
      "Add Parallel Planes Restraint...",
      lambda func:
      user_defined_add_planes_restraint()
      )
  
  


                           
