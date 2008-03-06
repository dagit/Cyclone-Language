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
#include "fn.h"
#define YKBSIZE 8192
typedef struct ykbuf ykbuf_t;
datatype ykproducer {
  ykp_p(Fn::fn_t<$(unsigned char ?@nozeroterm,int)@,int>); // general case
  ykp_fd(int);                                             // special (common) case
};
struct ykbuf {
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
};
extern ykbuf_t @fd2ykbuf(int fd);

extern ykbuf_t @p2ykbuf(Fn::fn_t<$(unsigned char ?@nozeroterm`H,int)@`H,int,`H> f);

extern ykbuf_t @string2ykbuf(const char ?s);
extern ykbuf_t @data2ykbuf(const char ?@nozeroterm s);

extern void ykfill(ykbuf_t @s,unsigned int n);
extern void ykdumpbuf(ykbuf_t @y);

/* The functions below are not needed by yakker output but are
   convenient to put here. */

extern void compress_whitespace(char ?x);

/* ykbgets(s,size,ykb) reads in at most one less than size characters
   from ykb and stores them into the buffer pointed to by s.  Reading
   stops after an EOF or a newline.  If a newline is read, it is
   stored into the buffer.  A 0 is stored after the last character in
   the buffer.
   Replacement for fgets(s,size,f).
*/
extern char ?@nozeroterm`r ykbgets(char ?@nozeroterm`r s, int size, ykbuf_t @ykb);

/* ykbread(s,n,ykb) reads n characters from ykb, storing them in s.  It
   returns the number of characters read.  If an error or EOF occurs
   it returns a short count or 0.
   Replacement for fread(s,1,n,f).
*/
extern size_t ykbread(char ?@nozeroterm s, size_t n, ykbuf_t @ykb);

#endif
