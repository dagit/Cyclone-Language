#ifndef _SYS_SELECT_H
#define _SYS_SELECT_H

#include <sys/ctime.h> // for struct timeval

namespace std {
#ifndef FD_SETSIZE
#define FD_SETSIZE 64
#endif

typedef	long	fd_mask;
#define	NFDBITS	(4 /*==sizeof(fd_mask)*/ * 8) /* bits per mask */
#ifndef	howmany
#define howmany(x,y) (((x)+((y)-1))/(y))
#endif

typedef	struct _types_fd_set {
  fd_mask fds_bits[howmany(FD_SETSIZE, NFDBITS)];
} fd_set;

#define	FD_SET(n,p)   ((p)->fds_bits[(n)/NFDBITS] |= (1L << ((n) % NFDBITS)))
#define	FD_CLR(n,p)   ((p)->fds_bits[(n)/NFDBITS] &= ~(1L << ((n) % NFDBITS)))
#define	FD_ISSET(n,p) ((p)->fds_bits[(n)/NFDBITS] & (1L << ((n) % NFDBITS)))
#define	FD_ZERO(p) for(int i=0;i<howmany(FD_SETSIZE,NFDBITS);i++)(p)->fds_bits[i]=0

int select(int n, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds, struct timeval *timeout);
}
#endif
