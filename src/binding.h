/* Name resolution
   Copyright (C) 2003 Dan Grossman, AT&T
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
#ifndef _BINDING_H
#define _BINDING_H

#include <dict.h>
#include "absyn.h"

namespace Binding {
  using List;
  using Dict;

  extern bool warn_override;
  void resolve_all(list_t<Absyn::decl_t,`H> tds);

  // only the lexer needs the rest of this
  typedef list_t<Absyn::var_t> namespace_name_t;
  extern datatype NSDirective {
    Namespace(namespace_name_t); // absolute
    Using(namespace_name_t); // absolute
  };
  typedef datatype NSDirective @ nsdirective_t;
  extern struct NSCtxt<`a> {
    namespace_name_t            curr_ns;    // current namespace (absolute)
    list_t<nsdirective_t>       availables; // current using/namespace stack
    dict_t<namespace_name_t,`a> ns_data;    // maps absolute names to info
  };
  typedef struct NSCtxt<`a> @ nsctxt_t<`a>;

  nsctxt_t<`a> mt_nsctxt(`b, `a mkdata(`b));
  void enter_ns(nsctxt_t<`a>, Absyn::var_t, `b, `a mkdata(`b));
  namespace_name_t enter_using(Absyn::seg_t loc, nsctxt_t ctxt, Absyn::qvar_t usename);
  void leave_ns(nsctxt_t ctxt);
  void leave_using(nsctxt_t ctxt);
  namespace_name_t resolve_rel_ns(Absyn::seg_t, nsctxt_t, namespace_name_t);
}
#endif
