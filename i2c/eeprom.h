#ifndef _EEPROM_H

#define _EEPROM_H

/* Define I2C EEPROM Slave Address */
#define EEPROM_ADDRESS			0xA0

/* Define I2C EEPROM SCL Frequency */
#define EEPROM_SPEED			400000

/* Define the type of the EEPROM internal address */
#define EEPROM_ADDRESS_TYPE	unsigned short

/* Define the EEPROM page read size */
#define EEPROM_PAGE_READ_SIZE	255

/* Define the EEPROM page write size */
#define EEPROM_PAGE_WRITE_SIZE	128

int EEPROM_Open (void);
int EEPROM_Write (EEPROM_ADDRESS_TYPE Address, unsigned char *buf, unsigned int Amount);
int EEPROM_Read (EEPROM_ADDRESS_TYPE Address, unsigned char *buf, unsigned int Amount);
int EEPROM_Close (void);
#endif
