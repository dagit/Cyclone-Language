/* This file is part of the Cyclone Library.
   Copyright (C) 2001 Greg Morrisett

   This library is free software; you can redistribute it and/or it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place, Suite
   330, Boston, MA 02111-1307 USA. */

/* Some constants taken from the GNU C Library:
   Copyright (C) 1991-1999, 2000, 2001 Free Software Foundation, Inc.
*/


#ifndef _NETINET_IN_H
#define _NETINET_IN_H

#include <sys/ctypes.h>
#include <sys/csocket.h>

/* Protocols */
#define IPPROTO_IP 0	     /* Dummy protocol for TCP.  */
#define IPPROTO_HOPOPTS 0    /* IPv6 Hop-by-Hop options.  */
#define IPPROTO_ICMP 1       /* Internet Control Message Protocol.  */
#define IPPROTO_IGMP 2       /* Internet Group Management Protocol. */
#define IPPROTO_IPIP 4       /* IPIP tunnels (older KA9Q tunnels use 94).  */
#define IPPROTO_TCP 6        /* Transmission Control Protocol.  */
#define IPPROTO_EGP 8        /* Exterior Gateway Protocol.  */
#define IPPROTO_PUP 12       /* PUP protocol.  */
#define IPPROTO_UDP 17       /* User Datagram Protocol.  */
#define IPPROTO_IDP 22       /* XNS IDP protocol.  */
#define IPPROTO_TP 29        /* SO Transport Protocol Class 4.  */
#define IPPROTO_IPV6 41      /* IPv6 header.  */
#define IPPROTO_ROUTING 43   /* IPv6 routing header.  */
#define IPPROTO_FRAGMENT 44  /* IPv6 fragmentation header.  */
#define IPPROTO_RSVP 46      /* Reservation Protocol.  */
#define IPPROTO_GRE 47       /* General Routing Encapsulation.  */
#define IPPROTO_ESP 50       /* encapsulating security payload.  */
#define IPPROTO_AH 51        /* authentication header.  */
#define IPPROTO_ICMPV6 58    /* ICMPv6.  */
#define IPPROTO_NONE 59      /* IPv6 no next header.  */
#define IPPROTO_DSTOPTS 60   /* IPv6 destination options.  */
#define IPPROTO_MTP 92       /* Multicast Transport Protocol.  */
#define IPPROTO_ENCAP 98     /* Encapsulation Header.  */
#define IPPROTO_PIM 103      /* Protocol Independent Multicast.  */
#define IPPROTO_COMP 108     /* Compression Header Protocol.  */
#define IPPROTO_RAW 255      /* Raw IP packets.  */
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

namespace Std {
  /* Type to represent a port. */
  typedef unsigned short in_port_t;

  typedef unsigned int in_addr_t;
  struct in_addr {
    in_addr_t s_addr;
  };

/* Definitions of the bits in an Internet address integer.

   On subnets, host and network parts are found according to
   the subnet mask, not these masks.  */

#define	IN_CLASSA(a)		((((in_addr_t)(a)) & 0x80000000) == 0)
#define	IN_CLASSA_NET		0xff000000
#define	IN_CLASSA_NSHIFT	24
#define	IN_CLASSA_HOST		(0xffffffff & ~IN_CLASSA_NET)
#define	IN_CLASSA_MAX		128

#define	IN_CLASSB(a)		((((in_addr_t)(a)) & 0xc0000000) == 0x80000000)
#define	IN_CLASSB_NET		0xffff0000
#define	IN_CLASSB_NSHIFT	16
#define	IN_CLASSB_HOST		(0xffffffff & ~IN_CLASSB_NET)
#define	IN_CLASSB_MAX		65536

#define	IN_CLASSC(a)		((((in_addr_t)(a)) & 0xe0000000) == 0xc0000000)
#define	IN_CLASSC_NET		0xffffff00
#define	IN_CLASSC_NSHIFT	8
#define	IN_CLASSC_HOST		(0xffffffff & ~IN_CLASSC_NET)

#define	IN_CLASSD(a)		((((in_addr_t)(a)) & 0xf0000000) == 0xe0000000)
#define	IN_MULTICAST(a)		IN_CLASSD(a)

#define	IN_EXPERIMENTAL(a)	((((in_addr_t)(a)) & 0xe0000000) == 0xe0000000)
#define	IN_BADCLASS(a)		((((in_addr_t)(a)) & 0xf0000000) == 0xf0000000)

/* Address to accept any incoming messages.  */
#define	INADDR_ANY		((in_addr_t) 0x00000000)
/* Address to send to all hosts.  */
#define	INADDR_BROADCAST	((in_addr_t) 0xffffffff)
/* Address indicating an error return.  */
#define	INADDR_NONE		((in_addr_t) 0xffffffff)

/* Network number for local host loopback.  */
#define	IN_LOOPBACKNET		127
/* Address to loopback in software to local host.  */
#ifndef INADDR_LOOPBACK
# define INADDR_LOOPBACK	((in_addr_t) 0x7f000001) /* Inet 127.0.0.1.  */
#endif

/* Defines for Multicast INADDR.  */
#define INADDR_UNSPEC_GROUP	((in_addr_t) 0xe0000000) /* 224.0.0.0 */
#define INADDR_ALLHOSTS_GROUP	((in_addr_t) 0xe0000001) /* 224.0.0.1 */
#define INADDR_ALLRTRS_GROUP    ((in_addr_t) 0xe0000002) /* 224.0.0.2 */
#define INADDR_MAX_LOCAL_GROUP  ((in_addr_t) 0xe00000ff) /* 224.0.0.255 */

  struct sockaddr_in {
    sa_family_t sin_family; // always AF_INET
    in_port_t sin_port;
    struct in_addr sin_addr;
    char sin_zero[8];
  };

  extern "C" {
    unsigned long htonl(unsigned long);
    unsigned short htons(unsigned short);
    unsigned long ntohl(unsigned long);
    unsigned short ntohs(unsigned short);
  }
}

#endif
