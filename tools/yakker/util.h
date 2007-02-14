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

#ifndef UTIL_H
#define UTIL_H
#include <graph.h>
#include <core.h>

typedef const char ?@           str_t;
typedef Graph::graph_t<str_t>   strgraph_t;
typedef Set::set_t<str_t>       strset_t;
typedef struct Set::Set<str_t> *strset_opt_t;

extern void fprint_strset(FILE @f,strset_t ns);
extern void fprint_uintset(FILE @f,Set::set_t<unsigned int,`H> ns);
extern void printGraph(Graph::graph_t<str_t> g);
extern void printUintGraph(Graph::graph_t<unsigned int> g);
extern void gen_uint(unsigned int u);

extern int debug;
extern int print_errors;
extern int expand_nonrec;

extern void eprintf(string_t fmt, ... inject parg_t ap)
  __attribute__((format(printf,1,2)));
extern void warn(string_t fmt, ... inject parg_t ap)
  __attribute__((format(printf,1,2)));
extern void internal_err(string_t fmt, ... inject parg_t ap)
  __attribute__((format(printf,1,2)));
void debugprint(int level, string_t fmt, ... inject parg_t ap)
  __attribute__((format(printf,2,3)));

Graph::graph_t<`a>
node_intersect(Graph::graph_t<`a> g, Set::set_t<`a,`H> x);

Graph::graph_t<`a>
node_delete(Graph::graph_t<`a> g, Set::set_t<`a,`H> x);

const char ?char_escape(char c);
const char ?string_escape(const char ?`H s);

#endif
