/* Kinds
   Copyright (C) 2004 Dan Grossman, AT&T
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
#ifndef _KINDS_H_
#define _KINDS_H_

#include "absyn.h"

namespace Kinds {
using Absyn;

// useful kinds
extern struct Kind rk; // shareable region kind
extern struct Kind ak; // shareable abstract kind
extern struct Kind bk; // shareable boxed kind
extern struct Kind mk; // shareable mem kind
extern struct Kind ek; // effect kind
extern struct Kind ik; // int kind
extern struct Kind boolk; // boolean kind
extern struct Kind ptrbk; // pointer bound kind

extern struct Kind trk; // top region kind
extern struct Kind tak; // top abstract kind
extern struct Kind tbk; // top boxed kind
extern struct Kind tmk; // top memory kind

extern struct Kind urk;  // unique region kind
extern struct Kind uak;  // unique abstract kind
extern struct Kind ubk;  // unique boxed kind
extern struct Kind umk;  // unique memory kind

extern struct Kind aqk;  // alias qualifier kind

extern struct Core::Opt<kind_t> rko;
extern struct Core::Opt<kind_t> ako;
extern struct Core::Opt<kind_t> bko;
extern struct Core::Opt<kind_t> mko;
extern struct Core::Opt<kind_t> iko;
extern struct Core::Opt<kind_t> eko;
extern struct Core::Opt<kind_t> boolko;
extern struct Core::Opt<kind_t> ptrbko;

extern struct Core::Opt<kind_t> trko;
extern struct Core::Opt<kind_t> tako;
extern struct Core::Opt<kind_t> tbko;
extern struct Core::Opt<kind_t> tmko;

extern struct Core::Opt<kind_t> urko;
extern struct Core::Opt<kind_t> uako;
extern struct Core::Opt<kind_t> ubko;
extern struct Core::Opt<kind_t> umko;

extern struct Core::Opt<kind_t> aqko;

Core::opt_t<kind_t> kind_to_opt(kind_t);
kindbound_t kind_to_bound(kind_t);
kindbound_t copy_kindbound(kindbound_t);

kind_t id_to_kind(string_t<`H>, seg_t); // used in parse.y
string_t kind2string(kind_t);

kind_t tvar_kind(tvar_t,kind_t def);

$(tvar_t,kindbound_t) swap_kind(type_t, kindbound_t);
  // for temporary kind refinement

kindbound_t compress_kb(kindbound_t);
kind_t force_kb(kindbound_t);

// returns true if first-arg is a sub-kind of second-arg
bool kind_leq(kind_t,kind_t);
bool kind_eq(kind_t,kind_t);

// only used by tctyp
Core::opt_t<kindbound_t> kind_to_bound_opt(kind_t);
bool constrain_kinds(kindbound_t, kindbound_t);
}
#endif 
