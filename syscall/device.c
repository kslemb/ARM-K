#include "../main.h"
#include "../uart/uart.h"
#include "../spi/spi1.h"
#include "../i2c/i2c0.h"
#include "device.h"


/* Define console stdin device structure */
DEVICE stdin = {
	DEV_FLAGS_INPUT,
	"stdin",
	0,
	0,
	0,
	0,
	0,
	0};

/* Define console stdout device structure */
DEVICE stdout = {
	DEV_FLAGS_OUTPUT,
	"stdout",
	0,
	0,
	0,
	0,
	0,
	0};

/* Define console stderr device structure */
DEVICE stderr = {
	DEV_FLAGS_OUTPUT,
	"stderr",
	0,
	0,
	0,
	0,
	0,
	0};

#ifdef ENABLE_UART0
DEVICE Uart0 = {
	(DEV_FLAGS_INPUT + DEV_FLAGS_OUTPUT),
	"uart0",
	Uart0_Open,
	Uart0_Close,
	Uart0_Write,
	Uart0_Read,
	Uart0_Init,
	/* Device IO control function */
	0};
#endif

#ifdef ENABLE_SPI1
DEVICE SPI1 = {
    (DEV_FLAGS_INPUT + DEV_FLAGS_OUTPUT),
    "spi1",
    SPI1_Open,
    SPI1_Close,
    SPI1_Write,
    SPI1_Read,
    SPI1_Init,
    SPI1_DeviceIOControl};
#endif

#ifdef ENABLE_I2C0
DEVICE I2C0 = {
	(DEV_FLAGS_INPUT + DEV_FLAGS_OUTPUT),
    "i2c0",
    I2C0_Open,
    I2C0_Close,
    I2C0_Write,
    I2C0_Read,
    I2C0_Init,
    I2C0_DeviceIOControl};
#endif

DEVICE_PTR	Device_List[] = {
	&stdin,
	&stdout,
	&stderr,
#ifdef ENABLE_UART0
	&Uart0,
#endif
#ifdef ENABLE_UART1
	&Uart1,
#endif
#ifdef ENABLE_SPI1
    &SPI1,
#endif
#ifdef ENABLE_I2C0
	&I2C0,
#endif
	0};

size_t Device_List_Count (void)
{
	return sizeof (Device_List) / sizeof (Device_List[0]);
}

int console_setfile (int fd, DEVICE_PTR device_ptr)
{
		switch (fd) {
			case STDIN_FILENO:
				Device_List[STDIN_FILENO] = device_ptr;
				break;
			case STDOUT_FILENO:
				Device_List[STDOUT_FILENO] = device_ptr;
				break;
			case STDERR_FILENO:
				Device_List[STDERR_FILENO] = device_ptr;
				break;
			default:
				return -1;
		}
	return 1;
}

int console_assign (int fd, char *devname)
{
	int i;
	DEVICE_PTR dev_ptr;

	for (i = 0; i < Device_List_Count (); i++) {
		dev_ptr = Device_List[i];
		if(dev_ptr == 0) {
			return -1;
		}
		if (__builtin_strcmp(devname, dev_ptr->name) == 0) {
			console_setfile (fd, dev_ptr);
			return 1;
		}
	}
 return 1;
}
