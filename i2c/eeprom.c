#include "../main.h"
#include "../syscall/device.h"
#include "i2c0.h"

#ifdef ENABLE_EEPROM
#include "eeprom.h"

DEVICE_PTR ptr_i2c0 = 0;

int EEPROM_Open (void)
{
    int FileDescriptor, SCL_Freq = EEPROM_SPEED;
	unsigned char EEProm_Addres = EEPROM_ADDRESS;

	/* Search 'i2c0' device in the File Descriptor Table */
    FileDescriptor = _open_r ("i2c0", 0, 0);
    /* If 'i2c0' device found */
    if (FileDescriptor > 0) {
    	/* Get pointer to the DEVICE structure */
        ptr_i2c0 = Device_List[FileDescriptor];
        /* Init i2c0 */
        ptr_i2c0->init ();
        /* Set i2c0 SCL clock frequency */
        ptr_i2c0->ioctl (I2C_SET_SPEED, &SCL_Freq);
        /* Set EEPROM Slave Address */
        ptr_i2c0->ioctl (I2C_SET_ADDRESS, &EEProm_Addres);
    }
    return FileDescriptor;
}

int EEPROM_Write (EEPROM_ADDRESS_TYPE Address, unsigned char *buf, unsigned int Amount)
{
	unsigned char EEProm_Addres = EEPROM_ADDRESS;
	unsigned int to_write, writen = 0;

	while (writen != Amount) {
		/* Send Start to the I2C Bus */
		if (ptr_i2c0->ioctl (I2C_SEND_START, 0) != 1)
			return -1;

		/* Write Slave Address */
		if (ptr_i2c0->write (&EEProm_Addres, 1) != 1)
			return -1;

		/* Write internal Address */
		if (ptr_i2c0->write ((unsigned char *)&Address, sizeof(EEPROM_ADDRESS_TYPE)) != sizeof(EEPROM_ADDRESS_TYPE))
			return -1;

		/* Calculate amount the we may write within current page */
		to_write = EEPROM_PAGE_WRITE_SIZE - (unsigned char) (Address & (EEPROM_PAGE_WRITE_SIZE -1));
		if (to_write > (Amount - writen))
			to_write = (Amount - writen);
		/* Write Buffer to the EEPROM */
		if (ptr_i2c0->write (buf, to_write) != to_write)
			return -1;
		/* Increase internal address and amount of writen bytes */
		Address +=	to_write;
		writen += to_write;
		buf += to_write;
		/* Send Stop to the I2C Bus */
		ptr_i2c0->ioctl (I2C_SEND_STOP, 0);
		/* Wait for data is writen from page buffer to the EEPROM */
		while (ptr_i2c0->ioctl (I2C_POLL_ADDRESS, &EEProm_Addres) != 1);
	}
	return (writen > 0) ? writen: -1;
}

int EEPROM_Read (EEPROM_ADDRESS_TYPE Address, unsigned char *buf, unsigned int Amount)
{
	unsigned int to_read, readed = 0;

	while (Amount) {
		/* Calculate the amount of data that we can read within the page */
		to_read = EEPROM_PAGE_READ_SIZE - (unsigned char) Address;
		if (to_read > Amount)
			to_read = Amount;
		/* Read data portion from EEPROM */
		if (ptr_i2c0->read (buf, to_read) != to_read)
			return -1;
		/* Increase internal address */
		Address += to_read;
		/* Decrease amount of data that we must read */
		Amount -= to_read;
		/* Move the buffer pointer and increase amount of readed data */
		buf += to_read;
		readed += to_read;
	}
	return (readed > 0) ? readed: -1;
}

int EEPROM_Close (void)
{
	/* Close i2c0 bus */
	ptr_i2c0->close ();
	/* Reset pointer to the i2c0 DEVICE structure */
	ptr_i2c0 = 0;

	return 1;
}
#endif
