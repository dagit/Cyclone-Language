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

#ifndef BUF_H
#define BUF_H
struct ssl_st;
typedef struct ssl_st SSL;
datatype yyproducer {
  yyp_fd(int);
  yyp_ssl(SSL@);
};
typedef struct yybuf {
  datatype yyproducer *prod;
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
} yybuf_t;
extern yybuf_t @fd2yybuf(int fd);
extern yybuf_t @string2yyb(const char ?s);
extern yybuf_t @ssl2yybuf(SSL@`H);
extern void yyfill(yybuf_t @s,unsigned int n);

extern void compress_whitespace(char ?x);

#endif
