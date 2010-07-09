/*
 * Copyright (c) 2001, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 * $Id: lwipopts.h,v 1.0 2009/10/01 KSL Embedded Exp $
 */
#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__


/* We don't have OS */
#define NO_SYS				1

/* Set this define to 1 for enable Critical Region Protection */
/* if you want enable protecton add this #include "arch/sys_arch.h" in sys.h file
   befor type declaration - little bug in lwip if NO_SYS == 1 */
#define  SYS_LIGHTWEIGHT_PROT		0


/* ---------- lwIP Memory Options ---------- */
/* We have 32-bit MCU set memory aligment to 4 bytes */
#define MEM_ALIGNMENT			4

/* MEM_SIZE: the size of the heap memory */
#define MEM_SIZE			4096


/* ---------- Memory  Pools ---------- */
/* MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
   sends a lot of data out of ROM (or other static memory), this
   should be set high. '16' - is predefined in opt.h */
#define MEMP_NUM_PBUF			16

/* MEMP_NUM_RAW_PCB: Number of raw connection PCBs
   (requires the LWIP_RAW option) '8' - is predefined in opt.h */
#define MEMP_NUM_RAW_PCB		8

/* MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
   per active UDP "connection". '4' - is predefined in opt.h */
#define MEMP_NUM_UDP_PCB		2

/* MEMP_NUM_TCP_PCB: the number of simulatenously active TCP connections.
   (requires the LWIP_TCP option) '5' - is predefined in opt.h */
#define MEMP_NUM_TCP_PCB		4

/* MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP connections.
 (requires the LWIP_TCP option) '8' - is predefined in opt.h */
#define MEMP_NUM_TCP_PCB_LISTEN		4

/* MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP segments.
  (requires the LWIP_TCP option) '16' - is predefined in opt.h */
#define MEMP_NUM_TCP_SEG		16

/* The following four are used only with the sequential API and can be
   set to '0' if application only will use the raw API */

/* MEMP_NUM_NETBUF: the number of struct netbufs. */
#define MEMP_NUM_NETBUF			0

/* MEMP_NUM_NETCONN: the number of struct netconns. */
#define MEMP_NUM_NETCONN		0

/* MEMP_NUM_TCPIP_MSG: the number of struct tcpip_msg, which is used
   for sequential API communication and incoming packets. Used in
   src/api/tcpip.c. */
#define MEMP_NUM_TCPIP_MSG_API		0
#define MEMP_NUM_TCPIP_MSG_INPKT	0


/* ---------- ARP options ---------- */
/* Enable ARP functionality */
#define LWIP_ARP			1


/* ---------- ICMP options ---------- */
/* Enable ICMP module inside the IP stack */
#define LWIP_ICMP			1


/* ---------- IP options ---------- */
/* IP_REASSEMBLY=1 is default: Reassemble incoming fragmented IP packets */
#define IP_REASSEMBLY                   0

/* IP_FRAG=1 is default: Fragment outgoing IP packets if their size exceeds MTU */
#define IP_FRAG                         0


/* ---------- AUTOIP options ---------- */
/* Define LWIP_AUTOIP to 1 if you want AutoIP configuration of interfaces */
#define LWIP_AUTOIP                     0


/* ---------- DHCP options ---------- */
/* Define LWIP_DHCP to 1 if you want DHCP configuration of interfaces */
#define LWIP_DHCP			0

/* Enable if you want to do an ARP check on the offered address  (recommended). */
#define DHCP_DOES_ARP_CHECK		((LWIP_DHCP) && (LWIP_ARP))

/* Allow DHCP and AUTOIP to be both enabled on the same interface at the same time */
#define LWIP_DHCP_AUTOIP_COOP		((LWIP_DHCP) && (LWIP_AUTOIP))


/* ---------- UDP options ---------- */
/* Define LWIP_UDP to 1 if you want enable UDP for lwIP stack */
#define LWIP_UDP			1


/* ---------- SNMP options ---------- */
/* Define LWIP_SNMP to 1 if you want enable SNMP module. UDP must be available for SNMP */
#define LWIP_SNMP			0


/* ---------- IGMP options ---------- */
/* Define LWIP_IGMP to 1 if you want enable IGMP module */
#define LWIP_IGMP			0


/* ---------- DNS options ---------- */
/* Define LWIP_DNS to 1 if you want enable DNS */
#define LWIP_DNS			0


/* ---------- TCP options ---------- */
/* Enable TCP in lwIP stack */
#define LWIP_TCP			1

/* TCP Maximum segment size. (default is 128)
   For the receive side, this MSS is advertised to the remote side when opening a connection.
   For the transmit size, this MSS sets an upper limit on the MSS advertised by the remote host.
 */
#define TCP_MSS                         128

/* TCP receive window. This must be at least (2 * TCP_MSS) for things to work well */
#define TCP_WND                         2048

/* Maximum number of retransmissions of data segments. */
#define TCP_MAXRTX			4

/* Maximum number of retransmissions of SYN segments. */
#define TCP_SYNMAXRTX			4

/* Controls if TCP should queue segments that arrive out of order.
   Define to 0 if your device is low on memory. */
#define TCP_QUEUE_OOSEQ			1

/* TCP sender buffer space (bytes). */
#define TCP_SND_BUF			256

/* Define LWIP_TCP_TIMESTAMPS to 1 to enable support the TCP timestamp option */
#define LWIP_TCP_TIMESTAMPS		0


/* ---------- lwIP API options ---------- */
/* Use PCB callback function for event processing */
//#define LWIP_EVENT_API			0
//#define LWIP_CALLBACK_API		1


/* ---------- pBUF options ---------- */
#define ETH_PAD_SIZE            0

/* PBUF_LINK_HLEN: the number of bytes that should be allocated for a
   link level header. The default is 14, the standard value for Ethernet */
#define PBUF_LINK_HLEN			14

/* PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. The default is
   designed to accomodate single full size TCP frame in one pbuf, including
   TCP_MSS, IP header, and link header */
#define PBUF_POOL_BUFSIZE               LWIP_MEM_ALIGN_SIZE(TCP_MSS+40+PBUF_LINK_HLEN)


/* ---------- Network Interfaces options ---------- */
/* Don't use any option in this port or use values set in opt.h file */


/* ---------- LOOPIF options ---------- */
/* Define LWIP_HAVE_LOOPIF to 1 for enable support for loop interface (127.0.0.1) */
#define LWIP_HAVE_LOOPIF		0


/* ---------- Thread options ---------- */
/* Don't use any option in this port or use values set in opt.h file */


/* ---------- Sequential layer options  ---------- */
/* Define LWIP_NETCONN to 1 for enable Netconn API (require to use api_lib.c) */
#define LWIP_NETCONN			0


/* ---------- Socket options ---------- */
/* Define LWIP_SOCKET to 1 for enable Socket API (require to use sockets.c) */
#define LWIP_SOCKET				0


/* ---------- Statistics options ---------- */
/* Define LWIP_STATS to 1 for enable statistics collection in lwip_stats */
#define LWIP_STATS				0


/* ---------- Checksum options ---------- */
/* Enable generation checksums in software for outgoing IP packets */
#define CHECKSUM_GEN_IP			1

/* Enable generation checksums in software for outgoing UDP packets */
#define CHECKSUM_GEN_UDP		1

/* Enable generation checksums in software for outgoing TCP packets */
#define CHECKSUM_GEN_TCP		1

/* Enable check checksums in software for incoming IP packets */
#define CHECKSUM_CHECK_IP		1

/* Enable check checksums in software for incoming UDP packets */
#define CHECKSUM_CHECK_UDP		1

/* Enable check checksums in software for incoming TCP packets */
#define CHECKSUM_CHECK_TCP		1


/* ---------- Debugging options ---------- */
/* Unrem next define for enable dubug information in lwIP stack */
// #define LWIP_DEBUG

/* Define debug types */
#ifdef LWIP_DEBUG
#define U8_F 					"c"
#define S8_F 					"c"
#define X8_F 					"x"
#define U16_F 					"u"
#define S16_F 					"d"
#define X16_F 					"x"
#define U32_F 					"u"
#define S32_F 					"d"
#define X32_F 					"x"
#endif

/* Comment this define for enable fatal print messages and abandon execution */
#define LWIP_NOASSERT

/* Use apropriate debug level: unrem level that you prefere */
#define LWIP_DBG_MIN_LEVEL		LWIP_DBG_LEVEL_OFF
//#define LWIP_DBG_MIN_LEVEL		LWIP_DBG_LEVEL_WARNING
//#define LWIP_DBG_MIN_LEVEL		LWIP_DBG_LEVEL_SERIOUS
//#define LWIP_DBG_MIN_LEVEL		LWIP_DBG_LEVEL_SEVERE

/* Set flags for LWIP_DEBUGF to enable that debug messages */
/* LWIP_DBG_HALT - will be stop execution after print */
#define LWIP_DBG_TYPES_ON		LWIP_DBG_OFF //(LWIP_DBG_ON | LWIP_DBG_TRACE | LWIP_DBG_STATE | LWIP_DBG_FRESH)

/* Define debug for lwiP modules */
#define ETHARP_DEBUG		LWIP_DBG_OFF
#define NETIF_DEBUG			LWIP_DBG_OFF
#define PBUF_DEBUG			LWIP_DBG_OFF
#define API_LIB_DEBUG		LWIP_DBG_OFF
#define API_MSG_DEBUG		LWIP_DBG_OFF
#define SOCKETS_DEBUG		LWIP_DBG_OFF
#define ICMP_DEBUG			LWIP_DBG_OFF
#define INET_DEBUG			LWIP_DBG_OFF
#define IP_DEBUG			LWIP_DBG_OFF
#define IP_REASS_DEBUG		LWIP_DBG_OFF
#define RAW_DEBUG			LWIP_DBG_OFF
#define MEM_DEBUG			LWIP_DBG_OFF
#define MEMP_DEBUG			LWIP_DBG_OFF
#define SYS_DEBUG			LWIP_DBG_OFF
#define TCP_DEBUG			LWIP_DBG_OFF
#define TCP_INPUT_DEBUG		LWIP_DBG_OFF
#define TCP_FR_DEBUG		LWIP_DBG_OFF
#define TCP_RTO_DEBUG		LWIP_DBG_OFF
#define TCP_CWND_DEBUG		LWIP_DBG_OFF
#define TCP_WND_DEBUG		LWIP_DBG_OFF
#define TCP_OUTPUT_DEBUG	LWIP_DBG_OFF
#define TCP_RST_DEBUG		LWIP_DBG_OFF
#define TCP_QLEN_DEBUG		LWIP_DBG_OFF
#define UDP_DEBUG			LWIP_DBG_OFF
#define TCPIP_DEBUG			LWIP_DBG_OFF
#define PPP_DEBUG			LWIP_DBG_OFF
#define SLIP_DEBUG			LWIP_DBG_OFF
#define DHCP_DEBUG			LWIP_DBG_OFF


#endif /* __LWIPOPTS_H__ */
