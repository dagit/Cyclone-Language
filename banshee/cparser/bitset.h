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

#ifndef BITSET_H
#define BITSET_H

typedef struct {
  int bsize, size; /* bsize: size in bytes, size: size in bits */
  long bitset[0];
} *bitset;

#define ASSIGN_BITB(b, n, val) ((b)->bitset[(n) >> 5] = (b)->bitset[(n) >> 5] & ~(1 << ((n) & 31)) | (val << ((n) & 31)))
#define SET_BITB(b, n) ((b)->bitset[(n) >> 5] |= 1 << ((n) & 31))
#define CLEAR_BITB(b, n) ((b)->bitset[(n) >> 5] &= ~(1 << ((n) & 31)))
#define BIT_SETP(b, n) ((b)->bitset[(n) >> 5] & (1 << ((n) & 31)))

bitset new_bitset(region r, int size);
bitset bitcopy(region r, bitset b);
bitset bclearb(bitset b);
void bforeach(void (*f)(int i, void *data), void *data, bitset b);
bitset bunion(region r, bitset b1, bitset b2);
bitset bunionb(bitset b1, bitset b2);
bitset bintersection(region r, bitset b1, bitset b2);
bitset bintersectionb(bitset b1, bitset b2);
bitset bdifference(region r, bitset b1, bitset b2);
bitset bdifferenceb(bitset b1, bitset b2);
bitset bcomplement(region r, bitset b);
bitset bcomplementb(bitset b);
bitset bassignb(bitset b1, bitset b2);
bool bitset_inp(bitset b1, bitset b2);
bool bitset_eqp(bitset b1, bitset b2);
bool bemptyp(bitset b);

#define bitset_scan(var, b) \
  for (var = 0; var < (b)->size; var++) \
    if (BIT_SETP((b), var))

#define bitset_scan_complement(var, b) \
  for (var = 0; var < (b)->size; var++) \
    if (!BIT_SETP((b), var))

#endif
