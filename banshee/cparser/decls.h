/* This file is part of the RC compiler.
   Copyright (C) 2000-2001 The Regents of the University of California.

RC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

RC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#ifndef DECLS_H
#define DECLS_H

#include "env.h"

/* Types representing declarations */

typedef struct field_declaration {
  struct field_declaration *next; /* Next field in struct/union */
  const char *name;		/* May be NULL for bitfields (if NULL, bitwidth == 0) */
  type type;
  field_decl ast;
  int bitwidth;			/* for bitfields, -1 otherwise */
  size_t offset;		/* in bits, not bytes */
  bool offset_cc;		/* TRUE if offset is a compile-time constant
				   (can be false because of variable-size arrays) */
  bool packed;			/* if packed attribute specified */
} *field_declaration;

/* A struct, union or enum */
typedef struct tag_declaration {
  int kind; /* One of kind_{struct/union/enum}_ref */
  const char *name; /* NULL for anonynous struct/union/enum */
  type reptype; /* The type used to represent an enum, NULL for struct
		   and unions */
  /* fields and fieldlist are only defined for structs/unions */
  env fields;
  field_declaration fieldlist;
  tag_ref ast;
  struct tag_declaration *shadowed; /* Any struct with the same tag defined in enclosing scope */
  bool defined, being_defined;
  bool fields_const, fields_volatile;
  bool transparent_union;	/* transparent_union attribute is present */

  size_t size, alignment;
  bool size_cc;			/* TRUE if size is a compile-time constant
				   (can be false because of variable-size arrays) */
  bool packed;			/* if packed attribute specified */
} *tag_declaration;

typedef enum { decl_variable, decl_constant, decl_function,
	       decl_typedef, decl_error, decl_magic_string } data_kind;

typedef struct data_declaration {
  data_kind kind;
  const char *name;
  type type;

  struct data_declaration *shadowed; /* For extern's shadowing globals in inner scopes */
  declaration definition; /* Pointer to actual definition, if any */
  declaration ast; /* Last declaration */
  expression initialiser; /* NULL if none */

  bool islimbo; /* TRUE if comes from an extern declaration in an inner scope
		   (also true for implicit function declarations) */
  bool isexternalscope; /* == TREE_PUBLIC   */
  bool isfilescoperef; /* == DECL_EXTERNAL */
  bool needsmemory;   /* == TREE_STATIC   */

  /* isused is TRUE if declaration used. For parameters, there is a special
     use during parameter list declaration to support forward parameters:
       - a duplicate parameter declaration is allowed if isused is FALSE
         once a duplicate is seen, isused is set to TRUE
       - parameters are created with isused == TRUE
       - after the forward parameters are seen, they have their isused field
         set to FALSE */
  bool isused;
  bool in_system_header;

  /* For functions */
  enum { function_implicit, function_normal, function_static, function_nested }
    ftype;
  bool isinline;
  bool isexterninline;
  typelist oldstyle_args; /* Type of arguments from old-style declaration */

  /* For variables */
  enum { variable_register, variable_static, variable_normal } vtype;
  bool islocal;
  bool isparameter; /* implies islocal */

  /* For constants */
  known_cst value;

  /* For magic_strings */
  const wchar_t *chars;
  size_t chars_length;

  /* RC specific fields */
  long id;			/* Numbering for analysis purposes */
  bool addressed;		/* locals: address taken, functions: used as value
				   (i.e., not called directly), others: undefined */
  bool istemp;			/* Identified as a "temp", i.e., all uses reached by
				   only one def. False if "addressed" is true. */
  bool interesting;		/* True for unaddressed pointer locals (those to which
				   the rc opts apply) */
  int sr_uses;			/* Number of uses of this var in a sameregion
				   dataflow constraint (if == 1 and istemp then
				   can remove var) */
} *data_declaration;

typedef struct label_declaration {
  const char *name;
  bool explicitly_declared;
  bool used;
  id_label firstuse; /* Never NULL */
  id_label definition; /* NULL until actually defined */
  function_decl containing_function;
} *label_declaration;

typedef struct environment
{
  struct environment *sameregion parent;
  function_decl fdecl;
  bool parm_level;
  env sameregion id_env;
  env sameregion tag_env;
} *environment;

extern data_declaration bad_decl;

#endif
