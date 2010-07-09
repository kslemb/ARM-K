#include "../main.h"

#ifdef ENABLE_I2C0
#include "i2c0.h"

/* Slave Address of the external device on the I2C Bus */
unsigned char I2C0_Slave_Address;

unsigned int I2C0_Get_Status (void)
{
	/* Wait for I2C interrupt */
	while (!(I2C0_CONSET & I2C_CONSET_SI));
	/* clear SI (I2C interrupt) flag SIC = 1 */
	I2C0_CONCLR = I2C_CONCLR_SIC;
	return I2C0_STAT;
}

signed I2C0_DeviceIOControl (unsigned int Command, void *ptr_buff)
{
	unsigned int Temp;

	switch (Command) {

        case I2C_SET_SPEED:
			Temp = *((unsigned int *) ptr_buff);
			/* Set the low time of the SCL clock */
			I2C0_SCLL = CPU_CLOCK_HZ/Temp/2 + 1;
			/* Set the high time of the SCL clock */
			I2C0_SCLH = CPU_CLOCK_HZ/Temp/2;
			return 1;

		case I2C_SET_ADDRESS:
			/* Save Slave addres of the device that we read/write */
			I2C0_Slave_Address = *((unsigned char *)ptr_buff);
			return 1;

		case I2C_GET_STATUS:
			if (ptr_buff != 0)
				*((unsigned int *)ptr_buff) = I2C0_Get_Status ();
			return 1;

		case I2C_SEND_START:
			/* Send START to the I2C BUS */
			I2C0_CONSET = I2C_CONSET_STA;
			/* Wait for Start condition is output to the bus */
			Temp = I2C0_Get_Status ();
			/* Start or Repeated Start transmitted or no relevant information */
			if ((Temp == 0x08) || (Temp == 0x10) || (Temp == 0xF8))
				Temp = 1;
			else
				Temp = -1;
			/* Clear start flag STAC = 1 */
			I2C0_CONCLR = I2C_CONCLR_STAC;
			return Temp;

		case I2C_SEND_STOP:
			/* Output Stop condition to the I2C bus STO = 1 */
			I2C0_CONSET = I2C_CONSET_STO;
			/* wait for STOP flag clear */
			while(I2C0_CONSET & I2C_CONSET_STO);
			/* clear SI (I2C interrupt) flag SIC = 1 */
			I2C0_CONCLR = I2C_CONCLR_SIC;
			return 1;

		case I2C_POLL_ADDRESS:
			/* Send START to the I2C BUS */
			I2C0_CONSET = I2C_CONSET_STA;
			/* Wait for Start condition is output to the bus */
			Temp = I2C0_Get_Status ();
			/* If the next criterion is not met: Start or Repeated Start transmitted or no relevant information */
			if ((Temp == 0x08) || (Temp == 0x10) || (Temp == 0xF8))
				return -1;
			I2C0_DAT = 	*((unsigned char *) ptr_buff) & 0xFE;
			Temp = I2C0_Get_Status ();
			/* If status not a: SLA+W transmitted and ACK received */
			if (Temp != 0x18)
				Temp = -1;
			else
				Temp = 1;
			/* Output Stop condition to the I2C bus STO = 1 */
			I2C0_CONSET = I2C_CONSET_STO;
			/* wait for STOP flag clear */
			while(I2C0_CONSET & I2C_CONSET_STO);
			/* clear SI (I2C interrupt) flag SIC = 1 */
			I2C0_CONCLR = I2C_CONCLR_SIC;
			return Temp;

         default:
			return -1;
	}
}

signed I2C0_Write (unsigned char *buffer, unsigned int Amount)
{
	unsigned int Status, Writen = 0;

	while (Amount--) {
		I2C0_DAT = *buffer++;
		Status = I2C0_Get_Status ();
		/* If status not a: SLA+W transmitted, ACK received or  data byte transmitted, ACK received */
	    if ((Status != 0x18) || (Status != 0x28)) {
    		/* Generate Stop condition */
			I2C0_DeviceIOControl (I2C_SEND_STOP, 0);
			return -1;
	    }
	    Writen++;
	}
	return (Writen > 0) ? Writen : -1;
}

signed I2C0_Read (unsigned char *buffer, unsigned int Amount)
{
	unsigned char SlaveAddress = I2C0_Slave_Address | 1;
	unsigned int Status, Readen = 0;

	/* Send Start or Repeated Start */
	if (I2C0_DeviceIOControl (I2C_SEND_START, 0) != 1) {
		return -1;
	}
	/* Write I2C Slave address with R bit equal to '1' */
	I2C0_DAT = SlaveAddress;
	/* Get Status of the Slave Address write */
	Status = I2C0_Get_Status ();
	/* SLA+R transmitted and ACK received */
    if (Status == 0x40)
		while (Amount--) {
			/* Generate ACK for next received byte */
			if (Amount != 1) {
				/* Set ACK -> '0' (informs slave to send next byte) */
				I2C0_CONSET = I2C_CONSET_AA;
			}
			/* Generate NACK for next received byte - last byte in read sequence */
			else {
				/* Set ACK -> '1' (informs slave to end transfer) */
				I2C0_CONCLR = I2C_CONCLR_AAC;
			}
			/* Wait for data arrived */
			Status = I2C0_Get_Status ();
			/* If status not a: Data byte received, ACK transmitted or Data byte received, NACK transmitted */
			if ((Status != 0x50) || (Status != 0x58)) {
				Readen = 0;
				break;
			}
			/* Increment amount of data that we read */
			Readen++;
		}
	/* Generate Stop condition */
	I2C0_DeviceIOControl (I2C_SEND_STOP, 0);
	return (Readen > 0) ? Readen : -1;
}

unsigned I2C0_Init (void)
{
	unsigned int Speed;

	/* Enable Power for SSP */
	PCONP |= PCI2C0;
	/* Set GPIO Port 0.2 as SCL	and 0.3 as SDA */
	PINSEL0 = (PINSEL0 & ~(PINSEL0_P02_MASK | PINSEL0_P03_MASK)) | (PINSEL0_P02_SCL0 | PINSEL0_P03_SDA0);
	/* We work only in master mode - reset slave Addr register */
	I2C0_ADR = 0x00;
	/* Set 400 kHz SCL clock as default */
	Speed = 400000;
	I2C0_DeviceIOControl (I2C_SET_SPEED, &Speed);

	/* Clear all bits AAC-Assert Acknowledge Clear bit, SIC-I2C Interrupt Clear Bit, */
	/* STAC - Start flag clear bit, I2ENC - I2C interface disable.                   */
	I2C0_CONCLR = I2C_CONCLR_MASK;
	/*  Enable I2C0 Bus in Master Mode */
	I2C0_CONSET = I2C_CONSET_I2EN;
	return 1;
}

unsigned I2C0_Open (void)
{
	return 0;
}

unsigned I2C0_Close (void)
{
	return 0;
}
#endif
