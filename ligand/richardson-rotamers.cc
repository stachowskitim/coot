/*
 * ligand/richardson-rotamers.cc
 *
 * Copyright 2007 by University of York
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
#include "chi-angles.hh"

void
coot::chi_angles::add_richardson_rotamers() {

       add_richardson_rotamer("ARG", "mmm-85", 22, 2, 2, 3, 3, -62, -62, -64, -68, -61, -65, -82, -85);
       add_richardson_rotamer("ARG", "mmm180", 11, 1, 0, 2, 2, -64, -62, -74, -68, -67, -65, 172, 175);
       add_richardson_rotamer("ARG", "mmt-85", 22, 2, 0, 4, 3, -60, -62, -72, -68, -178, 180, -92, -85);
       add_richardson_rotamer("ARG", "mmt180", 18, 2, 1, 3, 2, -63, -62, -66, -68, -179, 180, -168, 180);
       add_richardson_rotamer("ARG", "mmt85", 7, 1, 1, 1, 1, -5555, -62, -5555, -68, -5555, 180, -5555, 85);
       add_richardson_rotamer("ARG", "mtm-85", 54, 6, 13, 2, 3, -69, -67, -167, -167, -63, -65, -86, -85);
       add_richardson_rotamer("ARG", "mtm180", 48, 5, 1, 4, 8, -68, -67, 173, 180, -64, -65, 180, 175);
       add_richardson_rotamer("ARG", "mtm105", 15, 2, 1, 1, 2, -68, -67, -179, 180, -65, -65, 103, 105);
       add_richardson_rotamer("ARG", "mtt-85", 53, 6, 4, 7, 6, -66, -67, -177, 180, -179, 180, -83, -85);
       add_richardson_rotamer("ARG", "mtt180", 89, 9, 9, 5, 12, -67, -67, -178, 180, -177, 180, 174, 180);
       add_richardson_rotamer("ARG", "mtt85", 34, 4, 4, 4, 3, -67, -67, 178, 180, 179, 180, 83, 85);
       add_richardson_rotamer("ARG", "mtp-105", 7, 1, 0, 2, 1, -62, -67, 179, 180, 67, 65, -113, -105);
       add_richardson_rotamer("ARG", "mtp180", 45, 5, 4, 3, 6, -65, -67, 176, 180, 64, 65, -174, -175);
       add_richardson_rotamer("ARG", "mtp85", 22, 2, 2, 3, 2, -69, -67, 177, 180, 64, 65, 84, 85);
       add_richardson_rotamer("ARG", "ttm-85", 28, 3, 3, 3, 3, -175, -177, -178, 180, -65, -65, -84, -85);
       add_richardson_rotamer("ARG", "ttm180", 13, 1, 0, 4, 1, 180, -177, -178, 180, -67, -65, 176, 175);
       add_richardson_rotamer("ARG", "ttm105", 10, 1, 2, 1, 0, -178, -177, 170, 180, -66, -65, 107, 105);
       add_richardson_rotamer("ARG", "ttt-85", 26, 3, 3, 3, 2, -179, -177, 179, 180, 180, 180, -86, -85);
       add_richardson_rotamer("ARG", "ttt180", 33, 4, 3, 7, 3, -179, -177, 177, 180, -179, 180, 170, 180);
       add_richardson_rotamer("ARG", "ttt85", 19, 2, 2, 2, 2, -175, -177, 176, 180, 179, 180, 83, 85);
       add_richardson_rotamer("ARG", "ttp-105", 9, 1, 1, 1, 1, -5555, -177, -5555, 180, -5555, 65, -5555, -105);
       add_richardson_rotamer("ARG", "ttp180", 25, 3, 5, 3, 1, -178, -177, -178, 180, 65, 65, -162, -175);
       add_richardson_rotamer("ARG", "ttp85", 33, 4, 5, 3, 3, -179, -177, 177, 180, 65, 65, 83, 85);
       add_richardson_rotamer("ARG", "tpt180", 15, 2, 3, 1, 1, 179, -177, 60, 65, 178, 180, 163, 180);
       add_richardson_rotamer("ARG", "tpt85", 20, 2, 3, 2, 2, 177, -177, 64, 65, 180, 180, 86, 85);
       add_richardson_rotamer("ARG", "tpp180", 8, 1, 1, 0, 1, -5555, -177, -5555, 65, -5555, 65, -5555, -175);
       add_richardson_rotamer("ARG", "tpp85", 11, 1, 3, 1, 0, -178, -177, 57, 65, 57, 65, 85, 85);
       add_richardson_rotamer("ARG", "ptm-85", 5, 1, 0, 0, 1, -5555, 62, -5555, 180, -5555, -65, -5555, -85);
       add_richardson_rotamer("ARG", "ptm180", 6, 1, 0, 1, 1, -5555, 62, -5555, 180, -5555, -65, -5555, 175);
       add_richardson_rotamer("ARG", "ptt-85", 15, 2, 1, 2, 2, 66, 62, -176, 180, -178, 180, -83, -85);
       add_richardson_rotamer("ARG", "ptt180", 16, 2, 1, 2, 2, 59, 62, 176, 180, -178, 180, -177, 180);
       add_richardson_rotamer("ARG", "ptt85", 16, 2, 1, 2, 2, 65, 62, -178, 180, -179, 180, 88, 85);
       add_richardson_rotamer("ARG", "ptp180", 11, 1, 0, 2, 2, 71, 62, 171, 180, 65, 65, -161, -175);
       add_richardson_rotamer("ARG", "ptp85", 3, 0, 0, 1, 0, -5555, 62, -5555, 180, -5555, 65, -5555, 85);
       add_richardson_rotamer("LYS", "ptpt", 7, 1, 0, 2, 0, -5555, 62, -5555, 180, -5555, 68, -5555, 180);
       add_richardson_rotamer("LYS", "pttp", 13, 1, 0, 1, 2, 63, 62, 177, 180, 172, 180, 61, 65);
       add_richardson_rotamer("LYS", "pttt", 29, 2, 0, 4, 3, 64, 62, 179, 180, 180, 180, 180, 180);
       add_richardson_rotamer("LYS", "pttm", 8, 1, 0, 1, 1, -5555, 62, -5555, 180, -5555, 180, -5555, -65);
       add_richardson_rotamer("LYS", "ptmt", 5, 0, 0, 1, 0, -5555, 62, -5555, 180, -5555, 68, -5555, 180);
       add_richardson_rotamer("LYS", "tptp", 11, 1, 1, 1, 1, 179, -177, 59, 68, 163, 180, 60, 65);
       add_richardson_rotamer("LYS", "tptt", 32, 3, 5, 1, 2, 179, -177, 62, 68, 173, 180, 171, 180);
       add_richardson_rotamer("LYS", "tptm", 7, 1, 1, 1, 0, -5555, -177, -5555, 68, -5555, 180, -5555, -65);
       add_richardson_rotamer("LYS", "ttpp", 12, 1, 1, 0, 1, -5555, -177, -5555, 180, -5555, 68, -5555, 65);
       add_richardson_rotamer("LYS", "ttpt", 25, 2, 2, 5, 1, -177, -177, 169, 180, 71, 68, 172, 180);
       add_richardson_rotamer("LYS", "tttp", 49, 4, 5, 5, 3, -177, -177, -170, 180, -177, 180, 69, 65);
       add_richardson_rotamer("LYS", "tttt", 162, 13, 17, 19, 10, -177, -177, 178, 180, 179, 180, 180, 180);
       add_richardson_rotamer("LYS", "tttm", 37, 3, 4, 2, 3, -176, -177, -175, 180, -167, 180, -59, -65);
       add_richardson_rotamer("LYS", "ttmt", 20, 2, 2, 4, 1, -174, -177, -160, 180, -54, -68, -174, 180);
       add_richardson_rotamer("LYS", "ttmm", 5, 0, 1, 0, 0, -5555, -177, -5555, 180, -5555, -68, -5555, -65);
       add_richardson_rotamer("LYS", "mptt", 4, 0, 0, 0, 1, -5555, -90, -5555, 68, -5555, 180, -5555, 180);
       add_richardson_rotamer("LYS", "mtpp", 12, 1, 1, 1, 1, -62, -67, -177, 180, 71, 68, 68, 65);
       add_richardson_rotamer("LYS", "mtpt", 38, 3, 4, 2, 3, -69, -67, 174, 180, 65, 68, 175, 180);
       add_richardson_rotamer("LYS", "mttp", 42, 3, 2, 4, 4, -67, -67, -176, 180, 175, 180, 71, 65);
       add_richardson_rotamer("LYS", "mttt", 244, 20, 23, 14, 21, -67, -67, 179, 180, 179, 180, 179, 180);
       add_richardson_rotamer("LYS", "mttm", 56, 5, 3, 5, 6, -65, -67, 180, 180, -178, 180, -63, -65);
       add_richardson_rotamer("LYS", "mtmt", 40, 3, 6, 2, 3, -68, -67, -171, 180, -68, -68, -175, 180);
       add_richardson_rotamer("LYS", "mtmm", 12, 1, 0, 1, 1, -68, -67, -178, 180, -68, -68, -63, -65);
       add_richardson_rotamer("LYS", "mmtp", 9, 1, 0, 0, 1, -5555, -62, -5555, -68, -5555, 180, -5555, 65);
       add_richardson_rotamer("LYS", "mmtt", 77, 6, 3, 5, 8, -61, -62, -64, -68, -177, 180, -179, 180);
       add_richardson_rotamer("LYS", "mmtm", 18, 1, 1, 1, 2, -64, -62, -68, -68, -176, 180, -71, -65);
       add_richardson_rotamer("LYS", "mmmt", 10, 1, 0, 1, 1, -59, -62, -59, -68, -70, -68, -173, 180);
       add_richardson_rotamer("MET", "ptp", 12, 2, 1, 3, 3, 68, 62, -167, 180, 88, 75, 0, 0);
       add_richardson_rotamer("MSE", "ptp", 12, 2, 1, 3, 3, 68, 62, -167, 180, 88, 75, 0, 0);
       add_richardson_rotamer("MET", "ptm", 17, 3, 1, 6, 4, 67, 62, 174, 180, -78, -75, 0, 0);
       add_richardson_rotamer("MSE", "ptm", 17, 3, 1, 6, 4, 67, 62, 174, 180, -78, -75, 0, 0);
       add_richardson_rotamer("MET", "tpp", 30, 5, 8, 2, 5, -177, -177, 66, 65, 75, 75, 0, 0);
       add_richardson_rotamer("MSE", "tpp", 30, 5, 8, 2, 5, -177, -177, 66, 65, 75, 75, 0, 0);
       add_richardson_rotamer("MET", "tpt", 9, 2, 1, 4, 1, 179, -177, 67, 65, -179, 180, 0, 0);
       add_richardson_rotamer("MSE", "tpt", 9, 2, 1, 4, 1, 179, -177, 67, 65, -179, 180, 0, 0);
       add_richardson_rotamer("MET", "ttp", 28, 5, 7, 7, 2, 176, -177, 178, 180, 73, 75, 0, 0);
       add_richardson_rotamer("MSE", "ttp", 28, 5, 7, 7, 2, 176, -177, 178, 180, 73, 75, 0, 0);
       add_richardson_rotamer("MET", "ttt", 17, 3, 5, 2, 2, 180, -177, 171, 180, 174, 180, 0, 0);
       add_richardson_rotamer("MSE", "ttt", 17, 3, 5, 2, 2, 180, -177, 171, 180, 174, 180, 0, 0);
       add_richardson_rotamer("MET", "ttm", 36, 7, 3, 10, 8, -177, -177, 176, 180, -78, -75, 0, 0);
       add_richardson_rotamer("MSE", "ttm", 36, 7, 3, 10, 8, -177, -177, 176, 180, -78, -75, 0, 0);
       add_richardson_rotamer("MET", "mtp", 92, 17, 22, 10, 17, -68, -67, 177, 180, 72, 75, 0, 0);
       add_richardson_rotamer("MSE", "mtp", 92, 17, 22, 10, 17, -68, -67, 177, 180, 72, 75, 0, 0);
       add_richardson_rotamer("MET", "mtt", 43, 8, 9, 8, 7, -67, -67, 177, 180, -178, 180, 0, 0);
       add_richardson_rotamer("MSE", "mtt", 43, 8, 9, 8, 7, -67, -67, 177, 180, -178, 180, 0, 0);
       add_richardson_rotamer("MET", "mtm", 58, 11, 12, 11, 9, -67, -67, -177, 180, -76, -75, 0, 0);
       add_richardson_rotamer("MSE", "mtm", 58, 11, 12, 11, 9, -67, -67, -177, 180, -76, -75, 0, 0);
       add_richardson_rotamer("MET", "mmp", 15, 3, 3, 1, 4, -64, -65, -63, -65, 103, 103, 0, 0);
       add_richardson_rotamer("MSE", "mmp", 15, 3, 3, 1, 4, -64, -65, -63, -65, 103, 103, 0, 0);
       add_richardson_rotamer("MET", "mmt", 10, 2, 0, 2, 3, -63, -65, -64, -65, 180, 180, 0, 0);
       add_richardson_rotamer("MSE", "mmt", 10, 2, 0, 2, 3, -63, -65, -64, -65, 180, 180, 0, 0);
       add_richardson_rotamer("MET", "mmm", 105, 19, 21, 16, 19, -66, -65, -60, -65, -67, -70, 0, 0);
       add_richardson_rotamer("MSE", "mmm", 105, 19, 21, 16, 19, -66, -65, -60, -65, -67, -70, 0, 0);
       add_richardson_rotamer("GLU", "pt-20 ", 80, 5, 1, 9, 7, 63, 62, -175, 180, -18, -20, 0, 0);
       add_richardson_rotamer("GLU", "pm0", 32, 2, 0, 0, 4, 71, 70, -79, -80, 5, 0, 0, 0);
       add_richardson_rotamer("GLU", "tp10", 91, 6, 10, 2, 6, -177, -177, 65, 65, 13, 10, 0, 0);
       add_richardson_rotamer("GLU", "tt 0 ", 350, 24, 25, 42, 18, -177, -177, 178, 180, 2, 0, 0, 0);
       add_richardson_rotamer("GLU", "tm-20", 17, 1, 1, 1, 1, -5555, -177, -5555, -80, -5555, -25, 0, 0);
       add_richardson_rotamer("GLU", "mp0", 88, 6, 0, 2, 10, -65, -65, 85, 85, -3, 0, 0, 0);
       add_richardson_rotamer("GLU", "mt-10 ", 484, 33, 36, 29, 32, -67, -67, 177, 180, -10, -10, 0, 0);
       add_richardson_rotamer("GLU", "mm-40", 197, 13, 19, 7, 12, -65, -65, -58, -65, -40, -40, 0, 0);
       add_richardson_rotamer("GLN", "pt 20 ", 37, 4, 1, 5, 6, 64, 62, 180, 180, 20, 20, 0, 0);
       add_richardson_rotamer("GLN", "pm0", 15, 2, 0, 1, 3, -5555, 70, -5555, -75, -5555, 0, 0, 0);
       add_richardson_rotamer("GLN", "tp-100", 14, 2, 4, 2, 0, -5555, -177, -5555, 65, -5555, -100, 0, 0);
       add_richardson_rotamer("GLN", "tp60", 78, 9, 13, 9, 7, -175, -177, 64, 65, 60, 60, 0, 0);
       add_richardson_rotamer("GLN", "tt 0 ", 140, 16, 16, 29, 12, -174, -177, 173, 180, -5, 0, 0, 0);
       add_richardson_rotamer("GLN", "mp0", 24, 3, 0, 1, 5, -5555, -65, -5555, 85, -5555, 0, 0, 0);
       add_richardson_rotamer("GLN", "mt -30 ", 304, 35, 40, 26, 36, -67, -67, 177, 180, -25, -25, 0, 0);
       add_richardson_rotamer("GLN", "mm-40", 127, 15, 12, 13, 17, -66, -65, -60, -65, -40, -40, 0, 0);
       add_richardson_rotamer("GLN", "mm100", 22, 3, 4, 1, 2, -5555, -65, -5555, -65, -5555, 100, 0, 0);
       add_richardson_rotamer("ASP", "p-10", 203, 10, 1, 2, 13, 61, 62, -4, -10, 0, 0, 0, 0);
       add_richardson_rotamer("ASP", "p30", 194, 9, 1, 5, 12, 65, 62, 9, 30, 0, 0, 0, 0);
       add_richardson_rotamer("ASP", "t 0 ", 438, 21, 8, 44, 20, -176, -177, 1, 0, 0, 0, 0, 0);
       add_richardson_rotamer("ASP", "t70", 118, 6, 11, 7, 4, -179, -177, 65, 65, 0, 0, 0, 0);
       add_richardson_rotamer("ASP", "m-20 ", 1088, 51, 77, 38, 47, -71, -70, -15, -15, 0, 0, 0, 0);
       add_richardson_rotamer("ASN", "p-10", 103, 7, 0, 1, 10, 63, 62, -13, -10, 0, 0, 0, 0);
       add_richardson_rotamer("ASN", "p30", 132, 9, 0, 7, 12, 64, 62, 34, 30, 0, 0, 0, 0);
       add_richardson_rotamer("ASN", "t-20", 177, 12, 5, 21, 12, -174, -174, -20, -20, 0, 0, 0, 0);
       add_richardson_rotamer("ASN", "t30", 228, 15, 13, 18, 15, -168, -177, 31, 30, 0, 0, 0, 0);
       add_richardson_rotamer("ASN", "m-20", 580, 39, 65, 28, 33, -71, -65, -23, -20, 0, 0, 0, 0);
       add_richardson_rotamer("ASN", "m-80", 118, 8, 8, 9, 8, -71, -65, -76, -75, 0, 0, 0, 0);
       add_richardson_rotamer("ASN", "m120", 58, 4, 3, 3, 4, -64, -65, 132, 120, 0, 0, 0, 0);
       add_richardson_rotamer("ILE", "pp", 10, 1, 0, 1, 0, -5555, 62, -5555, 100, 0, 0, 0, 0);
       add_richardson_rotamer("ILE", "pt", 216, 13, 4, 13, 22, 61, 62, 171, 170, 0, 0, 0, 0);
       add_richardson_rotamer("ILE", "tp", 36, 2, 2, 1, 4, -169, -177, 66, 66, 0, 0, 0, 0);
       add_richardson_rotamer("ILE", "tt", 127, 8, 1, 8, 14, -174, -177, 167, 170, 0, 0, 0, 0);
       add_richardson_rotamer("ILE", "mp", 19, 1, 0, 2, 1, -5555, -65, -5555, 100, 0, 0, 0, 0);
       add_richardson_rotamer("ILE", "mt", 993, 60, 81, 58, 41, -66, -65, 169, 170, 0, 0, 0, 0);
       add_richardson_rotamer("ILE", "mm", 242, 15, 10, 16, 17, -57, -57, -59, -59, 0, 0, 0, 0);
       add_richardson_rotamer("LEU", "pp", 21, 1, 0, 2, 1, -5555, 62, -5555, 80, 0, 0, 0, 0);
       add_richardson_rotamer("LEU", "tp", 750, 29, 30, 36, 23, 177, -177, 63, 65, 0, 0, 0, 0);
       add_richardson_rotamer("LEU", "tt", 49, 2, 1, 3, 1, -172, -172, 147, 147, 0, 0, 0, 0);
       add_richardson_rotamer("LEU", "mp", 63, 2, 1, 5, 2, -85, -85, 66, 65, 0, 0, 0, 0);
       add_richardson_rotamer("LEU", "mt", 1548, 59, 62, 46, 66, -65, -65, 174, 174, 0, 0, 0, 0);
       add_richardson_rotamer("HIS", "p-80",   51,  9,  0, 6, 13, 60, 62, -75, -75, 0, 0, 0, 0);
       add_richardson_rotamer("HIS", "p80",    26,  4,  0, 4, 6, 61, 62, 78, 78, 0, 0, 0, 0);
       add_richardson_rotamer("HIS", "t-160",  31,  5,  5, 14, 1, -178, -177, -163, -163, 0, 0, 0, 0);
       add_richardson_rotamer("HIS", "t-80",   64, 11, 17, 9, 9, -173, -177, -81, -81, 0, 0, 0, 0);
       add_richardson_rotamer("HIS", "t60",    94, 16, 24, 17, 12, -178, -177, 62, 62, 0, 0, 0, 0);
       add_richardson_rotamer("HIS", "m-70",  174, 29, 26, 30, 30,  -60, -65, -69, -69, 0, 0, 0, 0);
       add_richardson_rotamer("HIS", "m170",   44,  7,  9,  3,  9,  -63, -65, 165, 165, 0, 0, 0, 0);
       add_richardson_rotamer("HIS", "m80",    78, 13, 14, 10, 14,  -66, -65, 83, 83, 0, 0, 0, 0);
       add_richardson_rotamer("TRP", "p-90",   67, 11,  2, 13, 14,   58, 62, -87, -90, 0, 0, 0, 0);
       add_richardson_rotamer("TRP", "p90",    34,  6,  1,  9,  6,   60,   62, 92, 90, 0, 0, 0, 0);
       add_richardson_rotamer("TRP", "t-105", 100, 16, 27, 10, 14,  178, -177, -105, -105, 0, 0, 0, 0);
       add_richardson_rotamer("TRP", "t90",   109, 18, 28, 14, 15, -178, -177, 88, 90, 0, 0, 0, 0);
       add_richardson_rotamer("TRP", "m-90",   31,  5,  0,  7,  7,  -70,  -65, -87, -90, 0, 0, 0, 0);
       add_richardson_rotamer("TRP", "m0",     48,  8, 15,  2,  8,  -66,  -65, -4, -4, 0, 0, 0, 0);
       add_richardson_rotamer("TRP", "m95",   195, 32, 22, 43, 29,  -69,  -65, 95, 95, 0, 0, 0, 0);
       add_richardson_rotamer("TYR", "p90", 182, 13, 1, 21, 12, 63, 62, 89, 90, 0, 0, 0, 0);
       add_richardson_rotamer("TYR", "t80", 486, 34, 55, 25, 30, 176, -177, 77, 80, 0, 0, 0, 0);
       add_richardson_rotamer("TYR", "m-85", 618, 43, 26, 50, 45, -65, -65, -87, -85, 0, 0, 0, 0);
       add_richardson_rotamer("TYR", "m -30 ", 124, 9, 15, 4, 9, -64, -65, -42, -30, 0, 0, 0, 0);
       add_richardson_rotamer("PHE", "p90", 202, 13, 1, 24, 11, 59, 62, 88, 90, 0, 0, 0, 0);
       add_richardson_rotamer("PHE", "t80", 522, 33, 57, 18, 29, 177, -177, 80, 80, 0, 0, 0, 0);
       add_richardson_rotamer("PHE", "m-85", 697, 44, 29, 51, 47, -64, -65, -83, -85, 0, 0, 0, 0);
       add_richardson_rotamer("PHE", "m -30 ", 149, 9, 12, 5, 11, -64, -65, -19, -30, 0, 0, 0, 0);
       add_richardson_rotamer("PRO", "Cg endo",    379, 44, 23, 54, 43,  30,  30, -37.5, -37.5, 0, 0, 0, 0);
       add_richardson_rotamer("PRO", "Cg exo",     372, 43, 68, 28, 44, -29, -30, 37.5, 37.5, 0, 0, 0, 0);
       add_richardson_rotamer("PRO", "cis Cg endo", 56,  6,  0,  1,  7,  31,  30, -37.5, -37.5, 0, 0, 0, 0);
       add_richardson_rotamer("THR", "p", 1200, 49, 25, 31, 65, 59, 62, 0, 0, 0, 0, 0, 0);
       add_richardson_rotamer("THR", "t", 169, 7, 0, 13, 6, -171, -175, 0, 0, 0, 0, 0, 0);
       add_richardson_rotamer("THR", "m", 1062, 43, 74, 55, 29, -61, -65, 0, 0, 0, 0, 0, 0);
       add_richardson_rotamer("VAL", "p", 169, 6, 2, 8, 8, 63, 63, 0, 0, 0, 0, 0, 0);
       add_richardson_rotamer("VAL", "t", 1931, 73, 90, 72, 63, 175, 175, 0, 0, 0, 0, 0, 0);
       add_richardson_rotamer("VAL", "m", 526, 20, 7, 20, 28, -64, -60, 0, 0, 0, 0, 0, 0);
       add_richardson_rotamer("SER", "p", 1201, 48, 33, 36, 55, 64, 62, 0, 0, 0, 0, 0, 0);
       add_richardson_rotamer("SER", "t", 541, 22, 22, 34, 18, 178, -177, 0, 0, 0, 0, 0, 0);
       add_richardson_rotamer("SER", "m", 714, 29, 44, 29, 25, -65, -65, 0, 0, 0, 0, 0, 0);
       add_richardson_rotamer("CYS", "p", 64, 23, 5, 23, 34, 55, 62, 0, 0, 0, 0, 0, 0);
       add_richardson_rotamer("CYS", "t", 74, 26, 20, 45, 21, -177, -177, 0, 0, 0, 0, 0, 0);
       add_richardson_rotamer("CYS", "m", 142, 50, 75, 32, 43, -65, -65, 0, 0, 0, 0, 0, 0);
}

