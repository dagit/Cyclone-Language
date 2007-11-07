/* Copyright (C) 2005 Greg Morrisett, AT&T.
   This file is part of the Cyclone project.

   Cyclone is free software; you can redistribute it
   and/or modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   Cyclone is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Cyclone; see the file COPYING. If not,
   write to the Free Software Foundation, Inc., 59 Temple Place -
   Suite 330, Boston, MA 02111-1307, USA. */

#ifndef SEMIRING_H
#define SEMIRING_H
namespace Semiring{

  typedef double weight_t;
  
  extern const weight_t zero_weight;
  extern const weight_t one_weight;
  /** returns: 1 on success, 0 on failure */
  extern int scan_weight(const char ?weight_s, weight_t @w_p);
  extern const char ?print_weight(weight_t);
  extern int check_nonzero(weight_t w);
  extern weight_t from_prob(double p);
  extern weight_t add(weight_t w1, weight_t w2);
  extern weight_t mult(weight_t w1, weight_t w2);
}
#endif
