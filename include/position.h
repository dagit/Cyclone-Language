/* This file is part of the Cyclone Library.
   Copyright (C) 2000-2001 Greg Morrisett

   This library is free software; you can redistribute it and/or it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place, Suite
   330, Boston, MA 02111-1307 USA. */

#ifndef _POSITION_H_
#define _POSITION_H_
// operations for tracking positions (locations) within source code
#include <core.h>
#include <list.h>

namespace Position {
  using Core;

  extern void   reset_position(string_t<`H>);
  extern void   set_position_file(string_t<`H>);
  extern string_t get_position_file();

  //////// Location Tracking /////////
  typedef unsigned int seg_t; 
  extern seg_t  segment_join(seg_t,seg_t);
  extern bool   segment_equals(seg_t,seg_t);
  extern mstring_t string_of_loc(unsigned int);
  extern mstring_t string_of_segment(seg_t);
  extern List::list_t<mstringptr_t> strings_of_segments(List::list_t<seg_t,`r>);

  //////// Errors ////////////////////
  struct Error;
  typedef struct Error @error_t;
  extern error_t mk_err(seg_t,string_t<`H>);

  //////// Error Reporting ///////////
  extern bool use_gcc_style_location;
  extern int num_errors;
  extern int max_errors;
  extern void post_error(error_t);
  extern bool error_p();

  // Line number translation //
  // Returns a string which can be inserted into the output c file
  // to indicate where the code came from in the original cyclone file
  extern string_t get_line_directive(seg_t);
}


#endif 
