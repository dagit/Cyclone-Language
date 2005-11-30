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

#include <regions.h>
#include <assert.h>
#include <string.h>

#include "config.h"
#include "bitset.h"

static bitset _new(region r, int size)
{
  int bsize = (size + 31) >> 5;
  bitset new = (bitset)rstralloc(r, sizeof(*new) + bsize * sizeof(new->bitset[0]));

  new->size = size;
  new->bsize = bsize;

  return new;
}

bitset new_bitset(region r, int size)
{
  return bclearb(_new(r, size));
}

bitset bclearb(bitset b)
{
  memset(b->bitset, 0, b->bsize * sizeof(b->bitset[0]));
  return b;
}

#if 0
void bforeach(void (*f)(int i, void *data), void *data, bitset b)
{
  int i, n;
  int bsize = b->bsize;

  for (n = 0, i = 0; i < bsize; i++, n += 8)
    {
      char bi = b->bitset[i];

      if (bi & 1) f(n, data);
      if (bi & 2) f(n + 1, data);
      if (bi & 4) f(n + 2, data);
      if (bi & 8) f(n + 3, data);
      if (bi & 16) f(n + 4, data);
      if (bi & 32) f(n + 5, data);
      if (bi & 64) f(n + 6, data);
      if (bi & 128) f(n + 7, data);
    }
}
#endif

bitset bunion(region r, bitset b1, bitset b2)
{
  int i, bsize = b1->bsize;
  bitset new = _new(r, b1->size);

  assert(b1->size == b2->size);
  for (i = 0; i < bsize; i++) new->bitset[i] = b1->bitset[i] | b2->bitset[i];

  return new;
}

bitset bunionb(bitset b1, bitset b2)
{
  int i, bsize = b1->bsize;

  assert(b1->size == b2->size);
  for (i = 0; i < bsize; i++) b1->bitset[i] |= b2->bitset[i];

  return b1;
}

bitset bintersection(region r, bitset b1, bitset b2)
{
  int i, bsize = b1->bsize;
  bitset new = _new(r, b1->size);

  assert(b1->size == b2->size);
  for (i = 0; i < bsize; i++) new->bitset[i] = b1->bitset[i] & b2->bitset[i];

  return new;
}

bitset bintersectionb(bitset b1, bitset b2)
{
  int i, bsize = b1->bsize;

  assert(b1->size == b2->size);
  for (i = 0; i < bsize; i++) b1->bitset[i] &= b2->bitset[i];

  return b1;
}

bitset bdifference(region r, bitset b1, bitset b2)
{
  int i, bsize = b1->bsize;
  bitset new = _new(r, b1->size);

  assert(b1->size == b2->size);
  for (i = 0; i < bsize; i++) new->bitset[i] = b1->bitset[i] & ~b2->bitset[i];

  return new;
}

bitset bdifferenceb(bitset b1, bitset b2)
{
  int i, bsize = b1->bsize;

  assert(b1->size == b2->size);
  for (i = 0; i < bsize; i++) b1->bitset[i] &= ~b2->bitset[i];

  return b1;
}

bitset bcomplementb(bitset b)
{
  int i, bsize = b->bsize;

  for (i = 0; i < bsize; i++) b->bitset[i] = ~b->bitset[i];
  if (b->size & 31)
    b->bitset[bsize - 1] &= (1 << (b->size & 31)) - 1;
  
  return b;
}

bitset bcomplement(region r, bitset b)
{
  int i, bsize = b->bsize;
  bitset new = _new(r, b->size);

  for (i = 0; i < bsize; i++) new->bitset[i] = ~b->bitset[i];
  if (b->size & 31)
    new->bitset[bsize - 1] &= (1 << (b->size & 31)) - 1;
  
  return new;
}

bitset bassignb(bitset b1, bitset b2)
{
  int i, bsize = b1->bsize;

  assert(b1->size == b2->size);
  for (i = 0; i < bsize; i++) b1->bitset[i] = b2->bitset[i];

  return b1;
}

bitset bitcopy(region r, bitset b)
{
  bitset new = _new(r, b->size);

  return bassignb(new, b);
}

bool bitset_inp(bitset b1, bitset b2)
{
  int i, bsize = b1->bsize;

  assert(b1->size == b2->size);
  for (i = 0; i < bsize; i++)
    if (b1->bitset[i] & ~b2->bitset[i]) return FALSE;

  return TRUE;
}

bool bitset_eqp(bitset b1, bitset b2)
{
  assert(b1->size == b2->size);
  return memcmp(b1->bitset, b2->bitset, b1->bsize * sizeof(b1->bitset[0])) == 0;
}

bool bemptyp(bitset b)
{
  int i, bsize = b->bsize;

  for (i = 0; i < bsize; i++) if (b->bitset[i]) return FALSE;

  return TRUE;
}
