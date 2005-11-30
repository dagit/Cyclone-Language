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

#include "parser.h"
#include "unparse.h"
#include "flags.h"
#include "constants.h"
#include "chkopt.h"
#include "AST_utils.h"

/* WARNING: Changes to temp use here must be reflected in 
   global/update_rc_temps_needed in rc.c */

enum { padj_aggregate, padj_array, padj_ptr,
       pupd_aggregate, pupd_trad, pupd_trad_safe, pupd_same, pupd_same_safe,
       pupd_zero, pupd_zero_r, pupd_ptr, pupd_ptr_r, pupd_equal, pupd_equal_r, pupd_wsame, pupd_wsame_r,
       pgbl_aggregate, pgbl_trad, pgbl_trad_safe, pgbl_zero, pgbl_ptr, pgbl_equal, pgbl_wtrad,
       pupd_parent, pupd_parent_safe };

static int profile_cnt;

/* Must match w/ defs in regions.c */
#define ADJUST_TICK "adjust"
#define GLOBAL_TICK "global"
#define UPDATE_TICK "update"

void stats_init(void)
{
  profile_cnt = 1;
}

void output_ptrcat(expression cat)
{
  if (cat)
    {
      output("__rcptrcat(");
      prt_expression(cat, 13);
      output(")");
    }
  else
    output("pcat_none");
}

void output_profile_header(int kind)
{
  location cloc = output_location();

  output("rcs_profile(%d, \"%s\", %d, %d, ",
	 profile_cnt++, cloc.filename, cloc.lineno, kind);
}

void output_profile(int kind, expression cat1, expression cat2)
{
  output_profile_header(kind);
  output_ptrcat(cat1);
  output(", ");
  output_ptrcat(cat2);
  output(")");
}

void output_updptr_profile(update_rc rcop)
{
  location cloc = output_location();
  const char *cname = rcop->ddecl->name;
  int kind;

  if (zero_expression(rcop->arg2))
    kind = pupd_zero;
  else if (known_sameregion_write(rcop))
    kind = pupd_wsame;
  else
    {
      kind = pupd_ptr;
      prt_regionof(rcop->arg1);
      output(" == ");
      prt_regionof(rcop->arg2);
      output(" ? rcs_profile(%d, \"%s\", %d, %d + (&zeroregion != ",
	     profile_cnt++, cloc.filename, cloc.lineno, pupd_equal);
      prt_regionof_name(cname);
      outputln("), 0, 0) :");
    }
  output("rcs_profile(%d, \"%s\", %d, %d + (&zeroregion != ",
	 profile_cnt++, cloc.filename, cloc.lineno, kind);
  prt_regionof_name(cname);
  output("), __rcupdcat(");
  prt_expression(rcop->arg1, 13);
  output(", %s), __rcupdcat(", cname);
  prt_expression(rcop->arg2, 13);
  output(", %s))", cname);
}

static void scost(char *reg)
{
#if 0
  output("({asm(\"rd %%%%tick,%%%%g2;sub %%%s,%%%%g2,%%%s\" : : : \"%%g2\", \"memory\");})", reg, reg);
#else
  output("__rc_stick_%s()", reg);
#endif
}

static void ecost(char *reg)
{
#if 0
  output("({asm(\"rd %%%%tick,%%%%g2;add %%%s,%%%%g2,%%%s\" : : : \"%%g2\", \"memory\");})", reg, reg);
#else
  output("__rc_etick_%s()", reg);
#endif
}

void stats_adjust_rc(adjust_rc rcop)
{
  expression of = rcop->arg1;

  if (rc_stats_costs)
    scost(ADJUST_TICK);
  else if (type_aggregate(of->type))
    if (rc_stats_detailed)
      output_profile(padj_aggregate, NULL, NULL);
    else
      output("rcs_adjust_struct++");
  else if (type_array(of->type))
    if (rc_stats_detailed)
      output_profile(padj_array, NULL, NULL);
    else
      {
	output("rcs_adjust_array += ");
	prt_nelements(rcop->arg1);
      }
  else
    if (rc_stats_detailed)
      output_profile(padj_ptr, of, NULL);
    else
      output("rcs_adjust++");
}

void stats_update_rc(update_rc rcop)
{
  expression of = rcop->arg1;

  if (rc_stats_costs)
    scost(UPDATE_TICK);
  else if (type_aggregate(of->type))
    if (rc_stats_detailed)
      output_profile(pupd_aggregate, NULL, NULL);
    else
      output("rcs_update_struct++");
  else if (type_traditional(of->type))
    if (rc_stats_detailed)
      output_profile(known_traditional_write(CAST(binary, rcop)) ? pupd_trad_safe : pupd_trad, NULL, NULL);
    else
      output("rcs_update_traditional++");
  else if (type_sameregion(of->type))
    if (rc_stats_detailed)
      output_profile(known_sameregion_write(rcop) ? pupd_same_safe : pupd_same, of, NULL);
    else
      output("rcs_update_same++");
  else if (type_parentptr(of->type))
    if (rc_stats_detailed)
      output_profile(known_parentptr_write(rcop) ? pupd_parent_safe : pupd_parent, of, NULL);
    else
      output("rcs_update_parent++");
  else
    if (rc_stats_detailed)
      output_updptr_profile(rcop);
    else
      output("rcs_update++");
}

bool stats_global_rc(global_rc rcop)
{
  binary brcop = CAST(binary, rcop);
  expression of = rcop->arg1;
  bool morework = FALSE;

  if (rc_stats_costs)
    scost(GLOBAL_TICK);
  else if (type_aggregate(of->type))
    if (rc_stats_detailed)
      output_profile(pgbl_aggregate, NULL, NULL);
    else
      output("rcs_global_struct++");
  else if (type_traditional(of->type) || type_sameregion(of->type) ||
	   type_parentptr(of->type))
    if (rc_stats_detailed)
      output_profile(known_traditional_write(brcop) ? pgbl_trad_safe : pgbl_trad, NULL, NULL);
    else 
      output("rcs_global_traditional++");
  else
    if (rc_stats_detailed)
      if (zero_expression(rcop->arg2))
	output_profile(pgbl_zero, of, NULL);
      else if (known_traditional_write(brcop))
	output_profile(pgbl_wtrad, of, NULL);
      else
	{
	  morework = TRUE;
	  output("%s = ", rcop->temps[2]->name);
	  prt_expression(of, 13);
	}
    else 
      output("rcs_global++");

  return morework;
}

void stats_adjust_rc_end(adjust_rc rcop)
{
  if (rc_stats_costs)
    ecost(ADJUST_TICK);
}

void stats_update_rc_end(update_rc rcop)
{
  if (rc_stats_costs)
    ecost(UPDATE_TICK);
}

void stats_global_rc_end(global_rc rcop)
{
  expression of = rcop->arg1;

  if (rc_stats_detailed && type_pointer(of->type) &&
      !type_traditional(of->type) &&
      !zero_expression(rcop->arg2))
    {
      prt_regionof(of);
      output(" == ");
      prt_regionof_name(rcop->temps[2]->name);
      output(" ? ");
      output_profile(pgbl_equal, NULL, NULL);
      output(" : ");
      output_profile_header(pgbl_ptr);
      output("__rcptrcat(%s), ", rcop->temps[2]->name);
      output_ptrcat(of);
      output(")");
      if (rc_stats_costs)
	output(", ");
    }
  if (rc_stats_costs)
    ecost(GLOBAL_TICK);
}

