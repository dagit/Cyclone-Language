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

#include "parser.h"
#include "input.h"

/* Source file current line is coming from.  */
char *input_filename;

/* Top-level source file.  */
char *main_input_filename;

/* Line number in current source file.  */
int lineno;

/* Stream for reading from input file.  */
FILE *finput;

/* Stack of currently pending input files.
   The line member is not accurate for the innermost file on the stack.  */
struct file_stack *input_file_stack;

/* Incremented on each change to input_file_stack.  */
int input_file_stack_tick;

void set_input(FILE *f, char *filename)
{
  input_file_stack = xmalloc(sizeof *input_file_stack);
  input_file_stack->name = main_input_filename = input_filename = filename;
  input_file_stack->line = lineno = 0;
  input_file_stack->indent_level = 0;
  input_file_stack->next = NULL;
  finput = f;
}
