/* This file is part of banshee-pta.
   Copyright (C) 2000-2001 The Regents of the University of California.

banshee-pta is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

banshee-pta is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with banshee-pta see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "andersen_terms.h"

extern int flag_field_based;

void analyze(declaration program) deletes;

void analysis_init(void) deletes;
void analysis_stats(FILE *f);
void analysis_reset(void) deletes;
void print_analysis_results(void) deletes;
void print_points_to_sets(void);
void analysis_print_graph(void);
void analysis_serialize(const char *filename);
void analysis_backtrack(int);
void analysis_region_serialize(const char *filename);
void analysis_region_deserialize(translation t, const char *filename);

#endif /* ANALYSIS_H */
