#ifndef _SYS_TIME_H
#define _SYS_TIME_H

#include <sys/ctypes.h> // for time_t

namespace std {
struct timeval {
  time_t tv_sec;
  long tv_usec;
};

}
#endif
