#ifndef _SYS_SOCKET_H
#define _SYS_SOCKET_H

#include <sys/types.h> // for socklen_t

#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define SOCK_RAW 3
#define SOCK_RDM 4
#define SOCK_SEQPACKET 5
#define SOCK_PACKET 10

#define PF_UNSPEC 0
#define PF_LOCAL 1
#define PF_UNIX PF_LOCAL
#define PF_INET 2
#define PF_AX25 3
#define PF_IPX 4
#define PF_APPLETALK 5
#define PF_ATMPVC 8
#define PF_X25 9
#define PF_INET6 10
#define PF_NETLINK 16
#define PF_PACKET 17

#define AF_UNSPEC PF_UNSPEC
#define AF_LOCAL PF_LOCAL
#define AF_UNIX PF_UNIX
#define AF_INET PF_INET
#define AF_AX25 PF_AX25
#define AF_IPX PF_IPX
#define AF_APPLETALK PF_APPLETALK
#define AF_ATMPVC PF_ATMPVC
#define AF_X25 PF_X25
#define AF_INET6 PF_INET6
#define AF_NETLINK PF_NETLINK
#define AF_PACKET PF_PACKET

namespace Socket {

typedef unsigned short sa_family_t;

xtunion sockaddr;

extern "C" int socket(int domain, int type, int protocol);
//extern int socketpair(int domain, int type, int protocol, int fds[2]);
extern int bind(int fd, const xtunion sockaddr addr);
//extern int getsockname(int fd, xtunion sockaddr @addr);
extern int connect(int fd, const xtunion sockaddr addr);
//extern int getpeername(int fd, xtunion sockaddr @addr);
//extern ssize_t send(int fd, const char ?buf, size_t n, int flags);
//extern ssize_t recv(int fd, char ?buf, size_t n, int flags);
//extern ssize_t sendto(int fd, const char ?buf, size_t n, int flags,
//                      const xtunion sockaddr addr);
//extern ssize_t recvfrom(int fd, char ?buf, size_t n, int flags,
//                        xtunion sockaddr *addr);
//extern ssize_t sendmsg(int fd, const struct msghdr *message, int flags);
//extern ssize_t recvmsg(int fd, struct msghdr *message, int flags);
//extern int getsockopt(int id, int level, int optname, void *optval,
//                      socklen_t *optlen);
extern int setsockopt(int fd, int level, int optname, const char ?optval,
                      socklen_t optlen);
extern "C" int listen(int fd, int n);
extern $(int, xtunion sockaddr addr) accept(int fd);
extern "C" int shutdown(int fd, int how);

}

#endif
