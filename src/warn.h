/* Name resolution
   Copyright (C) 2003 Greg Morrisett, AT&T
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
#ifndef _WARN_H
#define _WARN_H

#include <position.h>
#include <cycboot.h>
#include "absyn.h"

namespace Warn {
  void vwarn(Position::seg_t loc, string_t fmt, parg_t ? ap);

  void warn(Position::seg_t loc, string_t fmt, ... inject parg_t ap)
    __attribute__((format(printf,2,3)));
  void flush_warnings();

  void verr(Position::seg_t loc, string_t fmt, parg_t ? ap);

  void err(Position::seg_t loc, string_t fmt, ... inject parg_t ap)
    __attribute__((format(printf,2,3)));

  `a vimpos(string_t fmt, parg_t ? ap) __attribute__((noreturn));

  `a impos(string_t fmt, ... inject parg_t ap)
    __attribute__((format(printf,1,2), noreturn));

  `a vimpos_loc(Position::seg_t loc, string_t fmt, parg_t ? ap) 
    __attribute__((noreturn));

  `a impos_loc(Position::seg_t loc, string_t fmt, ... inject parg_t ap)
    __attribute__((format(printf,2,3), noreturn));

  extern datatype Warg {
    String(string_t);
    Exp(Absyn::exp_t);
    Typ(Absyn::type_t);
    Qvar(Absyn::qvar_t);
  };
  typedef datatype Warg @warg_t;

  void verr2(Position::seg_t, warg_t ?);
  void err2(Position::seg_t, ... inject warg_t);
  void vwarn2(Position::seg_t, warg_t ?);
  void warn2(Position::seg_t, ... inject warg_t);
}
#endif
