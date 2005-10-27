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

#ifndef NAMESPACES_H
#define NAMESPACES_H
#include "bnf.h"
typedef struct Hashtable::Table<const char ?@,const char ?@> @renaming_t;
grammar_t rename_grammar(grammar_t g, renaming_t t);
grammar_t namespace_grammar(const char ?ns, grammar_t g);
grammar_t unnamespace_grammar(grammar_t g);
#endif

