#ifndef _TIME_H_
#define _TIME_H_

#include <config.h>
#include <core.h>
#include <sys/types.h>
#include <stddef.h>

namespace Time {

  // What should this be under Cygwin?
#define CLOCKS_PER_SEC 1000000l

  struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
  };

  extern "C" time_t time(time_t *`r t);
  extern mstring_t asctime(const struct tm @`r timeptr);
  extern mstring_t ctime(const time_t @`r timep);
  extern "C" struct tm @gmtime(const time_t @`r timep);
  extern "C" struct tm @localtime(const time_t @`r timep);
  extern size_t strftime(string_t s, size_t maxsize, string_t fmt, 
                         const struct tm @`r t);
  extern mstring_t asctime_r(const struct tm @`r, mstring_t);
  extern mstring_t ctime_r(const time_t @`r, mstring_t);

  extern "C" long timezone;
  extern "C" int daylight;
}

#endif
