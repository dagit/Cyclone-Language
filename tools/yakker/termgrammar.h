/* Copyright (C) 2005 Greg Morrisett, AT&T.
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

#ifndef TERMGRAMMAR_H
#define TERMGRAMMAR_H
#include "bnf.h"
extern grammar_t grammar2termgrammar(grammar_t ds);
// version 2
extern grammar_t grammar2termgrammar_UT(grammar_t ds);
// version 3
extern grammar_t grammar2termgrammar_v3(grammar_t ds);

grammar_t grammar2bindgrammar(grammar_t g);

extern grammar_t grammar_RTLT(grammar_t ds);
// deprecated
extern grammar_t grammar_MDTU(grammar_t ds);
// deprecated
extern grammar_t grammar_RRT(grammar_t ds);
extern grammar_t grammar_IT(grammar_t ds);
extern grammar_t grammar_NOLITS(grammar_t ds);
extern grammar_t grammar_RNCT(grammar_t ds);
extern grammar_t grammar_IST(grammar_t ds);
extern grammar_t grammar_final(grammar_t ds);
extern grammar_t grammar_final_v3(grammar_t ds);
#endif
