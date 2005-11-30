/* This file was modified in 2000-2001 by David Gay for the RC compiler.
   The changes are 
   Copyright (c) 2000-2001 The Regents of the University of California.

   This file is distributed under the terms of the GNU General Public License
   (see below).
*/
#ifndef C_LEX_H
#define C_LEX_H

/* Define constants for communication with c-parse.y.
   Copyright (C) 1987, 1992 Free Software Foundation, Inc.

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

typedef struct
{ 
  char *filename;
  unsigned long lineno;
  bool in_system_header;
} location;

extern location last_location, dummy_location;

enum rid
{
  RID_UNUSED,

  RID_INT,
  RID_CHAR,
  RID_FLOAT,
  RID_DOUBLE,
  RID_VOID,

  RID_UNSIGNED,
  RID_SHORT,
  RID_LONG,
  RID_SIGNED,
  RID_INLINE,
  RID_COMPLEX,

  RID_AUTO,
  RID_STATIC,
  RID_EXTERN,
  RID_REGISTER,
  RID_TYPEDEF,

  RID_MAX
};

#define NORID RID_UNUSED

extern char *traditional token_buffer;	/* Pointer to token buffer.  */
extern bool in_system_header;	/* TRUE while in system header file */

int yylex(void);
void init_lex(void);

#endif
