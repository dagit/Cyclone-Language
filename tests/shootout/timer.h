#ifndef __TIMER_H__
#define __TIMER_H__

#ifdef TIMING
#include <sys/time.h>

#define START_TIME				\
{ struct timeval __start = { 0, 0 };		\
  struct timeval __end = { 0, 0 };		\
  gettimeofday(&__start,NULL);

#define END_TIME							\
  gettimeofday(&__end,NULL);						\
  __start.tv_sec = __end.tv_sec - __start.tv_sec;			\
  __start.tv_usec = __end.tv_usec - __start.tv_usec;			\
  if(__start.tv_usec < 0) {						\
    __start.tv_usec += 1000000;						\
    __start.tv_sec --;							\
  }									\
  fprintf(stderr, "%d.%06d\n",__start.tv_sec, __start.tv_usec);	\
}
#else
#define START_TIME
#define END_TIME
#endif

#endif
