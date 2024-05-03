/*
 * skeleton/graphical_skel.h
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

// -*-c++-*-
#include "clipper/contrib/skeleton.h"
#include "coords/mmdb-crystal.h"
#include "coords/Bond_lines.h"


class GraphicalSkel {

  class cv_list_t { 
  public: 
    int atom_index;
    coot::Cartesian geometrical_vector; 
  }; 
   
 public:
   GraphicalSkel(const clipper::Xmap<float> &in, clipper::Xmap<int> &out) 
      {
	 
	 std::cout << "GraphicalSkel input map: " 
	      << in.cell().descr().a() << " " 
	      << in.cell().descr().b() << " " 
	      << in.cell().descr().c() << " " 
	      << in.cell().descr().alpha() << " " 
	      << in.cell().descr().beta() << " " 
	      << in.cell().descr().gamma() << " " 
	      << std::endl;
	 
	 clipper::Xmap<int>::Map_reference_index ix;
	 double s, sr, sr2, mean, sigm;
	 s = sr = sr2 = 0.0;
	 for ( ix = out.first(); !ix.last(); ix.next() ) {
	    s += 1.0;
	    sr += in[ix];
	    sr2 += pow(in[ix],2);
	 }
	 mean = sr / s;
	 sigm = sqrt( s * sr2 - sr * sr ) / s;
	 for ( ix = out.first(); !ix.last(); ix.next() ) {
	    if ( in[ix] > mean+1.0*sigm )
	       out[ix] = 1;
	    else
	       out[ix] = 0;
	 }
	 clipper::Skeleton_basic skel(1);
	 skel(out,in);
      };

   GraphicalSkel() {}; 

   graphical_bonds_container 
      make_graphical_bonds( const clipper::Xmap<float> &in, 
			    const clipper::Xmap<int> &l1 ) const; 

    graphical_bonds_container 
       make_graphical_bonds(const clipper::Xmap<float> &map, 
 			   const clipper::Xmap<int>   &l1, 
 			   coot::Cartesian centre_point, 
 			   float box_radius, 
 			   float cut_off) const;  

/*    void */
/*      make_graphical_bonds(const clipper::Xmap<float> &map, */
/* 			  const clipper::Xmap<int>   &l1, */
/* 			  coot::Cartesian centre_point, */
/* 			  float box_radius, */
/* 			  float cut_off) const;  */


   graphical_bonds_container
      old_make_graphical_bonds(const clipper::Xmap<float> &map,
			   const clipper::Xmap<int>   &l1,
			   coot::Cartesian centre_point,
			   float box_radius) const; 

   void tip_filter( const clipper::Xmap<float> &map,
		    clipper::Xmap<int> *l1); 
   
   void prune( const clipper::Xmap<float> &map,
	       clipper::Xmap<int> *l1);

   int pruning_loop(clipper::Xmap<int> *l1) const;
   
   void label_end_points(clipper::Xmap<int> *li); 


   int Pprune(const clipper::Xmap<float> &map,
	       clipper::Xmap<int> *l1, float cut_off); 

   int Ptip_convert( const clipper::Xmap<float> &map,
		     clipper::Xmap<int> *l1, int level, float cut_off ); 

   int Ptip_convert_old( const clipper::Xmap<float> &map,
		     clipper::Xmap<int> *l1, int level, float cut_off ); 

   int N_tips(const clipper::Xmap<float> &map, 
	      const clipper::Xmap<int> &l1,
	      float cut_off) const; // the number of tips in the map

   atom_selection_container_t convert_to_atom(const clipper::Xmap<int> &l1, 
					      std::vector<coot::Cartesian> c); 
      
};


// This should ideally have a proper home (on the range)
// 
coot::Cartesian
average_Cartesians(std::vector<coot::Cartesian> c); 
