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
}

#endif
