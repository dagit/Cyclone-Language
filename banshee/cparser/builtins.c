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
#include "builtins.h"
#include "semantics.h"

data_declaration gcc_builtin_classify_type, builtin_memcpy, builtin_memcmp,
  builtin_strcpy, builtin_strcmp, builtin_strlen;

void builtins_init(void)
{
  gcc_builtin_classify_type = lookup_global_id("__builtin_classify_type");
  builtin_memcpy = lookup_global_id("memcpy");
  builtin_memcmp = lookup_global_id("memcmp");
  builtin_strcpy = lookup_global_id("strcpy");
  builtin_strcmp = lookup_global_id("strcmp");
  builtin_strlen = lookup_global_id("strlen");
}
