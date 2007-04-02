/* Copyright (C) 2007 AT&T.
   This file is part of the Cyclone project.

   Cyclone is free software; you can redistribute it
   and/or modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   Cyclone is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Cyclone; see the file COPYING. If not,
   write to the Free Software Foundation, Inc., 59 Temple Place -
   Suite 330, Boston, MA 02111-1307, USA. */

// Defines the interface to the termgrammar stack, which is used for creating a
// message as output from parsing a termgrammar term.

#ifndef TG_STACK_H
#define TG_STACK_H
extern void tg_init(char ?? args);

extern void tg_push(const char ?`H);

// push elements from var args.
extern void tg_push_string();
extern void tg_push_int();
extern void tg_push_float();
extern void tg_push_spec(const char ?`H specifier);

// Boolean: are we in printing mode.
/* extern int tg_print(); */
/* extern void tg_disable_print(); */
/* extern int tg_enable_print(); */
/* void tg_enter_binder(); */
/* void tg_exit_binder(); */

// peek at the top element of the stack
extern const char ? tg_peek();
extern const char ? tg_pop();
extern void tg_merge();
#endif
