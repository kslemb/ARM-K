#include "../main.h"
#include "../timer0/timer0.h"
#include "lwip_lib.h"


/* lwIP network interface structure used for our port */
struct netif enc28j60_netif;


/*-------------------------------------*/
/*      Include lwIP core code         */
/*-------------------------------------*/
/* Include "Modules initialization" */
#include "../lwip/src/core/init.c"

/* Include "Dynamic memory manager" */
#include "../lwip/src/core/mem.c"

/* Include "Dynamic pool memory manager" */
#include "../lwip/src/core/memp.c"

/* Include "Packet buffer management" */
#include "../lwip/src/core/pbuf.c"

#if (NO_SYS == 0)
/* Include "lwIP Operating System abstraction if OS used " */
#include "../lwip/src/core/sys.c"
#endif

/* Include "lwIP network interface abstraction" */
#include "../lwip/src/core/netif.c"

#if LWIP_RAW
/* Include "Implementation of raw protocol PCBs */
#include "../lwip/src/core/raw.c"
#endif

#if LWIP_STATS
/* Include "Statistics module" */
#include "../lwip/src/core/stats.c"
#endif

#if LWIP_TCP
/* Include "Transmission Control Protocol for IP" */
#include "../lwip/src/core/tcp.c"

/* Include "Transmission Control Protocol, incoming traffic" */
#include "../lwip/src/core/tcp_in.c"

/* Include "Transmission Control Protocol, outgoing traffic" */
#include "../lwip/src/core/tcp_out.c"

#endif

#if LWIP_UDP
/* Include "User Datagram Protocol module" */
#include "../lwip/src/core/udp.c"
#endif

#if LWIP_DHCP
/* Include "Dynamic Host Configuration Protocol client" */
#include "../lwip/src/core/dhcp.c"
#endif


#if LWIP_DNS
/* Include "DNS - host name to IP address resolver" */
#include "../lwip/src/core/dns.c"
#endif


/*-------------------------------------*/
/*      Include lwIP IPv4 code         */
/*-------------------------------------*/
/* Include "Functions common to all TCP/IPv4 modules" */
#include "../lwip/src/core/ipv4/inet.c"

/* Include "Internet checksum functions" */
#include "../lwip/src/core/ipv4/inet_chksum.c"

/* Include "IPv4 layer implementation for incoming and outgoing IP traffic" */
#include "../lwip/src/core/ipv4/ip.c"

/* Include "IPv4 address tools implementation" */
#include "../lwip/src/core/ipv4/ip_addr.c"

/* Include "IPv4 packet segmentation and reassembly implementation" */
#include "../lwip/src/core/ipv4/ip_frag.c"

#if LWIP_AUTOIP
/* Include "AutoIP Automatic LinkLocal IP Configuration" */
#include "../lwip/src/core/ipv4/autoip.c"
#endif

#if LWIP_ICMP
/* Include "ICMP - Internet Control Message Protocol" */
#include "../lwip/src/core/ipv4/icmp.c"
#endif

#if LWIP_IGMP
/* Include "IGMP - Internet Group Management Protocol" */
#include "../lwip/src/core/ipv4/igmp.c"
#endif


/*-------------------------------------*/
/*      Include lwIP SNMP code         */
/*-------------------------------------*/
#if LWIP_SNMP
/* Include "MIB tree access/construction functions" */
#include "../lwip/src/core/snmp/mib_structs.c"

/* Include "Management Information Base II (RFC1213) objects and functions" */
#include "../lwip/src/core/snmp/mib2.c"

/* Include "SNMP input message processing (RFC1157)" */
#include "../lwip/src/core/snmp/msg_in.c"

/* Include "SNMP output message processing (RFC1157)" */
#include "../lwip/src/core/snmp/msg_out.c"

/* Include "Abstract Syntax Notation One (ISO 8824, 8825) decoding" */
#include "../lwip/src/core/snmp/asn1_dec.c"

/* Include "Abstract Syntax Notation One (ISO 8824, 8825) encoding" */
#include "../lwip/src/core/snmp/asn1_enc.c"

#endif


/*-------------------------------------*/
/*      Include lwIP API code          */
/*-------------------------------------*/
/* Include "Sequential API Main thread module" */
#include "../lwip/src/api/tcpip.c"

#if LWIP_NETCONN
/* Include "Sequential API External module" */
#include "../lwip/src/api/api_lib.c"

/* Include "Sequential API Internal module " */
#include "../lwip/src/api/api_msg.c"

/* Include "Network buffer management" */
#include "../lwip/src/api/netbuf.c"
#endif

/* Include "Network Interface Sequential API module" */
#if LWIP_NETIF_API
#include "../lwip/src/api/netifapi.c"
#endif

/* Include "Error Management module" */
#include "../lwip/src/api/err.c"

#if LWIP_DNS && LWIP_SOCKET
/* Include "API functions for name resolving" */
#include "../lwip/src/api/netdb.c"
#endif

#if LWIP_SOCKET
/* Include "Sockets BSD-Like API module" */
#include "../lwip/src/api/sockets.c"
#endif


/*-------------------------------------*/
/* Include lwIP Network Interface code */
/*-------------------------------------*/
/* Include "Address Resolution Protocol module for IP over Ethernet" */
#include "../lwip/src/netif/etharp.c"

/* Include "Loop Interface" */
#if LWIP_HAVE_LOOPIF
#include "../lwip/src/netif/loopif.c"
#endif

/*-------------------------------------*/
/*   Include our port Interface code   */
/*-------------------------------------*/
#include "ports/lpc_enc28j60/sys_arch.c"
#include "ports/lpc_enc28j60/netif/ethernetif.c"


/* Process all lwip timer events. This function must be called every 10 ms.              */
/* This function is similar to the 'tcpip_thread' function defined in the tcpip.c module */
void lwip_process_timers (void)
{
	unsigned int CurrentTickCnt;
#if LWIP_TCP
	static unsigned int lwip_tcp_timer = 0;
#endif

#if LWIP_ARP
	static unsigned int lwip_arp_timer = 0;
#endif
	CurrentTickCnt = TickGet ();

#if LWIP_DHCP
	static unsigned int lwip_DHCP_fine_timer = 0;
	static unsigned int lwip_DHCP_coarce_timer = 0;
#endif

#if LWIP_AUTOIP
	static unsigned int lwip_autoip_timer = 0;
#endif

#if LWIP_ARP
	/* Process the ARP timer */
	if(TickGetDiff (CurrentTickCnt, lwip_arp_timer) >= TICKS_IN_MS(ARP_TMR_INTERVAL)) {
		lwip_arp_timer = CurrentTickCnt;
		etharp_tmr();
	}
#endif

#if LWIP_TCP
	/* Process the TCP timer */
	if (TickGetDiff (CurrentTickCnt, lwip_tcp_timer) >= TICKS_IN_MS(TCP_TMR_INTERVAL)) {
        	lwip_tcp_timer = CurrentTickCnt;
		/* Increment fast (incremented every 250ms) and slow (incremented every 500ms) tcp timers */
		tcp_tmr ();
	}
#endif

#if LWIP_DHCP
	/* Process the DHCP Coarce timer */
	if (TickGetDiff (CurrentTickCnt, lwip_DHCP_coarce_timer) >= TICKS_IN_MS(DHCP_COARSE_TIMER_MSECS)) {
	        lwip_DHCP_coarce_timer = CurrentTickCnt;
		dhcp_coarse_tmr ();
	}

	/* Process the DHCP Fine timer */
	if (TickGetDiff (CurrentTickCnt, lwip_DHCP_fine_timer) >= TICKS_IN_MS(DHCP_FINE_TIMER_MSECS)) {
		lwip_DHCP_fine_timer = CurrentTickCnt;
		dhcp_fine_tmr ();
	}
#endif

#if LWIP_AUTOIP
	/* Process the DHCP Fine timer */
	if (TickGetDiff (CurrentTickCnt, lwip_autoip_timer) >= TICKS_IN_MS(AUTOIP_TMR_INTERVAL)) {
		lwip_autoip_timer = CurrentTickCnt;
		autoip_tmr ();
	}
#endif
}

/* Init lwIP TCP/IP stack */
void init_lwip_stack (unsigned int IPaddress, unsigned int NETmask, unsigned int GATEaddress, unsigned char EnDHCP, unsigned char EnAutoIP)
{
	struct ip_addr ip_addr, net_mask, gate_addr;

	/* Initialize all lwIP modules */
	lwip_init ();

	/* Setup static IP values */
	ip_addr.addr	= htonl(IPaddress);
	net_mask.addr	= htonl(NETmask);
	gate_addr.addr	= htonl(GATEaddress);

#if (LWIP_AUTOIP || LWIP_DHCP)
	/* If DHCP or AutoIP is enabled clear IP values */
	if (EnDHCP || EnAutoIP) {
		ip_addr.addr	= 0;
		net_mask.addr	= 0;
		gate_addr.addr	= 0;
	}
#endif

	/* Add a network interface to the list of lwIP netifs */
 	/* use ethernet_input() insted of tcpip_input() */
	netif_add(&enc28j60_netif, &ip_addr, &net_mask, &gate_addr, NULL, ethernetif_init, ip_input);

	/* Set a 'enc28j60_netif' interface as the default network interface */
	netif_set_default(&enc28j60_netif);

#if LWIP_DHCP
	/* Start DHCP if it enabled */
	if (EnDHCP)
		dhcp_start (&enc28j60_netif);
#endif

#if LWIP_AUTOIP
	/* Start AutoIP if it enabled */
	if (EnAutoIP)
		autoip_start (&enc28j60_netif);
#endif

	/* Bring an interface up, available for processing traffic */
	netif_set_up (&enc28j60_netif);
}
