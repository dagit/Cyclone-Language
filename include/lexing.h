/* This file is part of the Cyclone Library.
   Copyright (C) 1999-2001 Greg Morrisett, Dan Grossman, AT&T

   This library is free software; you can redistribute it and/or it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place, Suite
   330, Boston, MA 02111-1307 USA. */

// Parts have been ported from Objective Caml:
/***********************************************************************)
(*                                                                     *)
(*                           Objective Caml                            *)
(*                                                                     *)
(*            Xavier Leroy, projet Cristal, INRIA Rocquencourt         *)
(*                                                                     *)
(*  Copyright 1996 Institut National de Recherche en Informatique et   *)
(*  en Automatique.  All rights reserved.  This file is distributed    *)
(*  under the terms of the GNU Library General Public License.         *)
(*                                                                     *)
(***********************************************************************/

#ifndef _LEXING_H_
#define _LEXING_H_

#include <core.h>
#include <cycboot.h>

namespace Lexing {
using Core;

extern datatype exn { extern Error(string_t) };

extern struct lexbuf<`a::B>{ /* use 'a for state that refill_buff might need */
  void   (@refill_buff)(struct lexbuf<`a> @);
  `a     refill_state;

  mstring_t lex_buffer;
  int    lex_buffer_len;
  int    lex_abs_pos;
  int    lex_start_pos;
  int    lex_curr_pos;
  int    lex_last_pos;
  int    lex_last_action;
  bool   lex_eof_reached;
};
typedef struct lexbuf<`a> @Lexbuf<`a>;

extern struct function_lexbuf_state<`b> { 
// instantiation for using function to read
  int (@read_fun)(mstring_t,int,`b);
  `b read_fun_state;
};
typedef struct function_lexbuf_state<`b> @Function_lexbuf_state<`b>;

extern struct lex_tables { 
// for space this should probably be shorts, but who cares
  int ?lex_base;
  int ?lex_backtrk;
  int ?lex_default;
  int ?lex_trans;
  int ?lex_check;
};
typedef struct lex_tables @LexTables;

extern Lexbuf<Function_lexbuf_state<`b>>
  from_function(int (@`H read_fun)(mstring_t<`H>,int,`b), `b);

extern Lexbuf<Function_lexbuf_state<FILE@`r>> from_file(FILE@`r);
extern Lexbuf<bool> from_string(mstring_t);

extern mstring_t<`r> rlexeme (region_t<`r>,Lexbuf<`a>);
extern mstring_t lexeme      (Lexbuf);
extern char      lexeme_char (Lexbuf, int);
extern int       lexeme_start(Lexbuf);
extern int       lexeme_end  (Lexbuf);

}

#endif
