#ifndef _LWIP_LIB_H_
#define _LWIP_LIB_H_

/* Include lwIP TCP/IP stack Options */
#include "lwip/opt.h"

/* Include lwIP stack API Headers */
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/tcpip.h"
#include "lwip/api.h"
#include "lwip/netifapi.h"
#include "lwip/sockets.h"
#include "lwip/mem.h"

/* Include our ethernet interface */
#include "netif/ethernetif.h"

/* lwIP network interface structure used for our port */
extern struct netif enc28j60_netif;

void lwip_process_timers (void);
void init_lwip_stack (unsigned int IPaddress, unsigned int NETmask, unsigned int GATEaddress, unsigned char EnDHCP, unsigned char EnAutoIP);

#endif
