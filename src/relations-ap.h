/* Tracking relations among variables.
   Copyright (C) 2003 Greg Morrisett
   This file is part of the Cyclone compiler.

   The Cyclone compiler is free software; you can redistribute it
   and/or modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   The Cyclone compiler is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the Cyclone compiler; see the file COPYING. If not,
   write to the Free Software Foundation, Inc., 59 Temple Place -
   Suite 330, Boston, MA 02111-1307, USA. */
#ifndef RELATIONS_H
#define RELATIONS_H
#include "absyn.h"
#include <cycboot.h> // for FILE

namespace Relations {
//////////////////////////////////////////////////////////////////////
// primitive relations that we track for non-escaping, integral variables
// to avoid array bounds checks

// these are the operands for the primitive relations
@tagged union RelnOp {
  unsigned int     RConst;    // a constant
  Absyn::vardecl_t RVar;      // a term-level variable
  Absyn::vardecl_t RNumelts;  // numelts(x) 
  Absyn::type_t    RType;     // `i::I 
  unsigned int     RParam;    // ith parameter of a function
  unsigned int     RParamNumelts; // numelts of ith parameter of a function
  unsigned int     RReturn;   // return value of function (int unused)
};
typedef union RelnOp reln_op_t;
// constructors for the operands
extern reln_op_t 
  RConst(unsigned int),
  RVar(Absyn::vardecl_t),
  RNumelts(Absyn::vardecl_t),
  RType(Absyn::type_t),
  RParam(unsigned int),
  RParamNumelts(unsigned int),
  RReturn();
// the different relations -- note that Rlte and Rlt are *unsigned* comparisons
enum Relation { Req, Rneq, Rlte, Rlt };
typedef enum Relation relation_t;
struct Reln {
  reln_op_t        rop1;
  relation_t       relation;
  reln_op_t        rop2;
};
  // Now defined in absyn.h
  //typedef struct Reln @`r reln_t<`r>;
  //typedef List::list_t<reln_t<`r>,`r> relns_t<`r>;

// map relational primop to one of our limited forms of relations
// (gets rid of > and >= in favor of < and <= to keep things simple.)
// assumes primop is one of ==,!=,<,<=,>,>=
extern $(Absyn::exp_t,relation_t,Absyn::exp_t) 
  primop2relation(Absyn::exp_t e1,Absyn::primop_t p,Absyn::exp_t e2);

// used to negate a relation (note -- must flip operands too)
extern relation_t flip_relation(relation_t r);

extern reln_t<`r> negate(region_t<`r>, reln_t);
// Try to convert an expression to a relation operand -- puts the result in p
// and returns true if successful, returns false otherwise.  
// Right now, this succeeds if e has a tag type, evaluates at compile-time
// to a constant, is a non-escaping variable, is the numelts of a non-escaping
// variable, or is a valueof_e(`i).
extern bool exp2relnop(Absyn::exp_t e, reln_op_t @p);

// tries to convert an expression e to a conjunction of relations
// Right now, e must be of the form:
// P ::= <constant-expression> | P && P | (int)P | (unsigned)P | Rop R Rop |
//       !(Rop R Rop)
// R ::= == | != | <= | < | >= | >
// Rop -- see exp2relnop above.
extern relns_t<`r> exp2relns(region_t<`r> r, Absyn::exp_t e);

// Add rop1 r rop2 to relns (avoids adding if already present)
extern relns_t<`r> add_relation(region_t<`r> rgn, 
                                reln_op_t rop1, relation_t r,
                                reln_op_t rop2, relns_t<`r> relns);

// Duplicate all relations in [relns] that mention an rop for [e_old],
// replacing its rop in the duplicated version with the rop for
// [e_new].  Used when deep-copying an expression after the flow
// analysis (i.e. during code generation)
extern relns_t<`r> duplicate_relation(region_t<`r> rgn,
				      Absyn::exp_t e_old, Absyn::exp_t e_new,
				      relns_t<`r> relns);

// Update relations with x being overwritten by e
extern relns_t<`r> reln_assign_var(region_t<`r>, relns_t<`r>, 
                                   Absyn::vardecl_t, Absyn::exp_t);
// Update relations with e1 being overwritten by e2
extern relns_t<`r> reln_assign_exp(region_t<`r>, relns_t<`r>, 
                                   Absyn::exp_t, Absyn::exp_t);
// Update relations with x being overwritten by some unknown value
extern relns_t<`r> reln_kill_var(region_t<`r>,relns_t<`r>, Absyn::vardecl_t);
// Update relations with e being overwritten by some unknown value
extern relns_t<`r> reln_kill_exp(region_t<`r>,relns_t<`r>, Absyn::exp_t);
// Copy relations from region `r to region `r2
extern relns_t<`r2> copy_relns(region_t<`r2>, relns_t<`r>);
// Returns true iff the relations are syntactically the same
extern bool same_relns(relns_t, relns_t);
// Return true if everything true in r1s appears in r2s
extern bool relns_approx(relns_t<`r> r2s, relns_t<`r> r1s);
// Return the set of relations that occur in both lists of relations
extern relns_t<`r> join_relns(region_t<`r> r, relns_t<`r>, relns_t<`r>);
// Prints relations
extern void print_relns(FILE@,relns_t);
// Converts relation stuff to strings
extern string_t reln2string(reln_t r);
extern string_t rop2string(reln_op_t r);
extern string_t relation2string(relation_t r);
extern string_t relns2string(relns_t r);
// returns true if the relations are consistent -- typical use
// is to check a relation you want to be true (e.g., i < numelts(x))
// under the current set of relations R.  To do so, you add the
// negation numelts(x) <= i and check to see if the resulting system
// is inconsistent.  If not, then you know that the desired relation holds.
extern bool consistent_relations(relns_t rlns);
}
#endif
