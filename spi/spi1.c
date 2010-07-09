#include "../main.h"

#ifdef ENABLE_SPI1
#include "spi1.h"

unsigned SPI1_Init (void)
{
        unsigned char volatile Dummy;

		/* Enable Power for SSP */
		PCONP |= PCSPI1;
        /* Set P0.4 as SPI0 SCK Line, P0.5 as SPI0 MISO Line, P0.6 as SPI0 MOSI Line*/
        PINSEL1 = (PINSEL1 & ~(PINSEL1_P017_MASK | PINSEL1_P018_MASK | PINSEL1_P019_MASK)) | (PINSEL1_P017_SCK1 | PINSEL1_P018_MISO1 | PINSEL1_P019_MOSI1);
		PINSEL1 = (PINSEL1 & ~PINSEL1_P020_MASK) | PINSEL1_P020_GPIO;
        /* Set P0.7 as GPIO  SPI SSEL Line not used  set to '1' - inactive state*/
        GPIO0_FIOMASK &= ~GPIO_IO_P20;
        GPIO0_FIOSET = GPIO_IO_P20;
        GPIO0_FIODIR |= GPIO_IO_P20;
        /* Set SPI prescaler to devide Fpclk to 2 */
        SSP_CPSR = 2;
        /* 8-bit data transfer, SPI Frame format, SCK Active Low CPOL=0, Data sampled on the first clock edge CPHA = 0 */
        /* SCR = 2 Set SCK frequency to 10Mhz SCK=Fpclk/CPSDVSR/(SCR+1) */
        SSP_CR0 = SSP_CR0_DSS_8 | SSP_CR0_FRF_SPI | (2 << 8);
		/* Disable LoopBack mode LBM=0, The SSP controller acts as a master on the bus MS=0*/
		SSP_CR1 = 0;
        /* Clear SSP RxFIFO */
        while (SSP_SR & SSP_SR_RNE)
                Dummy = SSP_DR;
        /* Enable SSP controller SSE = 1*/
        SSP_CR1 |= SSP_CR1_SSE;
		//printf ("SPI1 Initialized...\r\n");
        return 1;
}

int SPI1_Flush (unsigned int Amount)
{
        unsigned int volatile Dummy;
		unsigned int ToFlush = Amount;

        /* Wait until all data is transmited from SSP FIFO BSY->'0' */
        while (SSP_SR & SSP_SR_BSY);
        /* Check if we flush all requested data or RXFIFO is not full*/
		//printf ("Flush %d bytes\r\n", Amount);
        while ((Amount != 0) && (SSP_SR & SSP_SR_RNE)) {
                Dummy = SSP_DR;
                Amount --;
        }
        //printf ("%d bytes flushed \r\n", Amount);
        return (signed) (ToFlush - Amount);
}

signed SPI1_Write (unsigned char *buffer, unsigned int Amount)
{
        signed Writen = 0;
//		unsigned int Temp;
		//printf ("Write %d bytes\r\n", Amount);
        while (Amount--) {

				//printf ("Check if TX FIFO is full\r\n");
				//Temp = SSP_SR;
				//printf ("SSP_SR=0x%x\r\n", Temp);
				//Temp = SSP_CR0;
				//printf ("SSP_CR0=0x%x\r\n", Temp);
				//Temp = SSP_CR1;
				//printf ("SSP_CR1=0x%x\r\n", Temp);
				//Temp = SSP_CPSR;
				//printf ("SSP_CPSR=0x%x\r\n", Temp);
				//Temp = PINSEL1;
				//printf ("PINSEL1=0x%x\r\n", Temp);
				//Temp = GPIO0_FIOPIN;
				//printf ("FIO0PIN=0x%x\r\n", Temp);

                /* Stay in loop if SSP TXFIFO Full (TNF=0) */
                while (!(SSP_SR & SSP_SR_TNF));
                //printf ("Write data to the TX FIFO\r\n");
                /* Write charcter to the TX queue */
                SSP_DR = *buffer++;
				Writen++;
        }
		//printf ("%d bytes writen\r\n", Writen);
        return (Writen > 0) ? Writen : -1;
}

signed SPI1_Read (unsigned char *buffer, unsigned int Amount)
{
        unsigned int MustToWrite, Readed;

        Readed = Amount;
        while (Amount) {
                /* Wait until all data is transmited from SSP FIFO */
                while (SSP_SR & SSP_SR_BSY);
                /* While RXFIFO not empty and we don't read all requested amount of data */
                while ((SSP_SR & SSP_SR_RNE) && (Amount != 0)) {
                        *buffer++ = SSP_DR;
                        Amount--;
                }
                /* If we must read more data */
                if (Amount) {
                        /* Calculate amount thet we must read in current frame (SSP FIFO depth is 8 bytes)*/
                        if (Amount > SSP_FIFO_DEPTH)
                                MustToWrite = SSP_FIFO_DEPTH;
                        else
                                MustToWrite = Amount;
                        while (MustToWrite--)
                                /* Send a dummy byte to receive a byte */
                                SSP_DR = 0;
                }
        }

        return (signed) Readed;
}

signed SPI1_DeviceIOControl (unsigned int Command, void *ptr_buff)
{
	unsigned int *pAmount;
	int Flushed;

        switch (Command) {
        case SET_ETHERNET_CS:
                GPIO0_FIOCLR |= GPIO_IO_P20;
                return 1;
        case CLR_ETHERNET_CS:
                GPIO0_FIOSET |= GPIO_IO_P20;
                return 1;
        case FLUSH_RX_BUFFER:
				if (ptr_buff != 0x0) {
					pAmount = (unsigned int *) ptr_buff;
					Flushed = SPI1_Flush (*pAmount);
					return Flushed;
				}
				else return -1;
         default:
				return -1;
        }
}

unsigned SPI1_Open (void)
{
        SPI1_Flush (8);
		return 0;
}

unsigned SPI1_Close (void)
{
	SPI1_DeviceIOControl (CLR_ETHERNET_CS, 0x0);
	return 0;
}
#endif
