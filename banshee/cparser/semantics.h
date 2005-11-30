/* This file is part of the RC compiler.

This file is derived from the GNU C Compiler. It is thus
   Copyright (C) 1987, 88, 89, 92-7, 1998 Free Software Foundation, Inc.
and
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
Boston, MA 02111-1307, USA. */

#ifndef SEMANTICS_H
#define SEMANTICS_H

/* Predefined __builtin_va_list type */
extern type builtin_va_list_type;
extern data_declaration builtin_va_arg_decl;

/* Print an error message now
   for a recent invalid struct, union or enum cross reference.
   We don't print them immediately because they are not invalid
   when used in the `struct foo;' construct for shadowing.  */
void pending_xref_error(void);

void init_semantics(void);

extern environment global_env;

data_declaration lookup_id(const char *s, bool this_level_only);
data_declaration lookup_global_id(const char *s);

/* If elements is 'struct foo' shadow tag foo in the current environment */
void shadow_tag(type_element elements);

/* Like shadow_tag, but a pedwarn has already been issued if warned == 1 */
void shadow_tag_warned(type_element elements, int warned);


declarator make_function_declarator(location l, declarator d, declaration parms, type_element quals);

/* Start definition of function 'elements d' with attributes attribs.
   nested is true for nested function definitions.
   Returns false in case of error.
   Sets current_function_decl to the declaration for this function */
bool start_function(type_element elements, declarator d, attribute attribs,
		    bool nested);

/* Add old-style parameter declarations old_parms to the current function */
void store_parm_decls(declaration old_parms);

/* End definition of current function, furnishing it it's body. */
declaration finish_function(statement body);

/* The function currently being defined */
extern function_decl current_function_decl;



/* Start a new scope */
void pushlevel(bool parm_level);

/* Pop back to enclosing scope */
environment poplevel(void);


/* Categories of variable declarations */
enum { var_typedef, var_register, var_normal, var_static, var_extern };

void split_type_elements(type_element tlist, type_element *declspecs,
			 attribute *attributes);

/* Start definition of variable 'elements d' with attributes
   extra_attributes and attributes, asm specification astmt.
   If initialised is true, the variable has an initialiser.
   Returns the declaration for the variable.
*/
declaration start_decl(declarator d, asm_stmt astmt, type_element elements,
		       bool initialised, attribute extra_attributes,
		       attribute attributes);

/* Finish definition of decl, furnishing the optional initialiser init.
   Returns decl */
declaration finish_decl(declaration decl, expression init);

/* Create definition of function parameter 'elements d' with attributes
   extra_attributes and attributes.
   Returns the declaration for the parameter.
*/
declaration declare_parameter(declarator d, type_element elements,
			      attribute extra_attributes,
			      attribute attributes,
			      bool abstract);

declaration declare_old_parameter(location l, cstring id);


/* Start definition of struct/union (indicated by skind) type tag. */
type_element start_struct(location l, ast_kind skind, word tag);

/* Finish definition of struct/union furnishing the fields and attribs.
   Returns t */
type_element finish_struct(type_element t, declaration fields,
			   attribute attribs);

/* Return a reference to struct/union/enum (indicated by skind) type tag */
type_element xref_tag(location l, ast_kind skind, word tag);

/* Start definition of struct/union (indicated by skind) type tag. */
type_element start_enum(location l, word tag);

/* Finish definition of enum furnishing the names and attribs.
   Returns t */
type_element finish_enum(type_element t, declaration names,
			 attribute attribs);

/* Create declaration of field 'elements d : bitfield' with attributes
   extra_attributes and attributes.
   d can be NULL, bitfield can be NULL, but not both at the same time.
   Returns the declaration for the field.
*/
declaration make_field(declarator d, expression bitfield,
		       type_element elements, attribute extra_attributes,
		       attribute attributes);

declaration make_enumerator(location loc, cstring id, expression value);


/* Create and return type 'elements d' where d is an absolute declarator */
asttype make_type(type_element elements, declarator d);


/* Save the rest of the line in the directives list. Return '\n' */
int save_directive(char *directive);

/* Returns name of r */
char *rid_name(rid r);

/* If statement list l1 ends with an unfinished label, attach l2 to that
   label. Otherwise attach l2 to the end of l1 */
statement chain_with_labels(statement l1, statement l2);

const char *declarator_name(declarator d);

data_declaration lookup_id(const char *s, bool this_level_only);

/* The function currently being defined */
extern function_decl current_function_decl;

data_declaration implicitly_declare(identifier fnid);

void push_label_level(void);
void pop_label_level(void);

void init_data_declaration(data_declaration dd, declaration ast,
			   const char *name, type t);
data_declaration declare(environment env, data_declaration from,
			 bool ignore_shadow);

/* Build a declaration object for a string */
data_declaration declare_string(const char *name, bool wide, size_t length);

environment new_environment(region r, environment parent, bool parm_level);

tag_declaration declare_tag(tag_ref t);
tag_declaration lookup_tag(tag_ref t, bool this_level_only);
tag_declaration declare_global_tag(tag_ref t);
tag_declaration lookup_global_tag(tag_ref t);

#endif
