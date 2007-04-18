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

typedef datatype Rule_pattern *Rule_pat_t;
typedef datatype Rule_pattern @rule_pat_t;
datatype Rule_pattern {
  SymbPat(const char ?); // abstract grammar symbol: name.
  CharPat(unsigned int); // single character from a character range.
  LitPat(const char ?);
  SeqPat(rule_pat_t,rule_pat_t);
  WildcardPat(const char ?); // symbol for which this is a wildcard.
};


extern rule_pat_t pats2seq(List::list_t<rule_pat_t>);

extern const char ?pat2string(rule_pat_t);
extern rule_t pat2parse_rule(rule_pat_t p);
extern rule_t pat2print_rule(rule_pat_t p);
extern const char ?pat2print_code(rule_pat_t p);
#endif
