#ifndef DFA_H_
#define DFA_H_
#include <set.h>
#include <hashtable.h>
#include "fa.h"
#include "axarray.h"

namespace DFA {
//	namespace Kit {
//		// What's needed for building new Earley parsing DFAs based on a 
//		// pre-exisiting grammar.
//		struct dfa_kit {
//			dfa_t dfa;
//
//			unsigned int symb_action_counter;
//			struct Hashtable::Table<str_t,act_t> @symb_action_table;
//			struct Hashtable::Table<act_t,str_t> @action_symb_table;
//
//			Axarray::xarray_t<st_t> symbol_start_states;
//			Axarray::xarray_t<Set::set_t<st_t>> symbol_final_states;
//		};
//
//		typedef struct dfa_kit@ dfa_kit_t;
//
//		//////////
//		// Accessor functions for DFA kit.
//		/////////
//
//		// Get the set of final states for all symbols in the dfa.
//		extern Set::set_t<st_t> get_all_symbol_finals(dfa_kit_t k);
//
//		// Get the start state for the specified symbol-action.
//		extern st_t get_symbol_start(dfa_kit_t k, act_t symb_act);
//		// Get the final states for the specified symbol-action.
//		extern Set::set_t<st_t> get_symbol_finals(dfa_kit_t k, act_t symb_act);
//
//		extern st_t new_state(dfa_kit_t k);
//
//		// Create a new symbol-action for the symbol argument.
//		extern act_t new_symact(dfa_kit_t k, const char ?symb);
//		// Get the symbol-action for the specified symbol.
//		extern act_t lookup_symact(dfa_kit_t k, const char ?symb);
//		// Map a symbol-action to a symbol.
//		extern const char ?lookup_symbol(dfa_kit_t k, act_t act);
//
//		////////
//		// Convenience functions for extending the dfa in various ways.
//		////////
//		extern st_t mk_call(dfa_kit_t dk, const char ?symb, st_t final);
//		extern st_t mk_lit(dfa_kit_t dk, const char ?lit, st_t final);
//		extern st_t mk_char(dfa_kit_t dk, char c, st_t final);
//	}

	namespace Rep {
		// What's needed for Earley parsing.
		struct dfa_rep {
			dfa_t d;
			st_t start;
			// final states of entire dfa (as opposed to final states of all symbols).
			Set::set_t<st_t> finals;
			Hashtable::table_t<act_t,str_t> action_symb_table;
		};

		typedef struct dfa_rep@ dfa_rep_t;

		//////////
		// Accessor functions for DFA representation.
		/////////

//		// Get the set of final states for all symbols in the dfa.
//		extern Set::set_t<st_t> get_finals(dfa_rep_t dr);
//
//		// Get the start state for the specified symbol-action.
//		extern st_t get_start(dfa_rep_t dr);
//		extern dfa_t get_dfa(dfa_rep_t dr);
//		extern Hashtable::table_t<act_t,str_t> get_astable(dfa_rep_t dr);
	}

}

#endif /*DFA_H_*/
