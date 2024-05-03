/*
 * ideal/refinement-lights.hh
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
 * General Public License for more details.
 *
 * You should have received a copies of the GNU General Public License and
 * the GNU Lesser General Public License along with this program; if not,
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA, 02110-1301, USA.
 * See http://www.gnu.org/licenses/
 */


#ifndef REFINEMENT_LIGHTS_HH
#define REFINEMENT_LIGHTS_HH

#include <string>
#include "rama-types.hh"

namespace coot {

   class refinement_lights_info_t {
   public:
      class the_worst_t {
      public:
         the_worst_t(const unsigned int idx_in, const float &val) : restraints_index(idx_in), value(val) {
            is_set = true;
            restraints_index = -1;
         }
         the_worst_t() { is_set = false; value = -99999; restraints_index = -1; }
         int restraints_index; // can be -1
         float value;
         bool is_set;
         // update if value_in is more than this value
         void update_if_worse(const float &value_in, const int &idx_in) {
            if (! is_set) {
               restraints_index = idx_in;
               value = value_in;
               is_set = true;
               // std::cout << "update_if_worse() setting worst to " << value << std::endl;
            } else {
               if (value_in > value) {
                  restraints_index = idx_in;
                  value = value_in;
                  // std::cout << "update_if_worse() updating worst to " << value << std::endl;
               }
            }
         }
         void update_if_worse(const the_worst_t &baddie_in) {
            if (baddie_in.is_set) {
               if (! is_set) {
                  restraints_index = baddie_in.restraints_index;
                  value = baddie_in.value;
                  is_set = true;
               } else {
                  if (baddie_in.value > value) {
                     restraints_index = baddie_in.restraints_index;
                     value = baddie_in.value;
                  }
               }
            }
         }
      };

      std::string name;   // e.g. "Bonds" or "Angles"
      std::string label;  // e.g. "Bonds:  6.543"
      float value;        // e.g. 6.543
      int rama_type;
      the_worst_t worst_baddie;
      refinement_lights_info_t(const std::string &name_in, const std::string &label_in, float value_in) :
         name(name_in), label(label_in) {
         value = value_in;
         rama_type = RAMA_TYPE_LOGRAMA;
      }
   };

}

#endif // REFINEMENT_LIGHTS_HH
