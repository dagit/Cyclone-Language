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

#ifndef EARLEY_H
#define EARLEY_H
#include "bnf.h"
#include "fa.h"
#include "semiring.h"

#define MIN_ACTION 262

namespace Earley {
  
//	// Given a symbol name A, return corresponding "epsilon" version.
//  extern string_t eps_symb(string_t symb);

//////////////////////////////////////////////
//// SHARED functions (frontend and backend)
/////////////////////////////////////////////

  typedef struct symbInfo @`H symb_info_t;
  struct symbInfo {
    unsigned int symb_action_counter;
    struct Hashtable::Table<str_t,act_t> @symb_action_table;
    struct Hashtable::Table<act_t,str_t> @action_symb_table;
  };

  extern symb_info_t new_symb_info();
  extern act_t symb2act(string_t<`H> symb, symb_info_t si);
  extern string_t act2symb(act_t act, Hashtable::table_t<act_t,str_t> as_table);


  extern act_t callout_action();
  extern act_t repeat_action();
  extern act_t final_st_action();
  extern act_t start_states_table_action();
  extern act_t repeat_decr_action();
  extern act_t repeat_final_action();
  
  extern Hashtable::table_t<act_t,str_t> get_as_table(symb_info_t si);  		
  extern Hashtable::table_t<str_t,act_t> get_sa_table(symb_info_t si);  		
  extern $(dfa_t,Set::set_t<st_t>,symb_info_t) *fsm2dfa(const char ?filename) ;

//  extern $(dfa_t,Set::set_t<st_t>,symb_info_t) compile(grammar_t grm, rule_t r);
  // returns: int pair: a) the number of valid complete parses of the string.
  //                    b) the number of valid prefix parses of the string.
//  extern $(int,int) recognize(dfa_t dfa, symb_info_t symb_info, Set::set_t<st_t> dfa_final, 
//		     const char ?input, int show_progress);

//  extern parse_forest_t recognize_frag(dfa_t dfa, symb_info_t symb_info, const char?`H symb_name, 
//				       const char ?input, int show_progress);

  //////////////////////////////////////////////
  //// FRONTEND only
  /////////////////////////////////////////////

  // is_grm argument indicates whether to treat the dfa as encoding a grammar and, hence, print
  // grammar-related decls.
  extern void dfa2cyclone(FILE @f, dfa_t dfa, symb_info_t symb_info, string_t dfa_namespace, int is_grm);

  extern void act2symb_haxe(FILE @f, symb_info_t si);

  // Convert and print the rule as an NFA in FSM format.
  // Writes two files based on the filename_base: .fsm and .sym. The
  // former holds the NFA, the latter the mapping from symbolic names to
  // numbers.
  extern void rule_fsm(const char ?fsm_file,const char ?sym_file, grammar_t grm, rule_t r);

  extern void grammar_fsm(const char ?fsm_file,const char ?sym_file, grammar_t grm);

  extern void dfa2dot(dfa_t dfa, symb_info_t symb_info, const char ?dot_file);
  
  // Convert a rule to an LR state machine.
  extern $(dfa_t,Set::set_t<st_t>,
           Hashtable::table_t<st_t, Hashtable::table_t<act_t, List::list_t<cs_t>>>,
           symb_info_t) 
    rule2LR(grammar_t grm, rule_t r, rule_t right_context, int minimize, int inline_cs);

  extern const char ?yk_inputs;
}

#endif
