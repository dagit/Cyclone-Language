#ifndef _UNISTD_H
#define _UNISTD_H

#include <core.h>
#include <sys/types.h> /* for ssize_t */

namespace Unistd {
  extern "C" {
    int close(int);
    ssize_t write(int, const char @, size_t);
  }
}

#endif
