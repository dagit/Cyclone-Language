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

#ifndef PR_H
#define PR_H
#include <list.h>
#include "bnf.h"
extern void pr_repeat(FILE @f,repeat_t rp);
// "with_annotations" argument indicates whether to print any annotations
// that are included with the rule, e.g. semantic actions.
extern void pr_rule(FILE @f,rule_t r, int with_annotations);
extern void pr_definition(FILE @f,definition_t d);
extern void pr_definitions(FILE @f,grammar_t ds);
extern const char ?pr_rule2string(rule_t r);
extern int html_mode;
#endif
