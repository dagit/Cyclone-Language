/* This file is part of the Cyclone Library.
   Copyright (C) 2005 Greg Morrisett, AT&T

   This library is free software; you can redistribute it and/or it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place, Suite
   330, Boston, MA 02111-1307 USA. */

#ifndef YKBUF_H
#define YKBUF_H
#include "ssl.h"
datatype ykproducer {
  ykp_fd(int);
  ykp_ssl(SSL@);
};
typedef struct ykbuf {
  datatype ykproducer *prod;
  unsigned char
    ? @nozeroterm bot,
    ? @nozeroterm save,
    ? @nozeroterm mark,
    ? @nozeroterm cur,
    ? @nozeroterm lim,
    ? @nozeroterm top;
  int             eof;
  int             save_count;
  unsigned int    discarded;
} ykbuf_t;
extern ykbuf_t @fd2ykbuf(int fd);
extern ykbuf_t @string2ykbuf(const char ?s);
extern ykbuf_t @ssl2ykbuf(SSL@`H);
extern void ykfill(ykbuf_t @s,unsigned int n);
extern void ykdumpbuf(ykbuf_t @y);

extern void compress_whitespace(char ?x);

#endif
