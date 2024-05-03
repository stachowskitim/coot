/*
 * src/mmcif-sfs-to-mtz-main.cc
 *
 * Copyright 2012 by University of York
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

#include <iostream>

int mmcif_sfs_to_mtz(const char *cif_file_name, const char *mtz_out_file_name);

int main(int argc, char **argv) {

   int status = 0;

   if (argc != 3) {
      std::cout << "Usage: " << argv[0] << "<input-cif-file-name> <output-mtz-file-name>"<< std::endl;
   } else {

      int success = mmcif_sfs_to_mtz(argv[1], argv[2]);
      if (success != 1)
	 status = 1; // fail.
   } 
   return status;
} 
