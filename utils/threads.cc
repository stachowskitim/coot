/*
 * utils/threads.cc
 *
 * Copyright 2017 Medical Research Council
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
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */


#include <stdexcept>
#include <iostream>
#include <algorithm>
#include "coot-utils.hh"

#ifdef  _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// can return -1 if name is invalid
//
long coot::get_number_of_threads_by_system_call()  {

#ifdef _WIN32
SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  return sysinfo.dwNumberOfProcessors;
#else
  return sysconf(_SC_NPROCESSORS_CONF);
#endif

}

// unsigned int coot::coot_n_threads = 0; // static

unsigned int coot::get_max_number_of_threads() {

   unsigned int n_threads_default = 4;

   // only do this once:
   // (if coot_n_threads is unset, then set it)
   //
   // is this initially set to 0 by the compiler?
   //
   if (coot_n_threads == 0) {
      // coot_n_threads_cs is null then
      const char *e = getenv("COOT_N_THREADS");
      if (e) {
	 try {
	    // can throw an runtime_error exception on unable to convert
	    unsigned int t = util::string_to_int(e);
	    if (t >= 1024) t = n_threads_default;
	    coot_n_threads = t;
	 }
	 catch (const std::runtime_error &rte) {
	    coot_n_threads = 1;
	 }
      } else {
	 // no environment variable, use system call.
	 coot_n_threads = n_threads_default;
	 long n = get_number_of_threads_by_system_call();
	 if (n > 0)
	    coot_n_threads = n;
      }
   }
   return coot_n_threads;
}

void
coot::set_max_number_of_threads(unsigned int n) {

   coot_n_threads = n;
}
