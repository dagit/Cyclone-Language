#ifndef _SYS_RESOURCE_H
#define _SYS_RESOURCE_H

namespace Resource {

#define RLIMIT_CPU 0
#define RLIMIT_FSIZE 1
#define RLIMIT_DATA 2
#define RLIMIT_STACK 3
#define RLIMIT_CORE 4
#define RLIMIT_RSS 5
#define RLIMIT_NPROC 6
#define RLIMIT_NOFILE 7
#define RLIMIT_MEMLOCK 8

  typedef unsigned long rlim_t;

  struct rlimit {
    rlim_t rlim_cur;
    rlim_t rlim_max;
  };

  extern "C" int getrlimit(int resource, struct rlimit @`r rlim);
  extern "C" int setrlimit(int resource, const struct rlimit @`r rlim);
}

#endif
