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

extern void nfa_init(grammar_t grm);

extern st_t nfa_fresh_state();

extern st_t get_final(st_t a);
extern st_t get_etrans(st_t a);
extern cs_opt_t get_action(st_t a);
extern st_t get_atrans(st_t a);

extern st_t rule2nfa(grammar_t grm, rule_t r);

extern unsigned int what_interval(unsigned int ?intervals, unsigned int key);

extern st_t mklit(const char ?x);
extern void final(st_t a, st_t b);
extern void etrans(st_t a, st_t b);
extern void action(st_t from, cs_opt_t x, st_t to);
extern st_t mkact(cs_t x);
extern st_t mkstar(st_t a);
extern st_t mkseq(st_t a,st_t b);
extern st_t mkalt(st_t a,st_t b);
extern void warn_repeat();

extern st_t mklit_fsm(FILE@ f_fsm, const char ?x);
extern void final_fsm(FILE@ f_fsm, st_t a, st_t b, const char? symb);
extern void etrans_fsm(FILE@ f_fsm, st_t a, st_t b, Semiring::weight_t w);
extern void action_fsm(FILE@ f_fsm, st_t from, cs_opt_t x, st_t to, 
		       Semiring::weight_t w);

 extern void single_action_fsm(FILE@ f_fsm, st_t from, act_t x, const char ? name,
			       st_t to, Semiring::weight_t w);

extern st_t mkact_fsm(FILE@ f_fsm, cs_t x);
extern st_t mkstar_fsm(FILE@ f_fsm, st_t a);
extern st_t mkseq_fsm(FILE@ f_fsm, st_t a,st_t b);
extern st_t mkopt_fsm(FILE@ f_fsm, st_t a);

}

#endif
