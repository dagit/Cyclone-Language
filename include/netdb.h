/* This file is part of the Cyclone Library.
   Copyright (C) 2000-2001 Greg Morrisett

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

#ifndef _NETDB_H
#define _NETDB_H

#include <core.h>
#include <netinet/in.h>

namespace NetDB {
  struct servent {
    mstring_t s_name;
    mstring_t ?s_aliases;
    Inet::in_port_t s_port;
    mstring_t s_proto;
  };

  struct hostent {
    mstring_t h_name;
    mstring_t ?h_aliases;
    int h_addrtype;
    int h_length; // always 4
    struct Inet::in_addr *?h_addr_list;
#define h_addr h_addr_list[0]
  };

  extern struct servent *getservbyname(string_t name, string_t proto);
  extern struct hostent *gethostbyname(string_t name);
  extern void herror(const char ?`r);
}

#endif
