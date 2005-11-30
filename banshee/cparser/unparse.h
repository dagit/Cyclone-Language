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

#ifndef UNPARSE_H
#define UNPARSE_H

void unparse(FILE *to, declaration program) deletes;
void unparse_start(FILE *to);
void unparse_end(void) deletes;

location output_location(void);
void output(char *format, ...);
void outputln(char *format, ...);
const char *binary_op_name(ast_kind kind);
void prt_toplevel_declaration(declaration d);
void prt_nelements(expression array);
void prt_expression(expression e, int context_priority);
void prt_regionof_name(const char *name);
void prt_regionof(expression e);

#endif
