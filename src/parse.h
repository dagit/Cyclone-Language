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
#include <lexing.h>
using Core {
using Absyn {
using List {
using Lexing {
namespace Parse {
  extern list_t<decl_t> parse_file(FILE @f);
  extern bool no_register;
  extern datatype exn {extern Exit};
}
  struct FlatList<`a::A,`r> { struct FlatList<`a,`r> *`r tl; `a hd; };
  typedef struct FlatList<`a,`r> *`r flat_list_t<`a,`r>;
  extern struct Type_specifier {
    bool Signed_spec     : 1;
    bool Unsigned_spec   : 1;
    bool Short_spec      : 1;
    bool Long_spec       : 1;
    bool Long_Long_spec  : 1;
    bool Valid_type_spec : 1;
    type_t Type_spec;
    Position::seg_t loc;
  };
  typedef struct Type_specifier type_specifier_t;
  struct Declarator<`yy::R> {
    qvar_t                  id;
    List::list_t<type_modifier_t<`yy>,`yy> tms;
  };
  typedef struct Declarator<`yy> declarator_t<`yy>;
  typedef flat_list_t<$(declarator_t<`yy>,exp_opt_t),`yy> declarator_list_t<`yy>;

  extern enum Storage_class {
    Typedef_sc, Extern_sc, ExternC_sc, Static_sc, Auto_sc, Register_sc, 
      Abstract_sc
  };
  typedef enum Storage_class storage_class_t;
  extern struct Declaration_spec {
    storage_class_t*         sc;
    tqual_t                  tq;
    type_specifier_t         type_specs;
    bool                     is_inline;
    list_t<attribute_t>      attributes;
  };
  typedef struct Declaration_spec decl_spec_t;
  struct Abstractdeclarator<`yy::R> {
    list_t<type_modifier_t<`yy>,`yy> tms;
  };
  typedef struct Abstractdeclarator<`yy> abstractdeclarator_t<`yy>;

  extern datatype Pointer_qual {
    Numelts_ptrqual(exp_t);
    Region_ptrqual(type_t);
    Thin_ptrqual;
    Fat_ptrqual;
    Zeroterm_ptrqual;
    Nozeroterm_ptrqual;
    Notnull_ptrqual;
    Nullable_ptrqual;
  };
  typedef datatype Pointer_qual @`r pointer_qual_t<`r>;
  typedef list_t<pointer_qual_t<`r>,`r> pointer_quals_t<`r>;

#include "parse_tab.h"
}}}}
#endif
