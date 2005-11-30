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

#ifndef ENV_H
#define ENV_H

#include "dhash.h"

typedef struct env *env;

/* Create a new, empty environment with ancestor 'parent'.
   The environment, and any future contents, are allocated in r */
env new_env(region r, env parent);

/* An environment can be deleted by deleting its region */

/* Return parent environment of e */
env env_parent(env e);

/* Return region of e */
region env_region(env e);

/* Find entry s in in environment e. If not found, check ancestors
   except if this_level_only is true.
   Returns entry's value if s is found, NULL otherwise */
void *env_lookup(env e, const char *s, bool this_level_only);

/* Add an entry for s, with value 'value' to environment e.
   If e already contains an entry for s, then the old entry is
   hidden (but will still be found by env_scan).
   s can be NULL (this allows registering of untagged structs/etc
   in environments). Such entries cannot be found by env_lookup.
   Does not copy s. */
void env_add(env e, const char *s, void *value);

/* Scanning */
typedef dhash_scan env_scanner;
void env_scan(env e, env_scanner *scanner);
bool env_next(env_scanner *scanner, const char **name, void **value);

#endif
