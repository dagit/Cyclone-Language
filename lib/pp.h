/* This file is part of the Cyclone Library.
   Copyright (C) 2001 AT&T

   This library is free software; you can redistribute it and/or it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place, Suite
   330, Boston, MA 02111-1307 USA. */

#ifndef _PP_H_
#define _PP_H_

#include <core.h>
#include <cstdio.h>
#include <list.h>

namespace PP {
using Core;
using std;
using List;
extern struct Ppstate;
typedef struct Ppstate @ppstate_t;
extern struct Out;
typedef struct Out @out_t;
extern struct Doc;
typedef struct Doc @doc_t;

extern void file_of_doc(doc_t d, int w, FILE @f);
extern string_t string_of_doc(doc_t d, int w);
extern $(string_t,list_t<$(int,int,int,string_t)@>)@string_and_links(doc_t d, int w);

extern doc_t nil_doc();
extern doc_t blank_doc();
extern doc_t line_doc();
extern doc_t oline_doc();
extern doc_t text(string_t<`H> s);
extern doc_t textptr(stringptr_t<`H> s);
extern doc_t hyperlink(string_t<`H> shrt, string_t<`H> full);
extern doc_t nest(int k, doc_t d);
extern doc_t cat(...`r doc_t);
extern doc_t cats(list_t<doc_t,`H> doclist);
extern doc_t cats_arr(doc_t ?`H docs);
extern doc_t doc_union(doc_t d, doc_t d2);
extern doc_t tab(doc_t d);
extern doc_t ppseq(doc_t pp(`a), string_t<`H> sep, list_t<`a,`H> l0);
extern doc_t seq(string_t<`H> sep, list_t<doc_t,`H> l0);
extern doc_t ppseql(doc_t pp(`a), string_t<`H> sep, list_t<`a,`H> l0);
extern doc_t seql(string_t<`H> sep, list_t<doc_t,`H> l0);
extern doc_t group(string_t<`H> start, string_t<`H> stop, string_t<`H> sep, list_t<doc_t,`H> ss);
extern doc_t groupl(string_t<`H> start, string_t<`H> stop, string_t<`H> sep, list_t<doc_t,`H> ss);
extern doc_t egroup(string_t<`H> start, string_t<`H> stop, string_t<`H> sep, list_t<doc_t,`H> ss);
}
#endif /* _PP_H_ */
