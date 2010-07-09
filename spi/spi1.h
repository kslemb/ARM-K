#ifndef _SPI1_H_
#define _SPI1_H_

/* Define command numbers used in DeviceIOControl */
#define SET_ETHERNET_CS     1   /* Set CS PIN to the active '0' state */
#define CLR_ETHERNET_CS     2   /* Set CS PIN to the inactive '1' state */
#define FLUSH_RX_BUFFER     3   /* Clear RX buffer for requested amount */

unsigned SPI1_Init (void);
signed SPI1_Write (unsigned char *buffer, unsigned int Amount);
signed SPI1_Read (unsigned char *buffer, unsigned int Amount);
signed SPI1_DeviceIOControl (unsigned int Command, void *ptr_buff);
unsigned SPI1_Open (void);
unsigned SPI1_Close (void);

#endif
