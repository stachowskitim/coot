/*
 * ideal/neutron.cc
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
 * General Public License for more details.
 *
 * You should have received a copies of the GNU General Public License and
 * the GNU Lesser General Public License along with this program; if not,
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA, 02110-1301, USA.
 * See http://www.gnu.org/licenses/
 */


#include "simple-restraint.hh"

void
coot::restraints_container_t::init_neutron_occupancies() {
   
   neutron_occupancy_map["H"] =  -0.374000;
   neutron_occupancy_map["D"] =  0.667000;
   neutron_occupancy_map["H-1"] =  -0.374000;
   neutron_occupancy_map["He"] =  0.326000;
   neutron_occupancy_map["Li"] =  -0.190000;
   neutron_occupancy_map["Li+1"] =  -0.190000;
   neutron_occupancy_map["Be"] =  0.779000;
   neutron_occupancy_map["Be+2"] =  0.779000;
   neutron_occupancy_map["B"] =  0.530000;
   neutron_occupancy_map["C"] =  0.665000;
   neutron_occupancy_map["Cv"] =  0.665000;
   neutron_occupancy_map["N"] =  0.936000;
   neutron_occupancy_map["O"] =  0.581000;
   neutron_occupancy_map["O-1"] =  0.581000;
   neutron_occupancy_map["F"] =  0.565400;
   neutron_occupancy_map["F-1"] =  0.565400;
   neutron_occupancy_map["Ne"] =  0.456600;
   neutron_occupancy_map["Na"] =  0.363000;
   neutron_occupancy_map["Na+1"] =  0.363000;
   neutron_occupancy_map["Mg"] =  0.537500;
   neutron_occupancy_map["Mg+2"] =  0.537500;
   neutron_occupancy_map["Al"] =  0.344900;
   neutron_occupancy_map["Al+3"] =  0.344900;
   neutron_occupancy_map["Si"] =  0.414910;
   neutron_occupancy_map["Siv"] =  0.414910;
   neutron_occupancy_map["Si+4"] =  0.414910;
   neutron_occupancy_map["P"] =  0.513000;
   neutron_occupancy_map["S"] =  0.284700;
   neutron_occupancy_map["Cl"] =  0.957700;
   neutron_occupancy_map["Cl-1"] =  0.957700;
   neutron_occupancy_map["Ar"] =  0.190900;
   neutron_occupancy_map["K"] =  0.367000;
   neutron_occupancy_map["K+1"] =  0.367000;
   neutron_occupancy_map["Ca"] =  0.470000;
   neutron_occupancy_map["Ca+2"] =  0.470000;
   neutron_occupancy_map["Sc"] =  1.229000;
   neutron_occupancy_map["Sc+3"] =  1.229000;
   neutron_occupancy_map["Ti"] =  -0.343800;
   neutron_occupancy_map["Ti+2"] =  -0.343800;
   neutron_occupancy_map["Ti+3"] =  -0.343800;
   neutron_occupancy_map["Ti+4"] =  -0.343800;
   neutron_occupancy_map["V"] =  -0.382400;
   neutron_occupancy_map["V+2"] =  -0.382400;
   neutron_occupancy_map["V+3"] =  -0.382400;
   neutron_occupancy_map["V+5"] =  -0.382400;
   neutron_occupancy_map["Cr"] =  0.363500;
   neutron_occupancy_map["Cr+2"] =  0.363500;
   neutron_occupancy_map["Cr+3"] =  0.363500;
   neutron_occupancy_map["Mn"] =  -0.373000;
   neutron_occupancy_map["Mn+2"] =  -0.373000;
   neutron_occupancy_map["Mn+3"] =  -0.373000;
   neutron_occupancy_map["Mn+4"] =  -0.373000;
   neutron_occupancy_map["Fe"] =  0.945000;
   neutron_occupancy_map["Fe+2"] =  0.945000;
   neutron_occupancy_map["Fe+3"] =  0.945000;
   neutron_occupancy_map["Co"] =  0.249000;
   neutron_occupancy_map["Co+2"] =  0.249000;
   neutron_occupancy_map["Co+3"] =  0.249000;
   neutron_occupancy_map["Ni"] =  1.030000;
   neutron_occupancy_map["Ni+2"] =  1.030000;
   neutron_occupancy_map["Ni+3"] =  1.030000;
   neutron_occupancy_map["Cu"] =  0.771800;
   neutron_occupancy_map["Cu+1"] =  0.771800;
   neutron_occupancy_map["Cu+2"] =  0.771800;
   neutron_occupancy_map["Zn"] =  0.578000;
   neutron_occupancy_map["Zn+2"] =  0.578000;
   neutron_occupancy_map["Ga"] =  0.728800;
   neutron_occupancy_map["Ga+3"] =  0.728800;
   neutron_occupancy_map["Ge"] =  0.818500;
   neutron_occupancy_map["Ge+4"] =  0.818500;
   neutron_occupancy_map["As"] =  0.658000;
   neutron_occupancy_map["Se"] =  0.797000;
   neutron_occupancy_map["Br"] =  0.679500;
   neutron_occupancy_map["Br-1"] =  0.679500;
   neutron_occupancy_map["Rb"] =  0.709000;
   neutron_occupancy_map["Rb+1"] =  0.709000;
   neutron_occupancy_map["Sr"] =  0.702000;
   neutron_occupancy_map["Sr+2"] =  0.702000;
   neutron_occupancy_map["Y"] =  0.775000;
   neutron_occupancy_map["Y+3"] =  0.775000;
   neutron_occupancy_map["Zr"] =  0.716000;
   neutron_occupancy_map["Zr+4"] =  0.716000;
   neutron_occupancy_map["Nb"] =  0.795400;
   neutron_occupancy_map["Nb+3"] =  0.795400;
   neutron_occupancy_map["Nb+5"] =  0.795400;
   neutron_occupancy_map["Mo"] =  0.671500;
   neutron_occupancy_map["Mo+3"] =  0.671500;
   neutron_occupancy_map["Mo+5"] =  0.671500;
   neutron_occupancy_map["Mo+6"] =  0.671500;
   neutron_occupancy_map["Tc"] =  0.680000;
   neutron_occupancy_map["Ru"] =  0.703000;
   neutron_occupancy_map["Ru+3"] =  0.703000;
   neutron_occupancy_map["Ru+4"] =  0.703000;
   neutron_occupancy_map["Rh"] =  0.588000;
   neutron_occupancy_map["Rh+3"] =  0.588000;
   neutron_occupancy_map["Rh+4"] =  0.588000;
   neutron_occupancy_map["Pd"] =  0.591000;
   neutron_occupancy_map["Pd+2"] =  0.591000;
   neutron_occupancy_map["Pd+4"] =  0.592200;
   neutron_occupancy_map["Ag"] =  0.591000;
   neutron_occupancy_map["Ag+1"] =  0.592200;
   neutron_occupancy_map["Ag+2"] =  0.592200;
   neutron_occupancy_map["Cd"] =  0.487000;
   neutron_occupancy_map["Cd+2"] =  0.487000;
   neutron_occupancy_map["In"] =  0.406500;
   neutron_occupancy_map["In+3"] =  0.406500;
   neutron_occupancy_map["Sn"] =  0.622500;
   neutron_occupancy_map["Sn+2"] =  0.622500;
   neutron_occupancy_map["Sn+4"] =  0.622500;
   neutron_occupancy_map["Sb"] =  0.557000;
   neutron_occupancy_map["Sb+3"] =  0.557000;
   neutron_occupancy_map["Sb+5"] =  0.557000;
   neutron_occupancy_map["Te"] =  0.580000;
   neutron_occupancy_map["I"] =  0.528000;
   neutron_occupancy_map["I-1"] =  0.528000;
   neutron_occupancy_map["Xe"] =  0.492000;
   neutron_occupancy_map["Cs"] =  0.542000;
   neutron_occupancy_map["Cs+1"] =  0.542000;
   neutron_occupancy_map["Ba"] =  0.507000;
   neutron_occupancy_map["Ba+2"] =  0.507000;
   neutron_occupancy_map["La"] =  0.824000;
   neutron_occupancy_map["La+3"] =  0.824000;
   neutron_occupancy_map["Ce"] =  0.484000;
   neutron_occupancy_map["Ce+3"] =  0.484000;
   neutron_occupancy_map["Ce+4"] =  0.484000;
   neutron_occupancy_map["Pr"] =  0.458000;
   neutron_occupancy_map["Pr+3"] =  0.458000;
   neutron_occupancy_map["Pr+4"] =  0.458000;
   neutron_occupancy_map["Nd"] =  0.769000;
   neutron_occupancy_map["Nd+3"] =  0.769000;
   neutron_occupancy_map["Pm"] =  1.260000;
   neutron_occupancy_map["Pm+3"] =  1.260000;
   neutron_occupancy_map["Sm"] =  0.08000;
   neutron_occupancy_map["Sm+3"] =  0.080000;
   neutron_occupancy_map["Eu"] =  0.722000;
   neutron_occupancy_map["Eu+2"] =  0.722000;
   neutron_occupancy_map["Eu+3"] =  0.722000;
   neutron_occupancy_map["Gd"] =  0.650000;
   neutron_occupancy_map["Gd+3"] =  0.650000;
   neutron_occupancy_map["Tb"] =  0.738000;
   neutron_occupancy_map["Tb+3"] =  0.738000;
   neutron_occupancy_map["Dy"] =  1.690000;
   neutron_occupancy_map["Dy+3"] =  1.690000;
   neutron_occupancy_map["Ho"] =  0.801000;
   neutron_occupancy_map["Ho+3"] =  0.801000;
   neutron_occupancy_map["Er"] =  0.779000;
   neutron_occupancy_map["Er+3"] =  0.779000;
   neutron_occupancy_map["Tm"] =  0.707000;
   neutron_occupancy_map["Tm+3"] =  0.707000;
   neutron_occupancy_map["Yb"] =  1.243000;
   neutron_occupancy_map["Yb+2"] =  1.243000;
   neutron_occupancy_map["Yb+3"] =  1.243000;
   neutron_occupancy_map["Lu"] =  0.721000;
   neutron_occupancy_map["Lu+3"] =  0.721000;
   neutron_occupancy_map["Hf"] =  0.770000;
   neutron_occupancy_map["Hf+4"] =  0.770000;
   neutron_occupancy_map["Ta"] =  0.691000;
   neutron_occupancy_map["Ta+5"] =  0.691000;
   neutron_occupancy_map["W"] =  0.486000;
   neutron_occupancy_map["W+6"] =  0.486000;
   neutron_occupancy_map["Re"] =  0.920000;
   neutron_occupancy_map["Os"] =  1.070000;
   neutron_occupancy_map["Os+4"] =  1.070000;
   neutron_occupancy_map["Ir"] =  1.060000;
   neutron_occupancy_map["Ir+3"] =  1.060000;
   neutron_occupancy_map["Ir+4"] =  1.060000;
   neutron_occupancy_map["Pt"] =  0.960000;
   neutron_occupancy_map["Pt+2"] =  0.960000;
   neutron_occupancy_map["Pt+4"] =  0.960000;
   neutron_occupancy_map["Au"] =  0.763000;
   neutron_occupancy_map["Au+1"] =  0.763000;
   neutron_occupancy_map["Au+3"] =  0.763000;
   neutron_occupancy_map["Hg"] =  1.269200;
   neutron_occupancy_map["Hg+1"] =  1.269200;
   neutron_occupancy_map["Hg+2"] =  1.269200;
   neutron_occupancy_map["Tl"] =  0.877600;
   neutron_occupancy_map["Tl+1"] =  0.877600;
   neutron_occupancy_map["Tl+3"] =  0.877600;
   neutron_occupancy_map["Pb"] =  0.940500;
   neutron_occupancy_map["Pb+2"] =  0.940500;
   neutron_occupancy_map["Pb+4"] =  0.940500;
   neutron_occupancy_map["Bi"] =  0.853200;
   neutron_occupancy_map["Bi+3"] =  0.853200;
   neutron_occupancy_map["Bi+5"] =  0.853200;
   neutron_occupancy_map["Ra"] =  1.00000;
   neutron_occupancy_map["Ra+2"] =  1.000000;
   neutron_occupancy_map["Th"] =  1.031000;
   neutron_occupancy_map["Th+4"] =  1.031000;
   neutron_occupancy_map["Pa"] =  0.910000;
   neutron_occupancy_map["U"] =  1.010000;
   neutron_occupancy_map["U+3"] =  1.010000;
   neutron_occupancy_map["U+4"] =  1.010000;
   neutron_occupancy_map["U+6"] =  1.010000;
   neutron_occupancy_map["Np"] =  1.055000;
   neutron_occupancy_map["Np+3"] =  1.055000;
   neutron_occupancy_map["Np+4"] =  1.055000;
   neutron_occupancy_map["Np+6"] =  1.055000;
   neutron_occupancy_map["Am"] =  0.830000;

}
