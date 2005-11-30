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
#include "util.h"

double gettime(void)
{
  return 0;
}

DEFINE_ARRAY(wchar_array, wchar_t)

     //#ifndef HAVE_STRTOLD
long double mystrtold(const char *str, char **endptr)
{
  long double ld;
  int count;

  assert(!endptr);
  count = sscanf(str, "%Lf", &ld);
  assert(count == 1);

  return ld;
}

//#endif
