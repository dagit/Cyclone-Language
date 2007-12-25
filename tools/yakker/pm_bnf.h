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

#ifndef PM_BNF_H
#define PM_BNF_H
#include <core.h>
#include <list.h>
#include "bnf.h"
#include "ykbuf.h"
#include "funtable.h"
#include "fn.h"
#include "earley-parsetree.h"

typedef datatype Rule_pattern *Rule_pat_t;
typedef datatype Rule_pattern @rule_pat_t;
datatype Rule_pattern {
  SymbPat(const char ?); // abstract grammar symbol: name. !! apparently deprecated !!
  CharPat(unsigned int); // single character from a character range.
  LitPat(const char ?);
  SeqPat(rule_pat_t,rule_pat_t);
/*   RepeatPat(int,rule_pat_t); */
/*   DepSeqPat(rule_pat_t,fn_t<const char ?,rule_pat_t,`H>); // Dependent sequence pattern. */
  BinderPat(const char ?); // symbol for which this is a binder.
};

extern datatype exn{
  extern Pat_exception;
};

extern rule_pat_t pats2seq(List::list_t<rule_pat_t>);

extern const char ?pat2string(rule_pat_t);
extern rule_t pat2parse_rule(rule_pat_t p);
extern rule_t pat2print_rule(rule_pat_t p);
extern const char ?pat2print_code(rule_pat_t p, char ?? args);


// Treat a pattern as an AST and translate back into source
// string. Raises exception Pat_exception if cannot be translated
// (e.g. contains binders).
extern const char ?ast2string(rule_pat_t p);

extern rule_pat_t unescape_pat(rule_pat_t p, char escape_char);

// FIX: is the region of "const char ?" `H, or an implicit variable?
typedef const char?`H @`r1 ?`r2 strptr_array<`r1,`r2>;

extern void
parse_with_pat(ykbuf_t @`r ykb, Funtable::table_t parse_funs, rule_pat_t p, strptr_array<`r1,`r2> args);

// Returns the number of arguments scanned.
extern int
crawl_with_pat(rule_pat_t p, EarleyParsetree::parse_tree_t pt, string_t ykinput,
    strptr_array<`r1,`r2> args);
#endif
