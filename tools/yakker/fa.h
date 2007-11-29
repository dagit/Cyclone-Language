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

#ifndef FA_H
#define FA_H
#include <graph.h>
#include "semiring.h"

typedef unsigned int st_t; // States.  Sometimes, 0 is reserved to mark no transition
typedef Graph::graph_t<st_t> stgraph_t; // State graphs.
typedef unsigned int act_t;

#define NOTRANSITION 0
#define EPSILON NULL // the empty action

extern int uint_cmp(unsigned int a, unsigned int b);
extern int st_hash(st_t a);
extern int st_cmp(st_t a,st_t b);

extern st_t nfa_number_of_states;

extern int use_lazy_nfa;

extern unsigned int num_conflicts_ll;
extern unsigned int num_conflicts_dfa;
extern unsigned int num_conflicts_change_order;
extern unsigned int num_conflicts_resolved;
extern unsigned int num_conflicts_unresolved;
extern unsigned int num_conflicts_unbounded;
typedef unsigned int att_t;  // Attributes
typedef Set::set_t<att_t> aset_t;
extern struct DFA;
typedef struct DFA @dfa_t;

// Does state s have a transition on a in DFA dfa?
// Returns: target state, if yes; 0, if no.
st_t target(dfa_t dfa,st_t s,act_t a);

// Returns: target state and transition weight, if yes; $(0,zero_weight) if no.
$(st_t,Semiring::weight_t) target_w_weight(dfa_t dfa,st_t s,act_t a);

extern act_t* first_action_after(dfa_t dfa,st_t s,act_t a);
extern Set::set_t<st_t> dfa_final_states(dfa_t dfa);
extern void dfa_set_final(dfa_t dfa, st_t final, aset_t attrs);
extern dfa_t nfa2dfa(st_t start_state, Set::set_t<st_t,`H> final_states);
extern int dfa_is_final(dfa_t dfa,st_t s);
extern aset_t dfa_final_attrs(dfa_t dfa,st_t s);
// dfa_final_weight will throw exception if s is not a final state.
extern Semiring::weight_t dfa_final_weight(dfa_t dfa,st_t s);
extern dfa_t lookahead_dfa(grammar_t grm,List::list_t<rule_t> rules, rule_t right_ctxt);
extern void dfa_generate1(dfa_t dfa);
extern void dfa_generate2(dfa_t dfa);
extern void lookahead_glush(grammar_t grm,List::list_t<rule_t> rules, rule_t right_ctxt);

extern void nfa_dot(FILE @f,Set::set_t<st_t> final_states, cs_pred_t special);
extern void dfa_dot(FILE @f, dfa_t dfa, cs_pred_t special);

// print symbols in FSM symbol-file format.
void symbols_fsm(FILE @f, Hashtable::table_t<act_t,str_t> action_symb_table);

// print nfa in FSM text format. Action-symbol table contains specialized symbol names
// for actions. Any actions not in table will print out using fsm_char_escape.
void nfa_fsm(FILE @f,Set::set_t<st_t> final_states, 
	     Hashtable::table_t<act_t,str_t> action_symb_table,
	     Hashtable::table_t<st_t,act_t> act_table);
// read a dfa from the FSM format.
// returns NULL on failure.
$(dfa_t,Set::set_t<st_t>) *fsm_dfa(const char ?filename, const char ?`H start_symb,
				   act_t (@symb2act)(const char ?`H, `a),`a symb_info);

/* Print the dfa as a cyclone datastructure. */
extern void dfa_cyclone(FILE@,dfa_t);
/* Print the dfa as a haxe datastructure. */
extern void dfa_haxe(FILE@,dfa_t);

extern dfa_t dfa_minimize(dfa_t dfa);
// Construct a repeat dfa: a series of n states each transitioning to
// the given target state "target_s" on the "target" action. Each
// state transitions to the next on action "next".
// The last state is connected to the specified final state "f".
extern st_t construct_repeat_dfa(dfa_t dfa, act_t next, 
				 act_t target_act, st_t target_s, 
				 act_t final_act, st_t f);
#endif
