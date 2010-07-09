#ifndef _I2C0_H
#define _I2C0_H

/* Define command numbers used in DeviceIOControl function */
#define I2C_SET_SPEED		0
#define I2C_SET_ADDRESS		1
#define I2C_GET_STATUS		2
#define I2C_SEND_START		3
#define I2C_SEND_STOP		4
#define I2C_POLL_ADDRESS	5

unsigned I2C0_Open (void);
unsigned I2C0_Close (void);
unsigned I2C0_Init (void);
signed I2C0_Read (unsigned char *buffer, unsigned int Amount);
signed I2C0_Write (unsigned char *buffer, unsigned int Amount);
signed I2C0_DeviceIOControl (unsigned int Command, void *ptr_buff);

#endif
