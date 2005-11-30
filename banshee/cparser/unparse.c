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

#include <stdarg.h>

#include "parser.h"
#include "unparse.h"
#include "semantics.h"
#include "rc.h"
#include "stats.h"
#include "constants.h"
#include "edit.h"
#include "builtins.h"
#include "chkopt.h"
#include "AST_utils.h"

/* Set this to 1 to avoid warnings from gcc about paren use with
   -Wparentheses */
#define CONSERVATIVE_PARENS 1

/* Pick an indentation amount */
#define INDENT 2

/* Precedence levels */
#define P_TOP -1
#define P_COMMA 0
#define P_ASSIGN 1
#define P_COND 2
#define P_OR 3
#define P_AND 4
#define P_BITOR 5
#define P_BITXOR 6
#define P_BITAND 7
#define P_EQUALS 8
#define P_REL 9
#define P_SHIFT 10
#define P_PLUS 11
#define P_TIMES 12
#define P_CAST 13
#define P_CALL 14

/* The output file for unparsing */
static FILE *of;
static int indent_level;
static location output_loc;
static bool at_line_start;
static data_declaration regionof_decl;

static int qwrites, safe_qwrites;

static region tempregion;

typedef struct prt_closure {
  void (*fn)(struct prt_closure *closure);

  const char *name;
  struct prt_closure *parent;
} *prt_closure;


static void indent(void)
{
  indent_level += INDENT;
}

static void unindent(void)
{
  indent_level -= INDENT;
}

static void output_indentation(void)
{
  int i;

  for (i = 0; i < indent_level; i++) putc(' ', of);
}

static void newline(void)
{
  putc('\n', of);
  output_loc.lineno++;
  at_line_start = TRUE;
}

static void startline(void)
{
  if (!at_line_start) newline();
}

static void startline_noindent(void)
{
  startline();
  at_line_start = FALSE;
}

static void output_indent_if_needed(void)
{
  if (at_line_start)
    {
      at_line_start = FALSE;
      output_indentation();
    }
}

static void voutput(char *format, va_list args)
{
  output_indent_if_needed();
  vfprintf(of, format, args);
}

void output(char *format, ...)
{
  va_list args;

  va_start(args, format);
  voutput(format, args);
  va_end(args);
}

void outputln(char *format, ...)
{
  va_list args;

  va_start(args, format);
  voutput(format, args);
  va_end(args);

  newline();
}

static void output_cstring(cstring s)
{
  output_indent_if_needed();
  fwrite(s.data, s.length, 1, of);
}

static void output_line_directive(location l, bool include_filename)
{
  startline_noindent();
  if (include_filename)
    /* The filename doesn't need any quoting as the quotes were not stripped on
       input */
    outputln("#line %lu \"%s\"%s", l.lineno, l.filename,
	     l.in_system_header ? " 3" : "");
  else
    outputln("#line %lu", l.lineno);
}

static void set_location(location l)
{
  /* Ignore dummy locations */
  if (l.filename == dummy_location.filename)
    return;

  if ((l.filename != output_loc.filename &&
       strcmp(l.filename, output_loc.filename)) ||
      l.in_system_header != output_loc.in_system_header)
    output_line_directive(l, TRUE);
  else if (output_loc.lineno != l.lineno)
    {
      /* Just send some newlines for small changes */
      if (output_loc.lineno < l.lineno && output_loc.lineno + 10 >= l.lineno)
	{
	  while (output_loc.lineno != l.lineno)
	    newline();
	}
      else
	output_line_directive(l, FALSE);
    }

  output_loc = l;
}

location output_location(void)
{
  return output_loc;
}


void prt_toplevel_declarations(declaration dlist);
void prt_toplevel_declaration(declaration d);
void prt_asm_decl(asm_decl d);
void prt_extension_decl(extension_decl d);
void prt_data_decl(data_decl d);
void prt_ellipsis_decl(ellipsis_decl d);
void prt_function_decl(function_decl d);
void prt_variable_decl(variable_decl d);
void prt_declarator(declarator d, type_element elements, attribute attributes);
void prt_simple_declarator(declarator d, bool container_priority);
void prt_type_elements(type_element elements);
void prt_type_element(type_element em);
void prt_typename(typename tname);
void prt_typeof_expr(typeof_expr texpr);
void prt_typeof_type(typeof_type ttype);
void prt_attribute(attribute a);
void prt_rid(rid r);
void prt_qualifier(qualifier q);
void prt_tag_ref(tag_ref sr);
void prt_fields(declaration flist);
void prt_enumerators(declaration elist);
void prt_field_declaration(declaration d);
void prt_field_extension_decl(extension_decl d);
void prt_field_data_decl(data_decl d);
void prt_field_decl(field_decl fd);
void prt_enumerator(enumerator ed);
void prt_parameters(declaration parms);
void prt_parameter(declaration parm);
void prt_asttype(asttype t);
void prt_word(word w);

void prt_expressions(expression elist, bool isfirst);
void prt_expression(expression e, int context_priority);
void prt_comma(comma e, int context_priority);
void prt_sizeof_type(sizeof_type e, int context_priority);
void prt_alignof_type(alignof_type e, int context_priority);
void prt_label_address(label_address e, int context_priority);
void prt_cast(cast e, int context_priority);
void prt_cast_list(cast_list e, int context_priority);
void prt_conditional(conditional e, int context_priority);
void prt_identifier(identifier e, int context_priority);
void prt_compound_expr(compound_expr e, int context_priority);
void prt_function_call(function_call e, int context_priority);
void prt_array_ref(array_ref e, int context_priority);
void prt_field_ref(field_ref e, int context_priority);
void prt_unary(unary e, int context_priority);
void prt_binary(binary e, int context_priority);
void prt_init_list(init_list e, int context_priority);
void prt_init_index(init_index e, int context_priority);
void prt_init_field(init_field e, int context_priority);
void prt_lexical_cst(lexical_cst e, int context_priority);
void prt_string(string e, int context_priority);
void prt_adjust_rc(adjust_rc rcop, int context_priority);
void prt_global_rc(global_rc rcop, int context_priority);
void prt_update_rc(update_rc rcop, int context_priority);
void prt_rc_fns(declaration rc_fns);
void prt_parameter_declarations(declaration dlist);
void prt_parameter_declaration(declaration d);

void prt_statement(statement s);
void prt_compound_stmt(compound_stmt s);
void prt_compound_declarations(declaration dlist);
void prt_compound_declaration(declaration d);
void prt_asm_stmt(asm_stmt s);
void prt_asm_stmt_plain(asm_stmt s);
void prt_asm_operands(asm_operand olist);
void prt_asm_operand(asm_operand o);
void prt_if_stmt(if_stmt s);
void prt_labeled_stmt(labeled_stmt s);
void prt_expression_stmt(expression_stmt s);
void prt_while_stmt(while_stmt s);
void prt_dowhile_stmt(while_stmt s);
void prt_switch_stmt(switch_stmt s);
void prt_for_stmt(for_stmt s);
void prt_break_stmt(break_stmt s);
void prt_continue_stmt(continue_stmt s);
void prt_return_stmt(return_stmt s);
void prt_goto_stmt(goto_stmt s);
void prt_computed_goto_stmt(computed_goto_stmt s);
void prt_empty_stmt(empty_stmt s);

void prt_label(label l);
void prt_id_label(id_label l);
void prt_case_label(case_label l);
void prt_default_label(default_label l);

void prt_regionof(expression e);

void unparse_start(FILE *to)
{
  tempregion = newregion();
  of = to;
  output_loc = dummy_location;
  at_line_start = TRUE;
  indent_level = 0;

  regionof_decl = lookup_global_id("regionof");
  stats_init();
}

void unparse_end(void) deletes
{
  deleteregion_ptr(&tempregion);
  if (rc_collect_statistics)
    fprintf(stderr, "qwrites: %d/%d\n", safe_qwrites, qwrites);
}

void unparse(FILE *to, declaration program) deletes
{
  unparse_start(to);
  prt_toplevel_declarations(program);
  unparse_end();
}

void prt_toplevel_declarations(declaration dlist)
{
  declaration d;

  scan_declaration (d, dlist)
    prt_toplevel_declaration(d);
}

#define PRTCASE(type, x) case kind_ ## type: prt_ ## type(CAST(type, (x))); return

void prt_toplevel_declaration(declaration d)
{
  startline();
  switch (d->kind)
    {
      PRTCASE(asm_decl, d);
      PRTCASE(data_decl, d);
      PRTCASE(function_decl, d);
      PRTCASE(extension_decl, d);
    default: assert(0); break;
    }
}

/* Invariant: all declarations end with ; */
void prt_asm_decl(asm_decl d)
{
  prt_asm_stmt(d->asm_stmt);
}

void prt_extension_decl(extension_decl d)
{
  set_location(d->location);
  output("__extension__ ");
  prt_toplevel_declaration(d->decl);
}

void prt_ellipsis_decl(ellipsis_decl d)
{
  set_location(d->location);
  output("...");
}

void prt_data_decl(data_decl d)
{
  declaration vd;

  prt_type_elements(d->modifiers);
  prt_type_elements(CAST(type_element, d->attributes));

  scan_declaration (vd, d->decls)
    {
      prt_variable_decl(CAST(variable_decl, vd));
      if (vd->next)
	output(", ");
    }
  outputln(";");
}

void prt_parameter_declarations(declaration dlist)
{
  declaration d;

  scan_declaration (d, dlist)
    prt_parameter_declaration(d);
}

void prt_parameter_declaration(declaration d)
{
  startline();
  switch (d->kind)
    {
      PRTCASE(data_decl, d);
      PRTCASE(ellipsis_decl, d);
    default: assert(0); break;
    }
}

void prt_function_decl(function_decl d)
{
  prt_rc_fns(d->rc_fns);
  prt_declarator(d->declarator, d->qualifiers, d->attributes);
  startline();
  prt_parameter_declarations(d->old_parms);
  assert(is_compound_stmt(d->stmt));
  prt_statement(d->stmt);
  newline();
}

/* See prt_rid. This is ugly: GCC has builtin signatures for memcpy, memcmp,
   strcpy, strcmp and strlen which use the const qualifier. If we drop the
   const qualifier when unparsing declarations for these functions, we get
   a warning from gcc. Thus we disable dropping the const while unparsing
   declarations of these functions (a more principled approach would be to
   annotate the const-nodes in the AST with an "ignore me" flag. To be 
   really principled, we should only ignore the consts that cause a problem
   for our initialisations). 
   In extra evil, we enable const qualifiers via a global. */
static bool allow_const_evil_hack;

void prt_variable_decl(variable_decl d)
{
  if (d->ddecl == builtin_memcpy || d->ddecl == builtin_memcmp ||
      d->ddecl == builtin_strcpy || d->ddecl == builtin_strcmp ||
      d->ddecl == builtin_strlen)
    allow_const_evil_hack = TRUE;
  prt_declarator(d->declarator, NULL, d->attributes);
  allow_const_evil_hack = FALSE;

  if (d->asm_stmt)
    prt_asm_stmt_plain(d->asm_stmt);

  if (d->arg1)
    {
      output(" = ");
      prt_expression(d->arg1, P_ASSIGN);
    }
}

void prt_declarator(declarator d, type_element elements, attribute attributes)
{
  prt_type_elements(elements);
  prt_type_elements(CAST(type_element, attributes));
  prt_simple_declarator(d, 0);
}

void prt_simple_declarator(declarator d, bool not_star)
{
  if (d)
    switch (d->kind)
      {
      case kind_function_declarator:
	prt_simple_declarator(CAST(function_declarator, d)->declarator, TRUE);
	prt_parameters(CAST(function_declarator, d)->parms);
	break;
      case kind_array_declarator:
	{
	  array_declarator ad = CAST(array_declarator, d);

	  prt_simple_declarator(ad->declarator, TRUE);
	  if (!ad->arg1)
	    output("[]");
	  else
	    {
	      set_location(ad->arg1->location);
	      output("[");
	      prt_expression(ad->arg1, P_TOP);
	      output("]");
	    }
	  break;
	}
      case kind_pointer_declarator:
	{
	  pointer_declarator pd = CAST(pointer_declarator, d);

	  if (pd->qualifiers)
	    set_location(pd->qualifiers->location);
	  if (not_star)
	    output("(");
	  output("*");
	  prt_type_elements(pd->qualifiers);
	  prt_simple_declarator(pd->declarator, FALSE);
	  if (not_star)
	    output(")");
	  break;
	}
      case kind_identifier_declarator:
	set_location(d->location);
	output_cstring(CAST(identifier_declarator, d)->cstring);
	break;

      default: assert(0); break;
      }
}

void prt_type_elements(type_element elements)
{
  type_element em;

  scan_type_element (em, elements)
    {
      prt_type_element(em);
      output(" ");
    }
}

void prt_type_element(type_element em)
{
  switch (em->kind)
    {
      PRTCASE(typename, em);
      PRTCASE(typeof_expr, em);
      PRTCASE(typeof_type, em);
      PRTCASE(attribute, em);
      PRTCASE(rid, em);
      PRTCASE(qualifier, em);
    default:
      if (is_tag_ref(em))
	prt_tag_ref(CAST(tag_ref, em));
      else
	assert(0);
      break;
    }
}

void prt_typename(typename tname)
{
  set_location(tname->location);
  output("%s", tname->ddecl->name);
}

void prt_typeof_expr(typeof_expr texpr)
{
  set_location(texpr->location);
  output("typeof(");
  prt_expression(texpr->arg1, P_TOP);
  output(")");
}

void prt_typeof_type(typeof_type ttype)
{
  set_location(ttype->location);
  output("typeof(");
  prt_asttype(ttype->asttype);
  output(")");
}

void prt_attribute(attribute a)
{
  set_location(a->location);
  output("__attribute((");
  prt_word(a->word1);
  if (a->word2 || a->args)
    {
      output("(");
      if (a->word2)
	prt_word(a->word2);
      prt_expressions(a->args, a->word2 == NULL);
      output(")");
    }
  output("))");
}

void prt_rid(rid r)
{
  set_location(r->location);
  output("%s", rid_name(r));
}

void prt_qualifier(qualifier q)
{
  /* We drop all const's in the output to allow initialisers to be moved to
     the start of the block. This is not a big issue, as we have already
     type-checked the input program. */
  if (q->id == volatile_qualifier || (q->id == const_qualifier && allow_const_evil_hack))
    {
      set_location(q->location);
      output("%s", qualifier_name(q->id));
    }
}

void prt_tag_ref(tag_ref tr)
{
  set_location(tr->location);
  switch (tr->kind)
    {
    case kind_struct_ref: output("struct "); break;
    case kind_union_ref: output("union "); break;
    case kind_enum_ref: output("enum "); break;
    default: assert(0);
    }

  if (tr->word1)
    prt_word(tr->word1);
  if (tr->defined)
    {
      if (tr->kind == kind_enum_ref)
	prt_enumerators(tr->fields);
      else
	prt_fields(tr->fields);
    }
  if (tr->attributes)
    {
      output(" ");
      prt_type_elements(CAST(type_element, tr->attributes));
    }
}

void prt_enumerators(declaration elist)
{
  declaration d;

  output(" {");
  indent();
  startline();
  scan_declaration (d, elist)
    {
      prt_enumerator(CAST(enumerator, d));
      if (d->next)
	output(", ");
    }
  unindent();
  startline();
  output("}");
}

void prt_fields(declaration flist)
{
  declaration d;

  output(" {");
  indent();
  startline();
  scan_declaration (d, flist)
    prt_field_declaration(d);
  unindent();
  startline();
  output("}");
}

void prt_field_declaration(declaration d)
{
  if (is_extension_decl(d))
    prt_field_extension_decl(CAST(extension_decl, d));
  else
    prt_field_data_decl(CAST(data_decl, d));
}

void prt_field_extension_decl(extension_decl d)
{
  set_location(d->location);
  output("__extension__ ");
  prt_field_declaration(d->decl);
}

void prt_field_data_decl(data_decl d)
{
  declaration fd;

  prt_type_elements(d->modifiers);
  prt_type_elements(CAST(type_element, d->attributes));

  scan_declaration (fd, d->decls)
    {
      prt_field_decl(CAST(field_decl, fd));
      if (fd->next)
	output(", ");
    }
  outputln(";");
}

void prt_field_decl(field_decl fd)
{
  prt_declarator(fd->declarator, NULL, fd->attributes);
  if (fd->arg1)
    {
      output(" : ");
      prt_expression(fd->arg1, P_TOP);
    }
}

void prt_enumerator(enumerator ed)
{
  set_location(ed->location);
  output_cstring(ed->cstring);
  if (ed->arg1)
    {
      output(" = ");
      prt_expression(ed->arg1, P_ASSIGN);
    }
}

void prt_parameters(declaration parms)
{
  declaration d;

  output("(");
  scan_declaration (d, parms)
    {
      prt_parameter(d);
      if (d->next)
	output(", ");
    }
  output(")");
}

void prt_parameter(declaration parm)
{
  switch (parm->kind)
    {
    case kind_oldidentifier_decl:
      set_location(parm->location);
      output_cstring(CAST(oldidentifier_decl, parm)->cstring);
      break;
    case kind_ellipsis_decl:
      set_location(parm->location);
      output("...");
      break;
    case kind_data_decl:
      {
	data_decl dd = CAST(data_decl, parm);
	variable_decl vd = CAST(variable_decl, dd->decls);
	prt_type_elements(CAST(type_element, dd->attributes));
	prt_declarator(vd->declarator, dd->modifiers, vd->attributes);
	break;
      }
    default: assert(0); break;
    }
}

void prt_asttype(asttype t)
{
  prt_declarator(t->declarator, t->qualifiers, NULL);
}

void prt_word(word w)
{
  set_location(w->location);
  output_cstring(w->cstring);
}

void prt_expressions(expression elist, bool isfirst)
{
  expression e;

  scan_expression (e, elist)
    {
      if (!isfirst) output(", ");
      isfirst = FALSE;
      prt_expression(e, P_ASSIGN); /* priority is that of assignment */
    }
}

#define PRTEXPR(type, x) case kind_ ## type: prt_ ## type(CAST(type, (x)), context_priority); return

/* Context priorities are that of the containing operator, starting at 0
   for , going up to 14 for ->, . See the symbolic P_XX constants 
   P_TOP (-1) is used for contexts with no priority restrictions. */
void prt_expression(expression e, int context_priority)
{
  switch (e->kind) 
    {
      PRTEXPR(comma, e);
      PRTEXPR(sizeof_type, e);
      PRTEXPR(alignof_type, e);
      PRTEXPR(label_address, e);
      PRTEXPR(cast, e);
      PRTEXPR(cast_list, e);
      PRTEXPR(conditional, e);
      PRTEXPR(identifier, e);
      PRTEXPR(compound_expr, e);
      PRTEXPR(function_call, e);
      PRTEXPR(array_ref, e);
      PRTEXPR(field_ref, e);
      PRTEXPR(init_list, e);
      PRTEXPR(init_index, e);
      PRTEXPR(init_field, e);
    case kind_string_cst:
      PRTEXPR(lexical_cst, e);
      PRTEXPR(string, e);
      PRTEXPR(adjust_rc, e);
      PRTEXPR(global_rc, e);
      PRTEXPR(update_rc, e);
    default: 
      if (is_unary(e))
	{
	  prt_unary(CAST(unary, e), context_priority);
	  return;
	}
      assert(is_binary(e));
      prt_binary(CAST(binary, e), context_priority);
      return;
    }
}

#define OPEN(pri) \
  if (pri < context_priority) \
    output("(")

#define CLOSE(pri) \
  if (pri < context_priority) \
    output(")")

void prt_comma(comma e, int context_priority)
{
  OPEN(P_COMMA);
  prt_expressions(e->arg1, TRUE);
  CLOSE(P_COMMA);
}

void prt_sizeof_type(sizeof_type e, int context_priority)
{
  set_location(e->location);
  output("sizeof(");
  prt_asttype(e->asttype);
  output(")");
}

void prt_alignof_type(alignof_type e, int context_priority)
{
  set_location(e->location);
  output("__alignof__(");
  prt_asttype(e->asttype);
  output(")");
}

void prt_label_address(label_address e, int context_priority)
{
  set_location(e->location);
  output("&&");
  prt_id_label(e->id_label);
}

void prt_cast(cast e, int context_priority)
{
  OPEN(P_CAST);
  set_location(e->location);
  output("(");
  prt_asttype(e->asttype);
  output(")");
  prt_expression(e->arg1, P_CAST);
  CLOSE(P_CAST);
}

void prt_cast_list(cast_list e, int context_priority)
{
  OPEN(P_CAST);
  set_location(e->location);
  output("(");
  prt_asttype(e->asttype);
  output(")");
  prt_init_list(CAST(init_list, e->init_expr), P_ASSIGN);
  CLOSE(P_CAST);
}

void prt_conditional(conditional e, int context_priority)
{
  OPEN(P_COND);
  prt_expression(e->condition, P_COND);
  output(" ? ");
  if (e->arg1)
    prt_expression(e->arg1, P_COND);
  output(" : ");
  prt_expression(e->arg2, P_COND);
  CLOSE(P_COND);
}

void prt_identifier(identifier e, int context_priority)
{
  set_location(e->location);
  output_cstring(e->cstring);
}

void prt_compound_expr(compound_expr e, int context_priority)
{
  set_location(e->location);
  output("(");
  prt_compound_stmt(CAST(compound_stmt, e->stmt));
  output(")");
}

void prt_function_call(function_call e, int context_priority)
{
  /* Check for regionof and rewrite as direct access to page->region array */
  if (call_to(regionof_decl, e) && e->args && !e->args->next)
    {
      output("(");
      if (rc_pairs_array)
	output("__rcregions[");
      prt_regionof(e->args);
      if (rc_pairs_array)
	output("]");
      output(")");
    }
  else if (e->va_arg_call)
    {
      /* The extra parentheses are added because gcc 2.96 (aka redhat 7's
	 gcc) has a broken syntax for __builtin_va_arg */
      output("(__builtin_va_arg(");
      prt_expression(e->args, P_ASSIGN);
      output(", ");
      prt_asttype(e->va_arg_call);
      output("))");
    }
  else
    {
      prt_expression(e->arg1, P_CALL);
      output("(");
      prt_expressions(e->args, TRUE);
      output(")");
    }
}

void prt_array_ref(array_ref e, int context_priority)
{
  prt_expression(e->arg1, P_CALL);
  output("[");
  prt_expression(e->arg2, P_TOP);
  output("]");
}

void prt_field_ref(field_ref e, int context_priority)
{
  /* Reconstruct -> for nicer output */
  if (is_dereference(e->arg1))
    {
      prt_expression(CAST(dereference, e->arg1)->arg1, P_CALL);
      output("->");
    }
  else
    {
      prt_expression(e->arg1, P_CALL);
      output(".");
    }
  output_cstring(e->cstring);
}

void prt_unary(unary e, int context_priority)
{
  char *op = NULL, *postop = NULL;
  int pri = 0;

  /* Yuck. Evil hack because gcc is broken (breaks the a[i] === *(a+i)
     rule when a is a non-lvalue array). So we undo our earlier rewrite
     (from fix.c) of a[i] as *(a+i). Note that gcc doesn't allow i[a] in
     this case (bozos at work?) */
  if (is_dereference(e) && is_plus(e->arg1))
    {
      plus derefed = CAST(plus, e->arg1);

      if (type_array(derefed->arg1->type))
	{
	  prt_array_ref(derefed, context_priority);
	  return;
	}
    }
  switch (e->kind)
    {
    case kind_dereference: op = "*"; break;
    case kind_extension_expr: op = "__extension__ "; break;
      /* Higher priority for sizeof/alignof expr because we must
	 add parens around sizeof cast_expr (e.g. sizeof((char)x), not
	 sizeof (char)x */
    case kind_sizeof_expr: op = "sizeof "; pri = P_CALL; break;
    case kind_alignof_expr: op = "__alignof__ "; pri = P_CALL; break;
    case kind_realpart: op = "__real__ "; break;
    case kind_imagpart: op = "__imag__ "; break;
    case kind_address_of: op = "&"; break;
    case kind_unary_minus: op = "-"; break;
    case kind_unary_plus: op = "+"; break;
    case kind_preincrement: op = "++"; break;
    case kind_predecrement: op = "--"; break;
    case kind_postincrement: postop = "++"; break;
    case kind_postdecrement: postop = "--"; break;
    case kind_conjugate: case kind_bitnot: op = "~"; break;
    case kind_not: op = "!"; break;
    default: assert(0); return;
    }
  OPEN(P_CAST);
  set_location(e->location);
  if (op)
    {
      output(op);
      if (is_unary(e->arg1))
	output(" "); /* Catch weirdness such as - - x */
      if (!pri)
	pri = P_CAST;
    }
  prt_expression(e->arg1, pri ? pri : P_CALL);
  if (postop)
    output(postop);
  CLOSE(P_CAST);
}

const char *binary_op_name(ast_kind kind)
{
  switch (kind)
    {
    case kind_plus: return "+"; 
    case kind_minus: return "-"; 
    case kind_times: return "*"; 
    case kind_divide: return "/"; 
    case kind_modulo: return "%"; 
    case kind_lshift: return "<<"; 
    case kind_rshift: return ">>"; 
    case kind_leq: return "<="; 
    case kind_geq: return ">="; 
    case kind_lt: return "<"; 
    case kind_gt: return ">"; 
    case kind_eq: return "=="; 
    case kind_ne: return "!="; 
    case kind_bitand: return "&"; 
    case kind_bitor: return "|"; 
    case kind_bitxor: return "^"; 
    case kind_andand: return "&&"; 
    case kind_oror: return "||"; 
    case kind_assign: return "="; 
    case kind_plus_assign: return "+="; 
    case kind_minus_assign: return "-="; 
    case kind_times_assign: return "*="; 
    case kind_divide_assign: return "/="; 
    case kind_modulo_assign: return "%="; 
    case kind_lshift_assign: return "<<="; 
    case kind_rshift_assign: return ">>="; 
    case kind_bitand_assign: return "&="; 
    case kind_bitor_assign: return "|="; 
    case kind_bitxor_assign: return "^="; 
    default: assert(0); return "<bad>";
    }
}

void prt_binary(binary e, int context_priority)
{
  int pri, lpri, rpri;
  const char *op = binary_op_name(e->kind);
  bool perturb = FALSE;

  switch (e->kind)
    {
    case kind_times: case kind_divide: case kind_modulo:
      lpri = P_TIMES; pri = P_TIMES; rpri = P_CAST; break;
    case kind_plus: case kind_minus:
      lpri = P_PLUS; pri = P_PLUS; rpri = P_TIMES; break;
    case kind_lshift: case kind_rshift:
      pri = P_SHIFT;
      if (CONSERVATIVE_PARENS)
	lpri = rpri = P_TIMES;
      else
	{
	  lpri = P_SHIFT; rpri = P_PLUS; 
	}
      break;
    case kind_leq: case kind_geq: case kind_lt: case kind_gt:
      lpri = P_REL; pri = P_REL; rpri = P_SHIFT; break;
    case kind_eq: case kind_ne:
      lpri = P_EQUALS; pri = P_EQUALS; rpri = P_REL; break;
    case kind_bitand:
      pri = P_BITAND;
      if (CONSERVATIVE_PARENS)
	lpri = rpri = P_TIMES;
      else
	{
	  lpri = P_BITAND; rpri = P_EQUALS; 
	}
      break;
    case kind_bitxor:
      pri = P_BITXOR;
      if (CONSERVATIVE_PARENS)
	lpri = rpri = P_TIMES;
      else
	{
	  lpri = P_BITXOR; rpri = P_BITAND;
	}
      break;
    case kind_bitor:
      pri = P_BITOR;
      if (CONSERVATIVE_PARENS)
	lpri = rpri = P_TIMES;
      else
	{
	  lpri = P_BITOR; rpri = P_BITXOR;
	}
      break;
    case kind_andand:
      lpri = P_AND; pri = P_AND; rpri = P_BITOR; break;
    case kind_oror:
      pri = P_OR;
      if (CONSERVATIVE_PARENS)
	lpri = rpri = P_BITOR;
      else
	{
	  lpri = P_OR; rpri = P_AND; 
	}
      break;
    case kind_assign: case kind_plus_assign: case kind_minus_assign: 
    case kind_times_assign: case kind_divide_assign: case kind_modulo_assign:
    case kind_lshift_assign: case kind_rshift_assign: case kind_bitand_assign:
    case kind_bitor_assign: case kind_bitxor_assign:
      lpri = P_CAST; pri = P_ASSIGN; rpri = P_ASSIGN; break;
    default: assert(0); return;
    }
  OPEN(pri);
  if (flag_perturb && e->kind == kind_assign)
    perturb = drand48() >= 0.8;
  if (perturb)
    output("(perturb_counter++, ");
  prt_expression(e->arg1, lpri);
  set_location(e->location);
  output(" %s ", op);
  prt_expression(e->arg2, rpri);
  if (perturb)
    output(")");
  CLOSE(pri);

  if (e->kind == kind_assign && type_pointer(e->type) && rc_chkopt)
    output_chkopt_debug(e);
}

void prt_lexical_cst(lexical_cst e, int context_priority)
{
  set_location(e->location);
  output_cstring(e->cstring);
}

void prt_string(string e, int context_priority)
{
  expression s;

  scan_expression (s, e->strings)
    prt_expression(s, P_TOP);
}

void prt_regionof(expression e)
{
  output("__rcregionmap[(");
  prt_asttype(ptrint_type_ast);
  output(")");
  prt_expression(e, P_CAST);
  output(" >> " RPAGELOG_STRING "]");
}

void prt_regionof_name(const char *name)
{
  output("__rcregionmap[(");
  prt_asttype(ptrint_type_ast);
  output(")%s >> " RPAGELOG_STRING "]", name);
}

void prt_regionof_closure(prt_closure closure)
{
  output("__rcregionmap[(");
  prt_asttype(ptrint_type_ast);
  output(")(");
  CALL0(closure);
  output(") >> " RPAGELOG_STRING "]");
}

static void prt_inc(char *by)
{
  /* Looks nicer ;-) */
  if (strcmp(by, "1") == 0)
    output("++");
  else if (strcmp(by, "-1") == 0)
    output("--");
  else
    output("+= %s", by);
}

void prt_globalrc_adjust(char *by)
{
  if (rc_pairs_array)
    output("[__rcs[0]]"); /* Remember, a[b] is the same as b[a] */
  else if (rc_pairs_from)
    output("->id[zeroregion.rc]"); /* Remember, a[b] is the same as b[a] */
  else if (rc_pairs)
    output("->rc[0]");
  else
    output("->rc");

  prt_inc(by);
}

void prt_updaterc_adjust(char *by, const char *base)
{
  if (rc_pairs_array)
    output("[__rcs[%s]]", base);
  else if (rc_pairs_from)
    output("->id[%s->rc]", base);
  else if (rc_pairs)
    output("->rc[%s->id]", base);
  else
    output("->rc");

  prt_inc(by);
}

void prt_nelements(expression array)
{
  type basetype = array->type;

  output("sizeof ");
  prt_expression(array, P_CAST);
  output(" / sizeof ");
  while (type_array(basetype))
    {
      output("*");
      basetype = type_array_of(basetype);
    }
  prt_expression(array, P_CAST);
}

void prt_adjust_rc(adjust_rc rcop, int context_priority)
{
  expression of = rcop->arg1;

  set_location(rcop->location);

  output("(");
  indent();

  if (rc_collect_statistics)
    {
      stats_adjust_rc(rcop);
      output(", ");
    }

  if (type_aggregate(of->type))
    {
#if 1
      output("%s(& ", rcop->rcfn_decl->name);
      prt_expression(of, P_CAST);
      output(", %d)", rcop->incrementrc ? 1 : -1);
#else
      output("0");
#endif
    }
  else if (type_array(of->type))
    {
#ifdef ASSUME_GCC
      output("({ int __rctmp;");
      indent();
      newline();
      output("for (__rctmp = 0; __rctmp < ");
      prt_nelements(of);
      output("; __rctmp++) ");
      output("%s((", rcop->rcfn_decl->name);
      prt_expression(of, P_CAST);
      output(") + __rctmp, %d); })", rcop->incrementrc ? 1 : -1);
      unindent();
#else
      output("__rc_adjustarray(");
      prt_expression(of, P_CAST);
      output(", ");
      prt_nelements(of);
      output(", %s, %d)", rcop->rcfn_decl->name, rcop->incrementrc ? 1 : -1);
#endif
    }
  else
    {
      prt_regionof(of);
      prt_globalrc_adjust(rcop->incrementrc ? "1" : "-1");
    }
  if (rc_collect_statistics_end)
    {
      output(", ");
      stats_adjust_rc_end(rcop);
    }

  output(")");
  unindent();
}

void prt_update_rc(update_rc rcop, int context_priority)
{
  /* WARNING: If you change this, you might have to update 
     rc.c:update_rc_temps_needed */
  binary brcop = CAST(binary, rcop);
  expression of = rcop->arg1;

  set_location(rcop->location);
  output("(");
  indent();

  if (rc_collect_statistics)
    {
      stats_update_rc(rcop);
      output(", ");
    }

  if (type_aggregate(of->type))
    {
#if USE_RC_ADJUST_FOR_UPDATE
      output("%s(&", rcop->rcfn_decl->name);
      prt_expression(of, P_CAST);
      output(", -1), ");
#else
      output("%s(&", rcop->rcfn_decl->name);
      prt_expression(of, P_CAST);
      output(", &");
      prt_expression(rcop->arg2, P_CAST);
      output("), ");
#endif
    }
  else if (type_parentptr(of->type))
    {
      qwrites++;
      if (known_parentptr_write(rcop))
	safe_qwrites++;

      if (rc_keepchecks || !(rc_nochecks || known_parentptr_write(rcop) || zero_expression(rcop->arg2)))
	{
#ifdef USE_CHECK_CHILD
	  output("check_child(");
	  prt_regionof_name(rcop->ddecl->name);
	  output(", ");
	  prt_regionof(rcop->arg2);
	  output("), ");
#else
	  const char *rs = rcop->temps[0]->name, *ofs = rcop->temps[1]->name;

	  output("%s = ", rs);
	  prt_regionof_name(rcop->ddecl->name);
	  output(", %s = ", ofs);
	  prt_regionof(rcop->arg2);
	  output(", %s->rid >= %s->rid && %s->rid < %s->nextid ? 0 : abort(), ",
		 rs, ofs, rs, ofs);
#endif
	}
    }
  else if (type_traditional(of->type))
    {
      qwrites++;
      if (known_traditional_write(brcop))
	safe_qwrites++;

      if (rc_keepchecks || !(rc_nochecks || known_traditional_write(brcop)))
	{
	  prt_regionof(rcop->arg2);
	  output(" != &zeroregion ? abort() : 0, ");
	}
    }
  else if (type_sameregion(of->type))
    {
      qwrites++;
      if (known_sameregion_write(rcop))
	safe_qwrites++;

      if (rc_keepchecks || !(rc_nochecks || known_sameregion_write(rcop) || zero_expression(rcop->arg2)))
	{
	  prt_expression(rcop->arg2, P_AND);
	  output(" && ");
	  prt_regionof(rcop->arg2);
	  output(" != ");
	  prt_regionof_name(rcop->ddecl->name);
	  output(" ? abort() : 0, ");
	}
    }
  else if (known_sameregion_write(rcop) || known_traditional_write(brcop) ||
	   zero_expression(rcop->arg2))
    {
#ifdef NULLRC
      prt_expression(of, P_COND);
      output(" ? (");
#endif
      output("%s = ", rcop->temps[0]->name);
      prt_regionof(of);
      output(", %s = ", rcop->temps[1]->name);
      prt_regionof_name(rcop->ddecl->name);
      output(", (%s == %s ? 0 : %s",
	     rcop->temps[1]->name, rcop->temps[0]->name, rcop->temps[0]->name);
      prt_updaterc_adjust("-1", rcop->temps[1]->name);
      output(")");
#ifdef NULLRC
      output(") : 0");
#endif
      output(", ");
    }
  else
    {
#ifdef NULLRC
      prt_expression(of, P_COND);
      output(" ? (");
#endif
#if 1
      output("%s = ", rcop->temps[0]->name);
      prt_regionof(of);
      output(", %s = ", rcop->temps[1]->name);
      prt_regionof(rcop->arg2);
      output(", %s == %s ? 0 : (%s = ", rcop->temps[0]->name, rcop->temps[1]->name,
	     rcop->temps[2]->name);
      prt_regionof_name(rcop->ddecl->name);
      output(", (%s == %s ? 0 : %s", 
	     rcop->temps[2]->name, rcop->temps[0]->name, rcop->temps[0]->name);
      prt_updaterc_adjust("-1", rcop->temps[2]->name);
      output("), (%s == %s ? 0 : %s",
	     rcop->temps[2]->name, rcop->temps[1]->name, rcop->temps[1]->name);
      prt_updaterc_adjust("1", rcop->temps[2]->name);
      output("))");
#endif
#ifdef NULLRC
      output(") : (%s = ", rcop->temps[1]->name);
      prt_regionof(rcop->arg2);
      output(", %s = ", rcop->temps[2]->name);
      prt_regionof_name(rcop->ddecl->name);
      output(", (%s == %s ? 0 : %s",
	     rcop->temps[2]->name, rcop->temps[1]->name, rcop->temps[1]->name);
      prt_updaterc_adjust("1", rcop->temps[2]->name);
      output("))");
#endif
      output(", ");
    }
  if (rc_collect_statistics_end)
    {
      stats_update_rc_end(rcop);
      output(", ");
    }

  prt_expression(of, P_CAST);
  output(" = ");
  prt_expression(rcop->arg2, P_ASSIGN);

#if USE_RC_ADJUST_FOR_UPDATE
  if (type_aggregate(of->type))
    {
      output(", %s(&", rcop->rcfn_decl->name);
      prt_expression(of, P_CAST);
      output(", 1)");
    }
#endif

  output_chkopt_debug(CAST(binary, rcop));
  output(")");
  unindent();
}

void prt_global_rc(global_rc rcop, int context_priority)
{
  /* WARNING: If you change this, you might have to update 
     rc.c:global_rc_temps_needed */
  binary brcop = CAST(binary, rcop);
  expression dest = rcop->arg1, src = rcop->arg2;
  bool stats_end = rc_collect_statistics_end;

  set_location(rcop->location);
  output("(");
  indent();

  if (rc_collect_statistics)
    {
      stats_end = stats_global_rc(rcop) || stats_end;
      output(", ");
    }
  
  if (type_aggregate(dest->type))
    {
#if 1
      output("%s(& ", rcop->rcfn_decl->name);
      prt_expression(dest, P_CAST);
      output(", -1), ");
#endif
      prt_expression(dest, P_CAST);
      output(" = ");
      prt_expression(src, P_ASSIGN);
#if 1
      output(", %s(& ", rcop->rcfn_decl->name);
      prt_expression(dest, P_CAST);
      output(", 1)");
#endif
    }
  else if (type_traditional(dest->type) || type_sameregion(dest->type) ||
	   type_parentptr(dest->type))
    {
      qwrites++;
      if (known_traditional_write(brcop))
	safe_qwrites++;

      prt_expression(dest, P_CAST);
      output(" = ");
      prt_expression(src, P_ASSIGN);
      if (rc_keepchecks || !(rc_nochecks || known_traditional_write(brcop) || zero_expression(src)))
	{
	  output(", ");
	  prt_regionof(dest);
	  output(" != &zeroregion ? abort() : 0");
	}
    }
  else if (known_traditional_write(brcop) || zero_expression(src))
    {
#ifdef NULLRC
      prt_expression(dest, P_COND);
      output(" ? ");
#endif
      prt_regionof(dest);
      prt_globalrc_adjust("-1");
#ifdef NULLRC
      output(" : 0");
#endif
      output(", ");
      prt_expression(dest, P_CAST);
      output(" = ");
      prt_expression(src, P_ASSIGN);
    }
  else
    {
#ifdef NULLRC
      prt_expression(dest, P_COND);
      output(" ? (");
#endif
      output("%s = ", rcop->temps[0]->name);
      prt_regionof(dest);
      output(", ");
      prt_expression(dest, P_CAST);
      output(" = ");
      prt_expression(src, P_ASSIGN);
      output(", %s = ", rcop->temps[1]->name);
      prt_regionof(dest);
      output(", %s == %s ? 0 : (%s", rcop->temps[0]->name, rcop->temps[1]->name,
	     rcop->temps[0]->name);
      prt_globalrc_adjust("-1");
      output(", %s", rcop->temps[1]->name);
      prt_globalrc_adjust("1");
      output(")");
#ifdef NULLRC
      output(") : (");
      prt_expression(dest, P_CAST);
      output(" = ");
      prt_expression(src, P_ASSIGN);
      output(", ");
      prt_regionof(dest);
      prt_globalrc_adjust("1");
      output(")");
#endif
    }
  if (stats_end)
    {
      output(", ");
      stats_global_rc_end(rcop);
    }
  
  output_chkopt_debug(CAST(binary, rcop));
  output(")");
  unindent();
}

static void prt_aggregate_type(type t)
{
  if (type_struct(t))
    output("struct ");
  else
    output("union ");
  output("%s", type_tag(t)->name);
}

static void print_var(prt_closure closure)
{
  output("(*%s)", closure->name);
}

static void print_field(prt_closure closure)
{
  CALL0(closure->parent);
  output(".%s", closure->name);
}

static void print_array(prt_closure closure)
{
  CALL0(closure->parent);
  output("[%s]", closure->name);
}

typedef void (*prtrc_closure)(prt_closure ptr);

static void prt_rcfn(type basetype, prt_closure base, prtrc_closure ptraction)
{
  if (type_pointer(basetype))
    {
      if (!(type_sameregion(basetype) || type_traditional(basetype) ||
	    type_parentptr(basetype)))
	ptraction(base);
    }
  else if (type_contains_cross_pointers(basetype))
    {
      if (type_array(basetype))
	{
	  char *indexname = rstrdup(tempregion, next_temporary());
	  struct prt_closure parray;

	  parray.fn = print_array;
	  parray.parent = base;
	  parray.name = indexname;

	  outputln("{");
	  indent();
	  outputln("unsigned long %s;", indexname);
	  newline();
	  output("for (%s = 0; %s < (sizeof ", indexname, indexname);
	  CALL0(base);
	  output(") / sizeof *");
	  CALL0(base);
	  outputln("; %s++)", indexname);

	  indent();
	  prt_rcfn(type_array_of(basetype), &parray, ptraction);
	  unindent();

	  unindent();
	  outputln("}");
	}
      else
	{
	  tag_declaration tag = type_tag(basetype);
	  field_declaration field;
	  struct prt_closure pfield;

	  /* unions must be handled by programmer-provided rc_adjust fns */
	  /* We now allow unions and just produce bogus code. An error has
	     already been signalled */
	  /*assert(type_struct(basetype));*/

	  pfield.fn = print_field;
	  pfield.parent = base;
      
	  outputln("{");
	  indent();

	  for (field = tag->fieldlist; field; field = field->next)
	    {
	      pfield.name = field->name;
	      prt_rcfn(field->type, &pfield, ptraction);
	    }
	  unindent();
	  outputln("}");
	}
    }
}

static void prt_adjust_ptr(prt_closure ptr)
{
  if (rc_safe_adjustfn)
    {
      output("__rcr1 = ");
      prt_regionof_closure(ptr);
      outputln(";");

      output("if (__rcr1 != base) __rcr1");
    }
  else
      prt_regionof_closure(ptr);

  prt_globalrc_adjust("by");
  outputln(";");
}

void prt_adjust_function_decl(adjust_function_decl d)
{
  struct prt_closure pvar;

  outputln("static size_t %s(void *x, int by)", d->rcfn_decl->name);
  outputln("{");
  indent();
  if (type_pointer(d->type))
    outputln("void **p = x;");
  else
    {
      prt_aggregate_type(d->type);
      outputln("*p = x;");
    }
  newline();

  if (rc_safe_adjustfn)
    {
      output("regionid __rcr1, base = ");
      prt_regionof_name("p");
      outputln(";");
      newline();
    }

#if 1
  pvar.fn = print_var;
  pvar.name = "p";
  prt_rcfn(d->type, &pvar, prt_adjust_ptr);
#endif

  outputln("return sizeof *p;");
  unindent();
  outputln("}");
  newline();
}

static bool evilhack;

static void print_evil(prt_closure closure)
{
  if (evilhack)
    output("(*new)");
  else
    output("(*old)");
}

static void prt_update_ptr(prt_closure ptr)
{
  outputln("{");
  indent();

  output("__rcr1 = ");
  prt_regionof_closure(ptr);
  outputln(";");
  output("__rcr2 = ");
  evilhack = TRUE;
  prt_regionof_closure(ptr);
  evilhack = FALSE;
  outputln(";");

  outputln("if (__rcr1 != __rcr2)");
  outputln("{");
  indent();

  output("if (__rcr1 != base) __rcr1");
  prt_updaterc_adjust("-1", "base");
  outputln(";");

  output("if (__rcr2 != base) __rcr2");
  prt_updaterc_adjust("1", "base");
  outputln(";");

  unindent();
  outputln("}");

  unindent();
  outputln("}");
}

void prt_update_function_decl(update_function_decl d)
{
  struct prt_closure pvar;

  outputln("static void %s(", d->rcfn_decl->name);
  prt_aggregate_type(d->type);
  output(" *old, ");
  prt_aggregate_type(d->type);
  outputln(" *new)");

  outputln("{");
  indent();

  output("regionid __rcr1, __rcr2, base = ");
  prt_regionof_name("old");
  outputln(";");
  newline();

  pvar.fn = print_evil;
  prt_rcfn(d->type, &pvar, prt_update_ptr);

  unindent();
  outputln("}");
  newline();
}

void prt_adjustarray_function_decl(adjustarray_function_decl d)
{
  output("\
static void __rc_adjustarray(void *array, int nelems, adjust_fn *adjustelem, int by)\n\
{\n\
  char *x = array;\n\
\n\
  while (nelems--)\n\
    x += adjustelem(x, by);\n\
}\n\n");
}

void prt_rc_fn(declaration rc_fn)
{
  switch (rc_fn->kind)
    {
      PRTCASE(adjust_function_decl, rc_fn);
      PRTCASE(adjustarray_function_decl, rc_fn);
      PRTCASE(update_function_decl, rc_fn);
    default: assert(0); return;
    }
}

void prt_rc_fns(declaration rc_fns)
{
  declaration rc_fn;

  scan_declaration (rc_fn, rc_fns)
    prt_rc_fn(rc_fn);
}

void prt_init_list(init_list e, int context_priority)
{
  set_location(e->location);
  output("{ ");
  prt_expressions(e->args, TRUE);
  output(" }");
}

void prt_init_index(init_index e, int context_priority)
{
  set_location(e->location);
  output("[");
  prt_expression(e->arg1, P_ASSIGN);
  if (e->arg2)
    {
      output(" ... ");
      prt_expression(e->arg2, P_ASSIGN);
    }
  output("] ");
  prt_expression(e->init_expr, P_ASSIGN);
}

void prt_init_field(init_field e, int context_priority)
{
  prt_word(e->word1);
  output(" : ");
  prt_expression(e->init_expr, P_ASSIGN);
}

void prt_statement(statement s)
{
  switch (s->kind)
    {
      PRTCASE(asm_stmt, s);
      PRTCASE(compound_stmt, s);
      PRTCASE(if_stmt, s);
      PRTCASE(labeled_stmt, s);
      PRTCASE(expression_stmt, s);
      PRTCASE(while_stmt, s);
      PRTCASE(dowhile_stmt, s);
      PRTCASE(switch_stmt, s);
      PRTCASE(for_stmt, s);
      PRTCASE(break_stmt, s);
      PRTCASE(continue_stmt, s);
      PRTCASE(return_stmt, s);
      PRTCASE(goto_stmt, s);
      PRTCASE(computed_goto_stmt, s);
      PRTCASE(empty_stmt, s);
    default: assert(0); return;
    }
}

static void prt_as_compound(statement s)
{
  if (!is_compound_stmt(s))
    outputln("{");
  prt_statement(s);
  if (!is_compound_stmt(s))
    {
      startline();
      outputln("}");
    }
}

void prt_compound_stmt(compound_stmt s)
{
  statement s1;

  set_location(s->location);
  outputln("{");
  indent();
  if (s->id_labels)
    {
      id_label l;

      output("__label__ ");
      scan_id_label (l, s->id_labels)
	{
	  prt_id_label(l);
	  if (l->next) 
	    output(", ");
	}
      outputln(";");
    }
  if (s->decls)
    {
      prt_compound_declarations(s->decls);
      newline();
    }

  scan_statement (s1, s->stmts)
    prt_statement(s1);

  unindent();
  outputln("}");
}

void prt_compound_declarations(declaration dlist)
{
  declaration d;

  scan_declaration (d, dlist)
    prt_compound_declaration(d);
}

void prt_compound_declaration(declaration d)
{
  startline();
  switch (d->kind)
    {
      PRTCASE(data_decl, d);
      PRTCASE(extension_decl, d);
      PRTCASE(function_decl, d);
    default: assert(0); break;
    }
}

void prt_asm_stmt(asm_stmt s)
{
  prt_asm_stmt_plain(s);
  output(";");
}

void prt_asm_stmt_plain(asm_stmt s)
{
  set_location(s->location);
  output(" __asm ");
  if (s->qualifiers)
    prt_type_elements(s->qualifiers);
  output("(");
  prt_expression(s->arg1, P_TOP);
  if (s->asm_operands1 || s->asm_operands2 || s->asm_clobbers)
    {
      output(" : ");
      prt_asm_operands(s->asm_operands1);

      if (s->asm_operands2 || s->asm_clobbers)
	{
	  output(" : ");
	  prt_asm_operands(s->asm_operands2);

	  if (s->asm_clobbers)
	    {
	      output(" : ");
	      prt_expressions(CAST(expression, s->asm_clobbers), TRUE);
	    }
	}
    }
  output(")");
}

void prt_asm_operands(asm_operand olist)
{
  asm_operand o;

  scan_asm_operand (o, olist)
    {
      prt_asm_operand(o);
      if (o->next)
	output(", ");
    }
}

void prt_asm_operand(asm_operand o)
{
  prt_string(o->string, P_TOP);
  output("(");
  prt_expression(o->arg1, P_TOP);
  output(")");
}

void prt_if_stmt(if_stmt s)
{
  set_location(s->location);
  output("if (");
  /* CONSERVATIVE_PARENS: force parens around assignment within if */
  prt_expression(s->condition, CONSERVATIVE_PARENS ? P_COND : P_TOP);
  output(") ");
  indent();
  prt_as_compound(s->stmt1);
  unindent();
  if (s->stmt2)
    {
      startline();
      output("else ");
      indent();
      prt_as_compound(s->stmt2);
      unindent();
    }
}

void prt_labeled_stmt(labeled_stmt s)
{
  prt_label(s->label);
  output(": ");
  indent();
  prt_statement(s->stmt);
  unindent();
}

void prt_expression_stmt(expression_stmt s)
{
  prt_expression(s->arg1, P_TOP);
  outputln(";");
}

void prt_while_stmt(while_stmt s)
{
  set_location(s->location);
  output("while (");
  /* CONSERVATIVE_PARENS: force parens around assignment within while */
  prt_expression(s->condition, CONSERVATIVE_PARENS ? P_COND : P_TOP);
  output(") ");
  indent();
  prt_statement(s->stmt);
  unindent();
}

void prt_dowhile_stmt(while_stmt s)
{
  set_location(s->location);
  output("do ");
  indent();
  prt_statement(s->stmt);
  unindent();
  startline();
  output("while (");
  /* CONSERVATIVE_PARENS: force parens around assignment within do while */
  prt_expression(s->condition, CONSERVATIVE_PARENS ? P_COND : P_TOP);
  outputln(");");
}

void prt_switch_stmt(switch_stmt s)
{
  set_location(s->location);
  output("switch (");
  /* CONSERVATIVE_PARENS: force parens around assignment within switch */
  prt_expression(s->condition, CONSERVATIVE_PARENS ? P_COND : P_TOP);
  output(") ");
  indent();
  prt_statement(s->stmt);
  unindent();
}

void prt_for_stmt(for_stmt s)
{
  set_location(s->location);
  output("for (");
  if (s->arg1)
    prt_expression(s->arg1, P_TOP);
  output("; ");
  if (s->arg2)
    prt_expression(s->arg2, P_TOP);
  output("; ");
  if (s->arg3)
    prt_expression(s->arg3, P_TOP);
  output(") ");
  indent();
  prt_statement(s->stmt);
  unindent();
}  

void prt_break_stmt(break_stmt s)
{
  set_location(s->location);
  outputln("break;");
}

void prt_continue_stmt(continue_stmt s)
{
  set_location(s->location);
  outputln("continue;");
}

void prt_return_stmt(return_stmt s)
{
  set_location(s->location);
  if (s->arg1)
    {
      output("return ");
      prt_expression(s->arg1, P_TOP);
      outputln(";");
    }
  else
    outputln("return;");
}

void prt_goto_stmt(goto_stmt s)
{
  set_location(s->location);
  output("goto ");
  prt_id_label(s->id_label);
  outputln(";");
}

void prt_computed_goto_stmt(computed_goto_stmt s)
{
  set_location(s->location);
  output("goto *");
  prt_expression(s->arg1, P_TOP);
  outputln(";");
}

void prt_empty_stmt(empty_stmt s)
{
  set_location(s->location);
  outputln(";");
}

void prt_label(label l)
{
  switch (l->kind)
    {
      PRTCASE(id_label, l);
      PRTCASE(case_label, l);
      PRTCASE(default_label, l);
    default: assert(0); return;
    }
}

void prt_id_label(id_label l)
{
  set_location(l->location);
  output_cstring(l->cstring);
}

void prt_case_label(case_label l)
{
  set_location(l->location);
  output("case ");
  prt_expression(l->arg1, P_ASSIGN);
  if (l->arg2)
    {
      output(" ... ");
      prt_expression(l->arg2, P_ASSIGN);
    }
}

void prt_default_label(default_label l)
{
  set_location(l->location);
  output("default");
}
