#ifndef _NETINET_IN_H
#define _NETINET_IN_H

#include <sys/types.h>
#include <sys/socket.h>

/* Protocols */
#define IPPROTO_IP 0
#define IPPROTO_ICMP 1
#define IPPROTO_IGMP 2
#define IPPROTO_IPIP 4
#define IPPROTO_TCP 6
#define IPPROTO_EGP 8
#define IPPROTO_PUP 12
#define IPPROTO_UDP 17
#define IPPROTO_IDP 22
#define IPPROTO_TP 29
#define IPPROTO_IPV6 41
#define IPPROTO_ROUTING 43
#define IPPROTO_FRAGMENT 44
#define IPPROTO_RSVP 46
#define IPPROTO_GRE 47
#define IPPROTO_ESP 50
#define IPPROTO_AH 51
#define IPPROTO_ICMPV6 58
#define IPPROTO_NONE 59
#define IPPROTO_DSTOPTS 60
#define IPPROTO_MTP 92
#define IPPROTO_ENCAP 98
#define IPPROTO_PIM 103
#define IPPROTO_COMP 108
#define IPPROTO_RAW 255
#define IPPROTO_MAX 256

/* Standard well-known ports.  */
#define IPPORT_ECHO 7             /* Echo service.  */
#define IPPORT_DISCARD 9          /* Discard transmissions service.  */
#define IPPORT_SYSTAT 11          /* System status service.  */
#define IPPORT_DAYTIME 13         /* Time of day service.  */
#define IPPORT_NETSTAT 15         /* Network status service.  */
#define IPPORT_FTP 21             /* File Transfer Protocol.  */
#define IPPORT_TELNET 23          /* Telnet protocol.  */
#define IPPORT_SMTP 25            /* Simple Mail Transfer Protocol.  */
#define IPPORT_TIMESERVER 37      /* Timeserver service.  */
#define IPPORT_NAMESERVER 42      /* Domain Name Service.  */
#define IPPORT_WHOIS 43           /* Internet Whois service.  */
#define IPPORT_MTP 57

#define IPPORT_TFTP 69            /* Trivial File Transfer Protocol.  */
#define IPPORT_RJE 77
#define IPPORT_FINGER 79          /* Finger service.  */
#define IPPORT_TTYLINK 87
#define IPPORT_SUPDUP 95          /* SUPDUP protocol.  */


#define IPPORT_EXECSERVER 512     /* execd service.  */
#define IPPORT_LOGINSERVER 513    /* rlogind service.  */
#define IPPORT_CMDSERVER 514
#define IPPORT_EFSSERVER 520

    /* UDP ports.  */
#define IPPORT_BIFFUDP 512
#define IPPORT_WHOSERVER 513
#define IPPORT_ROUTESERVER 520

    /* Ports less than this value are reserved for privileged processes.  */
#define IPPORT_RESERVED 1024

    /* Ports greater this value are reserved for (non-privileged) servers.  */
#define IPPORT_USERRESERVED 5000

namespace Inet {

  /* Type to represent a port. */
  typedef unsigned short in_port_t;

  typedef unsigned int in_addr_t;
  struct in_addr {
    in_addr_t s_addr;
  };
  struct sockaddr_in {
    Socket::sa_family_t sin_family; // always AF_INET
    in_port_t sin_port;
    struct in_addr sin_addr;
    char sin_zero[8];
  };
  extern xtunion Socket::sockaddr {
    extern sockaddr_in(struct sockaddr_in);
  };
}

#endif
