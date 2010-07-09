#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <unistd.h>

/* Device driver information */
typedef const struct DEVICE {
	/* Device flags: input/output ... */
	int	flags;
	/* Device name */
	char name[8];
	/* OPEN device fuction */
	unsigned (*open) (void);
	/* CLOSE device function */
	unsigned (*close) (void);
	/* OUTPUT function */
	signed (*write) (unsigned char *ptr, unsigned int len);
	/* INPUT function */
	signed (*read) (unsigned char *ptr, unsigned int len);
	/* INIT device function */
	unsigned (*init) (void);
	/* Device IO control function */
	signed (*ioctl) (unsigned int cmd, void *ptr);
}DEVICE;

typedef DEVICE	*DEVICE_PTR;

#define DEV_FLAGS_INPUT	 0x00000001	/* Device can be used as input console */
#define DEV_FLAGS_OUTPUT 0x00000002	/* Device can be used as output console */

/* Define indexes in the global Device_List structure for the LPC214x peripheral */

/* UART File Descriptor values */
//#define	FD_UART0			(3)			/* Uart0 File Descriptor */
//#define	FD_UART1			(4)			/* Uart1 File Descriptor */
/* UART File Descriptor values */
//#define FD_SPI1             (5)         /* SPI1 File Descriptor */

extern DEVICE_PTR	Device_List[];

size_t Device_List_Count (void);
int console_assign (int fd, char *devname);

#endif
