#ifndef _SYS_SELECT_H
#define _SYS_SELECT_H

#include <sys/ctime.h> // for struct timeval
#include <sys/ctypes.h>

namespace Std {

extern "C" int select(int n, fd_set *readfds, fd_set *writefds,
		      fd_set *exceptfds, struct timeval *timeout);
}
#endif
