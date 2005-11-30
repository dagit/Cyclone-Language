/* Parser interface.
   Copyright (C) 2001 Greg Morrisett, AT&T
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
#ifndef _PARSE_H_
#define _PARSE_H_
#include "absyn.h"
#include "bansheeif.h"
using Core {
namespace Parse {
  List::list_t<Absyn::decl_t> parse_file(FILE @`H);
  List::list_t<$(Absyn::type_t, List::list_t<BansheeIf::constraint_t>)@> parse_constraint_file(FILE @`H f);
  extern datatype exn {extern Exit};
}
  string_t token2string(int token);
#ifdef ALL_PARSE_H
using Absyn {
using List {
  namespace Parse {
  extern struct FlatList<`a::A,`r> { struct FlatList<`a,`r> *`r tl; `a hd; };
  typedef struct FlatList<`a,`r> *`r flat_list_t<`a,`r>;
  extern struct Type_specifier {
    bool Signed_spec     : 1;
    bool Unsigned_spec   : 1;
    bool Short_spec      : 1;
    bool Long_spec       : 1;
    bool Long_Long_spec  : 1;
    bool Complex_spec    : 1;
    bool Valid_type_spec : 1;
    type_t Type_spec;
    seg_t loc;
  };
  typedef struct Type_specifier type_specifier_t;
  extern struct Declarator<`yy> {
    qvar_t id;
    seg_t  varloc;
    list_t<type_modifier_t<`yy>,`yy> tms;
  };
  typedef struct Declarator<`yy> declarator_t<`yy>;
  typedef flat_list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t),`yy> declarator_list_t<`yy>;
  typedef flat_list_t<declarator_t<`yy>,`r> declarators_t<`r,`yy>;

  enum Storage_class {
    Typedef_sc, Extern_sc, ExternC_sc, Static_sc, Auto_sc, Register_sc, 
      Abstract_sc, None_sc
  };

    enum ConstraintOps {
      C_AND_OP,
      C_OR_OP,
      C_NOT_OP,
      C_EQ_OP,
      C_INCL_OP
    };

  typedef enum Storage_class storage_class_t;
  extern struct Declaration_spec {
    storage_class_t  sc;
    tqual_t          tq;
    type_specifier_t type_specs;
    bool             is_inline;
    attributes_t     attributes;
  };
  typedef struct Declaration_spec decl_spec_t;
  extern struct Abstractdeclarator<`yy> {
    list_t<type_modifier_t<`yy>,`yy> tms;
  };
  typedef struct Abstractdeclarator<`yy> abstractdeclarator_t<`yy>;
  typedef datatype Pointer_qual @`r pointer_qual_t<`r>;
  typedef list_t<pointer_qual_t<`r>,`r> pointer_quals_t<`r>;
  typedef exp_t (@`H exp_maker_fun_t)(exp_t,exp_t,seg_t);
  }
#ifndef IS_PARSE_Y
  using Parse {
#include "parse_tab.h"
  }
#endif
}}
#endif 
}
#endif
