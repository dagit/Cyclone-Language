#ifndef	_UTIME_H
#define	_UTIME_H	1

#include <core.h>
#include <sys/ctypes.h>
#include <cstddef.h>
#include <time.h>

/* Structure describing file times.  */
struct utimbuf
  {
    time_t actime;		/* Access time.  */
    time_t modtime;		/* Modification time.  */
  };

/* Set the access and modification times of FILE to those given in
   *FILE_TIMES.  If FILE_TIMES is NULL, set them to the current time.  */
extern int utime (string_t file,
		  struct utimbuf *file_times);

#endif /* cutime.h */
