#ifndef _ARPA_INET_H
#define _ARPA_INET_H

#include <netinet/in.h>

namespace Inet {
  extern int inet_aton(string_t cp, struct in_addr @`r inp);
}

#endif
