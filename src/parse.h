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
#include <stdio.h>
#include <list.h>
#include <lexing.h>
using Core {
using Absyn {
using List {
using Lexing {
namespace Parse {
extern list_t<decl_t> parse_file(FILE @f);
extern bool no_register;
}
extern datatype Type_specifier;
typedef datatype Type_specifier @type_specifier_t;
extern enum Storage_class {
 Typedef_sc, Extern_sc, ExternC_sc, Static_sc, Auto_sc, Register_sc, Abstract_sc
};
typedef enum Storage_class storage_class_t;
extern struct Declaration_spec;
typedef struct Declaration_spec @decl_spec_t;
extern struct Declarator;
typedef struct Declarator @declarator_t;
extern struct Abstractdeclarator;
typedef struct Abstractdeclarator @abstractdeclarator_t;
#include "parse_tab.h"
}}}}
#endif



