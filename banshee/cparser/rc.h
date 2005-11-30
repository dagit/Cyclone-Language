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

#ifndef RC_H
#define RC_H

#define RPAGELOG_STRING "13"

extern asttype ptrint_type_ast;
extern bool flag_perturb;
extern long perturb_seed;


void rc_process(declaration the_program) deletes;
expression rc_check_function_call(function_call fce);

void add_rcop(region r, edge e, data_declaration v, bool increment);

#endif
