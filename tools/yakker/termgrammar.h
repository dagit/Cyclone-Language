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

#ifndef TERMGRAMMAR_H
#define TERMGRAMMAR_H
#include "bnf.h"
// Flag: used to set whether anonymous binders will be added for every symbol.
extern int include_anon_binder;

// Flag: used to set whether named binders will be added for every symbol.
extern int include_named_binder;

// if esc_chars is NULL, then a default set of chars is escaped.
extern grammar_t escape_grammar(grammar_t g, const char? esc_chars);

// Assumes that flattening has been performed.
extern grammar_t grammar2bnfgen_grammar(grammar_t<`H> g);
extern grammar_t grammar2termgrammar_bnf(grammar_t ds);
extern grammar_t grammar2bindgrammar(grammar_t<`H> g);
#endif
