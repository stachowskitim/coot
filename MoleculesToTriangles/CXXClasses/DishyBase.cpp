/*
 * MoleculesToTriangles/CXXClasses/DishyBases.cpp
 *
 * Copyright 2009 by Martin Noble, University of Oxford
 * Author: Martin Noble
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#include "DishyBase.h"

void
DishyBaseContainer_t::init() {
   cytidine_base_names.push_back(" N1 ");
   cytidine_base_names.push_back(" C2 ");
   cytidine_base_names.push_back(" N3 ");
   cytidine_base_names.push_back(" C4 ");
   cytidine_base_names.push_back(" C5 ");
   cytidine_base_names.push_back(" C6 ");
   cytidine_base_names.push_back(" O2 ");
   cytidine_base_names.push_back(" N4 ");

   uracil_base_names.push_back(" N1 ");
   uracil_base_names.push_back(" C2 ");
   uracil_base_names.push_back(" N3 ");
   uracil_base_names.push_back(" C4 ");
   uracil_base_names.push_back(" C5 ");
   uracil_base_names.push_back(" C6 ");
   uracil_base_names.push_back(" O2 ");
   uracil_base_names.push_back(" O4 ");

   adenine_base_names.push_back(" N9 ");
   adenine_base_names.push_back(" C8 ");
   adenine_base_names.push_back(" N7 ");
   adenine_base_names.push_back(" C5 ");
   adenine_base_names.push_back(" C4 ");
   adenine_base_names.push_back(" N1 ");
   adenine_base_names.push_back(" C2 ");
   adenine_base_names.push_back(" N3 ");
   adenine_base_names.push_back(" C6 ");
   adenine_base_names.push_back(" N6 ");

   guanine_base_names.push_back(" N9 ");
   guanine_base_names.push_back(" C8 ");
   guanine_base_names.push_back(" N7 ");
   guanine_base_names.push_back(" C5 ");
   guanine_base_names.push_back(" C4 ");
   guanine_base_names.push_back(" N1 ");
   guanine_base_names.push_back(" C2 ");
   guanine_base_names.push_back(" N3 ");
   guanine_base_names.push_back(" C6 ");
   guanine_base_names.push_back(" O6 ");
   guanine_base_names.push_back(" N2 ");
   
   thymine_base_names.push_back(" N1 ");
   thymine_base_names.push_back(" C2 ");
   thymine_base_names.push_back(" N3 ");
   thymine_base_names.push_back(" C4 ");
   thymine_base_names.push_back(" C5 ");
   thymine_base_names.push_back(" C6 ");
   thymine_base_names.push_back(" O2 ");
   thymine_base_names.push_back(" O4 ");
   thymine_base_names.push_back(" C5M");

}
std::vector<std::pair<int, int> > DishyBase_t::bondingPattern = {
    std::pair<int,int>(0,1),
    std::pair<int,int>(1,2),
    std::pair<int,int>(2,3),
    std::pair<int,int>(3,4),
    std::pair<int,int>(4,0),
};
