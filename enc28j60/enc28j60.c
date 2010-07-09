#include "../main.h"
#include "../syscall/device.h"
#include "../spi/spi1.h"
#include "enc28j60.h"

#include "../generic/console.h"

DEVICE_PTR ptr_spi = 0;

/* Adsress of the next received packet in the internal ENC28J60 buffer */
unsigned int next_packet_addr;

int enc28j60_open (void)
{
    int FileDescriptor;

    FileDescriptor = _open_r ("spi1", 0, 0);
    if (FileDescriptor > 0) {
        ptr_spi = Device_List[FileDescriptor];
        ptr_spi->init ();
    }
    else
	 ;// printf ("Can't open spi1\n\r");
    return FileDescriptor;
}

void enc28j60_BitField_Clear (unsigned int Address, unsigned char BitField)
{
    unsigned char Data[2];
    unsigned int AmountToFlush = 2;

    /* Add BFC opcode to the address */
    Data[0] = BFC | (unsigned char) Address;
    /* Put Bitfield value to the transmitted buffer */
    Data[1] = BitField;
    /* Select the chip and send the Read Control Register opcode/address */
    ptr_spi->ioctl (SET_ETHERNET_CS, 0);
    /* Send BFC opcode with address and BitField */
    ptr_spi->write (Data, 2);
    /* Remove two dummy bytes of data from RXFIFO */
    ptr_spi->ioctl (FLUSH_RX_BUFFER, (void *) &AmountToFlush);
    /* Unselect the chip */
    ptr_spi->ioctl (CLR_ETHERNET_CS, 0);
}


void enc28j60_BitField_Set (unsigned int Address, unsigned char BitField)
{
    unsigned char Data[2];
    unsigned int AmountToFlush = 2;

    /* Add BFC opcode to the address */
    Data[0] = BFS | (unsigned char) Address;
    /* Put Bitfield value to the transmitted buffer */
    Data[1] = BitField;
    /* Select the chip and send the Read Control Register opcode/address */
    ptr_spi->ioctl (SET_ETHERNET_CS, 0);
    /* Send BFC opcode with address and BitField */
    ptr_spi->write (Data, 2);
    /* Remove two dummy bytes of data from RXFIFO */
    ptr_spi->ioctl (FLUSH_RX_BUFFER, (void *) &AmountToFlush);
    /* Unselect the chip */
    ptr_spi->ioctl (CLR_ETHERNET_CS, 0);
}

void enc28j60_Bank_Select (unsigned int Reg)
{
    enc28j60_BitField_Clear (ECON1, ECON1_BSEL1 | ECON1_BSEL0);
    enc28j60_BitField_Set (ECON1, (unsigned char) (Reg >> 8));
}

unsigned char enc28j60_Read_ETH_Reg (unsigned int Address)
{
    unsigned char Data[2];

    /* Add RCR opcode to the address */
    Data[0] = RCR | (unsigned char) Address;
    /* Select the chip and send the Read Control Register opcode/address */
    ptr_spi->ioctl (SET_ETHERNET_CS, 0);
    /* Send RCR opcode with address */
    ptr_spi->write (Data, 1);
    /* Read received data */
    ptr_spi->read (Data, 2);
    /* Unselect the chip */
    ptr_spi->ioctl (CLR_ETHERNET_CS, 0);

    return (Data[1]);
}

unsigned char enc28j60_Read_MAC_Reg (unsigned int Address)
{
    unsigned char Data[3];

    /* Add RCR opcode to the address */
    Data[0] = RCR | (unsigned char) Address;
    /* Select the chip and send the Read Control Register opcode/address */
    ptr_spi->ioctl (SET_ETHERNET_CS, 0);
    /* Send RCR opcode with address */
    ptr_spi->write (Data, 1);
    /* Read received data */
    ptr_spi->read (Data, 3);
    /* Unselect the chip */
    ptr_spi->ioctl (CLR_ETHERNET_CS, 0);

    return (Data[2]);
}

void enc28j60_Write_ETH_MAC_Reg (unsigned int Address, unsigned char Value)
 {
    unsigned char Data[3];
    unsigned int AmountToFlush = 2;

    /* Add WCR opcode to the address */
    Data[0] = WCR | (unsigned char) Address;
    /* Put Value to the transmitted buffer */
    Data[1] = Value;
    /* Select the chip and send the Read Control Register opcode/address */
    ptr_spi->ioctl (SET_ETHERNET_CS, 0);
    /* Send WCR opcode with address and data that we must write*/
    ptr_spi->write (Data, 2);
    /* Remove two dummy bytes of data from RXFIFO */
    ptr_spi->ioctl (FLUSH_RX_BUFFER, (void *) &AmountToFlush);
    /* Unselect the chip */
    ptr_spi->ioctl (CLR_ETHERNET_CS, 0);
}

unsigned int enc28j60_Read_PHY_Reg (unsigned char Register)
{
    unsigned int Value;

    /* Set the right address and start the register read operation */
    enc28j60_Bank_Select (MIREGADR);
    enc28j60_Write_ETH_MAC_Reg (MIREGADR, Register);
    enc28j60_Write_ETH_MAC_Reg (MICMD, MICMD_MIIRD);
    /* Loop to wait until the PHY register has been read through the MII */
    enc28j60_Bank_Select (MISTAT);
    /* This requires 10.24us */
    while (enc28j60_Read_MAC_Reg (MISTAT) & MISTAT_BUSY);
    /* Stop reading */
    enc28j60_Bank_Select (MIREGADR);
    enc28j60_Write_ETH_MAC_Reg (MICMD, 0x00);
    /* Obtain result */
    Value = enc28j60_Read_MAC_Reg (MIRDH);
    Value <<= 8;
    Value |= enc28j60_Read_MAC_Reg (MIRDL);

    return (Value);
}

void enc28j60_Write_PHY_Reg (unsigned char Register, unsigned int Value)
{
    /* Write the register address */
    enc28j60_Bank_Select (MIREGADR);
    enc28j60_Write_ETH_MAC_Reg (MIREGADR, Register);
    /* Writing data, Order is important: LSB first, MSB last */
    enc28j60_Write_ETH_MAC_Reg (MIWRL, LOW(Value));
    enc28j60_Write_ETH_MAC_Reg (MIWRH, HIGH(Value));
    /* Wait until the PHY register written */
    enc28j60_Bank_Select (MISTAT);
    while (enc28j60_Read_MAC_Reg (MISTAT) & MISTAT_BUSY);
}

signed enc28j60_Write_Buffer (unsigned char *pData, unsigned int Length)
{
	unsigned char Data;
	unsigned int AmountToFlush = SSP_FIFO_DEPTH;
	signed Writen = (signed) Length;

	 /* Put WBM opcode to the buffer */
	Data = WBM;
	/* Select the chip and send the Read Control Register opcode/address */
	ptr_spi->ioctl (SET_ETHERNET_CS, 0);
	/* Send WBM opcode */
	ptr_spi->write (&Data, 1);
	/* Send Data to the ENC28J60*/
	ptr_spi->write (pData, Length);
	/* Remove dummy bytes of data from RXFIFO (max = SSP_FIFO_DEPTH) */
	ptr_spi->ioctl (FLUSH_RX_BUFFER, (void *) &AmountToFlush);
	/* Unselect the chip */
	ptr_spi->ioctl (CLR_ETHERNET_CS, 0);

	return Writen;
}

signed enc28j60_Write_Zerro_Buffer (unsigned int Length)
{
	unsigned char Data = 0;
	unsigned int AmountToFlush = SSP_FIFO_DEPTH;
	signed Writen = (signed) Length;

	 /* Put WBM opcode to the buffer */
	Data = WBM;
	/* Select the chip and send the Read Control Register opcode/address */
	ptr_spi->ioctl (SET_ETHERNET_CS, 0);
	/* Send WBM opcode */
	ptr_spi->write (&Data, 1);
	/* Write zerro buffer to the ENC28J60*/
	while (Length--)
		ptr_spi->write (&Data, 1);
	/* Remove dummy bytes of data from RXFIFO (max = SSP_FIFO_DEPTH) */
	ptr_spi->ioctl (FLUSH_RX_BUFFER, (void *) &AmountToFlush);
	/* Unselect the chip */
	ptr_spi->ioctl (CLR_ETHERNET_CS, 0);

	return Writen;
}

void enc28j60_packet_send_prepare (void)
{
	unsigned char ControlByte = 0;

	/* Set write pointer to the start of transmit buffer */
	enc28j60_Write_ETH_MAC_Reg (EWRPTL, LOW(TXSTART));
	enc28j60_Write_ETH_MAC_Reg (EWRPTH, HIGH(TXSTART));
	/* Write Control Byte if it equal to '0' that means that we use MACON3 settings */
	enc28j60_Write_Buffer (&ControlByte, 1);
}

void enc28j60_packet_send (unsigned int TotalLength)
{
	unsigned int PadLength;

	/* !!!!!!!!! Experimental check later for if must use this !!!!!!!!! */
	if (TotalLength < 60) {
		PadLength = 60 - TotalLength;
		TotalLength = 60;
	}
	else
		PadLength = 0;

	/* Set ETXND pointer to the packet size that we transmitt */
	enc28j60_Write_ETH_MAC_Reg (ETXNDL, LOW(TXSTART + TotalLength));
	enc28j60_Write_ETH_MAC_Reg (ETXNDH, HIGH(TXSTART + TotalLength));

	if ( PadLength )
		enc28j60_Write_Zerro_Buffer (PadLength);

	/* Send filled packet into the network */
	enc28j60_BitField_Set (ECON1, ECON1_TXRTS);

	/* Rev. B4 Silicon Errata point 12. Reset the transmit logic problem. */
	if ( (enc28j60_Read_ETH_Reg (EIR) & EIR_TXERIF) )
		enc28j60_BitField_Clear (ECON1, ECON1_TXRTS);
}


signed enc28j60_Read_Buffer (unsigned char *pData, unsigned int Length)
{
	unsigned char Data;
	unsigned int AmountToFlush = 1;

	/* Put RBM opcode to the buffer */
	Data = RBM;
	/* Select the chip and send the Read Control Register opcode/address */
	ptr_spi->ioctl (SET_ETHERNET_CS, 0);
	/* Send WBM opcode and data that we must write*/
	ptr_spi->write (&Data, 1);
	/* Remove one dummy byte of data from RXFIFO */
	ptr_spi->ioctl (FLUSH_RX_BUFFER, (void *) &AmountToFlush);
	/* Read received data */
	ptr_spi->read (pData, Length);
	/* Unselect the chip */
	ptr_spi->ioctl (CLR_ETHERNET_CS, 0);

	return (signed) Length;
}

signed enc28j60_Check_If_Packet_Received (void)
{
	/* Select Bank 1 */
	enc28j60_Bank_Select (EPKTCNT);
	/* check if packet is accepted and completely written into the buffer */
	if( enc28j60_Read_ETH_Reg (EPKTCNT) == 0 ) {
		/* Select Bank 0 */
		enc28j60_Bank_Select (ERDPTL);
		return -1;
	}
	else {
		/* Select Bank 0 */
		enc28j60_Bank_Select (ERDPTL);
		return 1;
	}
}

unsigned enc28j60_packet_read_prepare (void)
{
	unsigned char PacketHead[6];
	unsigned int packet_length = 0;

	/* Set the read pointer to the start of the received packet */
	enc28j60_Write_ETH_MAC_Reg (ERDPTL, LOW(next_packet_addr));
	enc28j60_Write_ETH_MAC_Reg (ERDPTH, HIGH(next_packet_addr));

	/* Read six bytes "Head" of the received packet */
	enc28j60_Read_Buffer (PacketHead, 6);

	/* Save next packet address */
	next_packet_addr = 0;
	next_packet_addr = PacketHead[1];
        next_packet_addr <<= 8;
	next_packet_addr |= PacketHead[0];

	/* Save Packet length (including CRC and type of packet) */
	packet_length = PacketHead[3];
	packet_length <<= 8;
	packet_length |= PacketHead[2];

	/* Save status and check CRC/symbol errors
	   The ERXFCON.CRCEN is set by default after Reset.
	   Normally we should not need to check this.
	 */
	unsigned int packet_status;
	packet_status = 0;
	packet_status = PacketHead[5];
	packet_status <<= 8;
	packet_status |= PacketHead[4];
	if ((packet_status & 0x80) == 0x0)
		return 0;

	return packet_length;
}

void enc28j60_free_received_packet (void)
{
	/* Decrement packet counter - we are serviced this packet */
	enc28j60_BitField_Set (ECON2, ECON2_PKTDEC);

	/* Move read pointer to the start of the next received packet */
	/* This will free the memory in the ic buffer */
	enc28j60_Write_ETH_MAC_Reg (ERXRDPTL, LOW(next_packet_addr));
	enc28j60_Write_ETH_MAC_Reg (ERXRDPTH, HIGH(next_packet_addr));
}

void enc28j60_drop_received_packet (void)
{
	/* Get the address of the next packet and amount of received data */
	if (enc28j60_packet_read_prepare () > 0) {
		/* Drop received packet */
		enc28j60_free_received_packet ();
	}
}

void enc28j60_Get_RevID (PETH_Rev RevID)
{
 unsigned int PHY_ID1, PHY_ID2;

	/* Get ENC28J60 silicon Rev ID */
	enc28j60_Bank_Select (EREVID);
	RevID->DEV_RevID = enc28j60_Read_ETH_Reg (EREVID);
	/* Read PHY information registers and convert to PHY_ID values */
	PHY_ID1 = enc28j60_Read_PHY_Reg (PHID1);
	PHY_ID2 = enc28j60_Read_PHY_Reg (PHID2);
	RevID->PHY_Ident = PHY_ID2 & 0xFC00;
	RevID->PHY_Ident <<= 6;
	RevID->PHY_Ident |= PHY_ID1;
	RevID->PHI_RevID = PHY_ID2 & 0x0F;
	RevID->PHY_PartNumber = (PHY_ID2 >> 4) & 0x3F;
	/* Select Bank 0 */
	enc28j60_Bank_Select (ERDPTL);
}

void enc28j60_Reset (void)
{
	unsigned char Data;
	unsigned int AmountToFlush = 1;

    /* Move System Reset opcode to the buffer */
    Data = SR;
    /* Select the chip and send the Read Control Register opcode/address */
    ptr_spi->ioctl (SET_ETHERNET_CS, 0);
    /* Send WCR opcode with address and data that we must write*/
    ptr_spi->write (&Data, 1);
    /* Wait for data transmitted and remove 1 dummy byte */
    ptr_spi->ioctl (FLUSH_RX_BUFFER, (void *) &AmountToFlush);
    /* Unselect the chip */
    ptr_spi->ioctl (CLR_ETHERNET_CS, 0);
}

int enc28j60_Check_Operational_State (unsigned int Timeout)
{
	unsigned char Temp;

	/*
	Wait for CLKRDY to become set.
	Bit 3 in ESTAT is an unimplemented bit.  If it reads out as '1' that
	means the part is in RESET or otherwise our SPI pin is being driven
	incorrectly.  Make sure it is working before proceeding.
	*/
	do {
		Temp = enc28j60_Read_ETH_Reg (ESTAT);
		Timeout--;
		if (Timeout == 0) {
		/* Set LEDs to blink periodically */
		enc28j60_Set_LED (0x0AA2);
		return -1;
		}
   } while((Temp & (1 << 3)) || (~Temp & ESTAT_CLKRDY));

	return 1;
}

signed enc28j60_Init (unsigned char *pMACAddr)
{
	volatile unsigned int Temp = 0;

	/* Check if ENC28J60 go out from Hardware Reset */
	if (enc28j60_Check_Operational_State (30000) < 0) {
		printf ("ENC28J60 HR checkout error!\n\r");
		return -1;
	}

	/* Issue Software Reset to the ENC28J60 */
	enc28j60_Reset ();

	/* Additional delay before init - B4 silicon errata work around */
	while (Temp != 50000)
		Temp++;


	/* Enter Bank 1 and configure Receive Filters
	 * (No need to reconfigure - Unicast OR Broadcast with CRC checking is acceptable after Reset)
     */
	enc28j60_Bank_Select (ERXFCON);
	enc28j60_Write_ETH_MAC_Reg (ERXFCON, ERXFCON_CRCEN | ERXFCON_UCEN | ERXFCON_BCEN);

	/* Select Bank 0 */
    enc28j60_Bank_Select (ERXSTL);
	/* Fill RX buffer start address */
	enc28j60_Write_ETH_MAC_Reg (ERXSTL, LOW(RXSTART));
	enc28j60_Write_ETH_MAC_Reg (ERXSTH, HIGH(RXSTART));
	/* Set receive pointer address to the start of the RX buffer LSB first*/
	enc28j60_Write_ETH_MAC_Reg (ERXRDPTL, LOW(RXSTART));
	enc28j60_Write_ETH_MAC_Reg (ERXRDPTH, HIGH(RXSTART));
	next_packet_addr = RXSTART;
	/* Fill RX buffer end address */
	enc28j60_Write_ETH_MAC_Reg (ERXNDL, LOW(RXSTOP));
	enc28j60_Write_ETH_MAC_Reg (ERXNDH, HIGH(RXSTOP));

	/* Fill TX buffer start address */
	enc28j60_Write_ETH_MAC_Reg (ETXSTL, LOW(TXSTART));
	enc28j60_Write_ETH_MAC_Reg (ETXSTH, HIGH(TXSTART));
	/* Fill TX buffer stop address */
	enc28j60_Write_ETH_MAC_Reg (ETXNDL, LOW(TXSTOP));
	enc28j60_Write_ETH_MAC_Reg (ETXNDH, HIGH(TXSTOP));

	/* Configuring MAC */
	enc28j60_Bank_Select (MACON1);
	/* Enable the receive portion of the MAC */
	enc28j60_Write_ETH_MAC_Reg (MACON1, MACON1_TXPAUS | MACON1_RXPAUS | MACON1_MARXEN);
	/* Pad packets to 60 bytes, add CRC, and check */
	enc28j60_Write_ETH_MAC_Reg (MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);
	 /* Allow infinite deferals if the medium is continuously busy */
	enc28j60_Write_ETH_MAC_Reg (MACON4, MACON4_DEFER);
	/*  Late collisions occur beyond 63+8 bytes (8 bytes for preamble/start of frame delimiter)
		55 is all that is needed for IEEE 802.3, but ENC28J60 B5 errata for improper link pulse
		collisions will occur less often with a larger number.
	*/
	enc28j60_Write_ETH_MAC_Reg (MACLCON2, 63);
	/*  Set non-back-to-back inter-packet gap to 9.6us - minimum. */
	enc28j60_Write_ETH_MAC_Reg (MAIPGL, 0x12);
	enc28j60_Write_ETH_MAC_Reg (MAIPGH, 0x0C);
	/* Set the back-to-back inter-packet gap time to IEEE specified requirements - 0x12 is 9.6us in half duplex */
	enc28j60_Write_ETH_MAC_Reg (MABBIPG, 0x12);
	/* Increase stability for the long cabels Collision Window ++*/
	enc28j60_Write_ETH_MAC_Reg (MACLCON2, 0x3C);
	/* Set the maximum packet size which the controller will accept */
	enc28j60_Write_ETH_MAC_Reg (MAMXFLL, LOW(MAX_PACKET_SIZE));
	enc28j60_Write_ETH_MAC_Reg (MAMXFLH, HIGH(MAX_PACKET_SIZE));


	/* Copy used board MAC address to the ENC28J60 */
	enc28j60_Bank_Select (MAADR1);
	enc28j60_Write_ETH_MAC_Reg (MAADR6, pMACAddr[5]);
	enc28j60_Write_ETH_MAC_Reg (MAADR5, pMACAddr[4]);
	enc28j60_Write_ETH_MAC_Reg (MAADR4, pMACAddr[3]);
	enc28j60_Write_ETH_MAC_Reg (MAADR3, pMACAddr[2]);
	enc28j60_Write_ETH_MAC_Reg (MAADR2, pMACAddr[1]);
	enc28j60_Write_ETH_MAC_Reg (MAADR1, pMACAddr[0]);

	/* Disable half duplex loopback in PHY.  Bank bits changed to Bank 2 as a  side effect. */
	enc28j60_Write_PHY_Reg (PHCON2, PHCON2_HDLDIS);

	/* Configure LEDA to display RX activity + Link, LEDB to display TX activity (40ms) */
	enc28j60_Set_LED (0x0C12);

	/* Set Half Duplex mode */
	Temp = enc28j60_Read_PHY_Reg (PHCON1);
	Temp &= ~PHCON1_PDPXMD;
	enc28j60_Write_PHY_Reg (PHCON1, Temp);
	/* Clear FULDPX bit in MACON3 register - Half duplex */
	enc28j60_Bank_Select (MACON3);
	/* Stupid action because after reset this bit is always '0' */
	enc28j60_Write_ETH_MAC_Reg (MACON3, enc28j60_Read_MAC_Reg (MACON3) & ~MACON3_FULDPX);

	/* Enable receive logic */
	enc28j60_BitField_Set (ECON1, ECON1_RXEN);

	/* Go out with Bank0 selected*/
	enc28j60_Bank_Select (ERDPTL);

	return 1;
}

