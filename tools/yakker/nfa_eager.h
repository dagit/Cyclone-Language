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

#ifndef NFA_EAGER_H
#define NFA_EAGER_H
#include "bnf.h"
#include "fa.h"

/* The Nfa_eager and Nfa_lazy namespaces should have the same declarations */
namespace Nfa_eager {

void nfa_init(grammar_t grm);

st_t nfa_fresh_state();

st_t get_final(st_t a);
st_t get_etrans(st_t a);
cs_opt_t get_action(st_t a);
st_t get_atrans(st_t a);

st_t rule2nfa(grammar_t grm, rule_t r);

unsigned int what_interval(unsigned int ?intervals, unsigned int key);

}

#endif
