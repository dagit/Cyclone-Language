#ifndef _SYS_TIME_H
#define _SYS_TIME_H

#include <sys/types.h> // for time_t

struct timeval {
  time_t tv_sec;
  long tv_usec;
};

#endif
