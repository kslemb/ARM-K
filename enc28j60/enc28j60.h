#ifndef _ENC28J60_H_
#define _ENC28J60_H_

/* ENC28J60 Device Information Structure */
typedef struct ETH_Rev
{
	unsigned int PHY_Ident;
	unsigned char PHY_PartNumber;
	unsigned char PHI_RevID;
	unsigned char DEV_RevID;
} ETH_Rev, *PETH_Rev;

/* ENC28J60 Commands Opcodes */
#define	WCR (2<<5)		/* Write Control Register */
#define BFS (4<<5)		/* Bit Field Set */
#define	BFC (5<<5)		/* Bit Field Clear */
#define	RCR (0<<5)		/* Read Control Register */
#define RBM ((1<<5) | 0x1A)	/* Read Buffer Memory */
#define	WBM ((3<<5) | 0x1A)	/* Write Buffer Memory */
#define	SR  ((7<<5) | 0x1F)	/* System Reset */

/* Register Declaration: LSB - Register Address, MSB - Bank */

/* Bank 0 registers */
#define ERDPTL		0x000
#define ERDPTH		0x001
#define EWRPTL		0x002
#define EWRPTH		0x003
#define ETXSTL		0x004
#define ETXSTH		0x005
#define ETXNDL		0x006
#define ETXNDH		0x007
#define ERXSTL		0x008
#define ERXSTH		0x009
#define ERXNDL		0x00A
#define ERXNDH		0x00B
#define ERXRDPTL	0x00C
#define ERXRDPTH	0x00D
#define ERXWRPTL	0x00E
#define ERXWRPTH	0x00F
#define EDMASTL		0x010
#define EDMASTH		0x011
#define EDMANDL		0x012
#define EDMANDH		0x013
#define EDMADSTL	0x014
#define EDMADSTH	0x015
#define EDMACSL		0x016
#define EDMACSH		0x017
#define EIE		0x01B
#define EIR		0x01C
#define ESTAT		0x01D
#define ECON2		0x01E
#define ECON1		0x01F

/* Bank 1 registers */
#define EHT0		0x100
#define EHT1		0x101
#define EHT2		0x102
#define EHT3		0x103
#define EHT4		0x104
#define EHT5		0x105
#define EHT6		0x106
#define EHT7		0x107
#define EPMM0		0x108
#define EPMM1		0x109
#define EPMM2		0x10A
#define EPMM3		0x10B
#define EPMM4		0x10C
#define EPMM5		0x10D
#define EPMM6		0x10E
#define EPMM7		0x10F
#define EPMCSL		0x110
#define EPMCSH		0x111
#define EPMOL		0x114
#define EPMOH		0x115
#define ERXFCON		0x118
#define EPKTCNT		0x119

/* Bank 2 registers */
#define MACON1		0x200
#define MACON3		0x202
#define MACON4		0x203
#define MABBIPG		0x204
#define MAIPGL		0x206
#define MAIPGH		0x207
#define MACLCON1	0x208
#define MACLCON2	0x209
#define MAMXFLL		0x20A
#define MAMXFLH		0x20B
#define MICMD		0x212
#define MIREGADR	0x214
#define MIWRL		0x216
#define MIWRH		0x217
#define MIRDL		0x218
#define MIRDH		0x219

/* Bank 3 registers */
#define MAADR5		0x300
#define MAADR6		0x301
#define MAADR3		0x302
#define MAADR4		0x303
#define MAADR1		0x304
#define MAADR2		0x305
#define EBSTSD		0x306
#define EBSTCON		0x307
#define EBSTCSL		0x308
#define EBSTCSH		0x309
#define MISTAT		0x30A
#define EREVID		0x312
#define ECOCON		0x315
#define EFLOCON		0x317
#define EPAUSL		0x318
#define EPAUSH		0x319


/* PHY Register Declaration */
#define PHCON1      	0x00
#define PHSTAT1     	0x01
#define PHID1       	0x02
#define PHID2       	0x03
#define PHCON2      	0x10
#define PHSTAT2     	0x11
#define PHIE        	0x12
#define PHIR        	0x13
#define PHLCON      	0x14


/* ETH/MAC/MII Registers Bits Declaration */

/* EIE */
#define	EIE_INTIE	(1<<7)
#define	EIE_PKTIE	(1<<6)
#define	EIE_DMAIE	(1<<5)
#define	EIE_LINKIE	(1<<4)
#define	EIE_TXIE	(1<<3)
#define	EIE_TXERIE	(1<<1)
#define	EIE_RXERIE	(1)

/* EIR */
#define	EIR_PKTIF	(1<<6)
#define	EIR_DMAIF	(1<<5)
#define	EIR_LINKIF	(1<<4)
#define	EIR_TXIF	(1<<3)
#define	EIR_TXERIF	(1<<1)
#define	EIR_RXERIF	(1)

/* ESTAT */
#define	ESTAT_INT	(1<<7)
#define ESTAT_BUFER	(1<<6)
#define	ESTAT_LATECOL	(1<<4)
#define	ESTAT_RXBUSY	(1<<2)
#define	ESTAT_TXABRT	(1<<1)
#define	ESTAT_CLKRDY	(1)

/* ECON2 */
#define	ECON2_AUTOINC	(1<<7)
#define	ECON2_PKTDEC	(1<<6)
#define	ECON2_PWRSV	(1<<5)
#define	ECON2_VRPS	(1<<3)

/* ECON1 */
#define	ECON1_TXRST	(1<<7)
#define	ECON1_RXRST	(1<<6)
#define	ECON1_DMAST	(1<<5)
#define	ECON1_CSUMEN	(1<<4)
#define	ECON1_TXRTS	(1<<3)
#define	ECON1_RXEN	(1<<2)
#define	ECON1_BSEL1	(1<<1)
#define	ECON1_BSEL0	(1)

/* ERXFCON */
#define	ERXFCON_UCEN	(1<<7)
#define	ERXFCON_ANDOR	(1<<6)
#define	ERXFCON_CRCEN	(1<<5)
#define	ERXFCON_PMEN	(1<<4)
#define	ERXFCON_MPEN	(1<<3)
#define	ERXFCON_HTEN	(1<<2)
#define	ERXFCON_MCEN	(1<<1)
#define	ERXFCON_BCEN	(1)

/* MACON1 */
#define	MACON1_TXPAUS	(1<<3)
#define	MACON1_RXPAUS	(1<<2)
#define	MACON1_PASSALL	(1<<1)
#define	MACON1_MARXEN	(1)

/* MACON3 */
#define	MACON3_PADCFG2	(1<<7)
#define	MACON3_PADCFG1	(1<<6)
#define	MACON3_PADCFG0	(1<<5)
#define	MACON3_TXCRCEN	(1<<4)
#define	MACON3_PHDREN	(1<<3)
#define	MACON3_HFRMEN	(1<<2)
#define	MACON3_FRMLNEN	(1<<1)
#define	MACON3_FULDPX	(1)

/* MACON4 */
#define	MACON4_DEFER	(1<<6)
#define	MACON4_BPEN	(1<<5)
#define	MACON4_NOBKOFF	(1<<4)

/* MICMD */
#define	MICMD_MIISCAN	(1<<1)
#define	MICMD_MIIRD	(1)

/* EBSTCON */
#define	EBSTCON_PSV2	(1<<7)
#define	EBSTCON_PSV1	(1<<6)
#define	EBSTCON_PSV0	(1<<5)
#define	EBSTCON_PSEL	(1<<4)
#define	EBSTCON_TMSEL1	(1<<3)
#define	EBSTCON_TMSEL0	(1<<2)
#define	EBSTCON_TME	(1<<1)
#define	EBSTCON_BISTST	(1)

/* MISTAT */
#define	MISTAT_NVALID	(1<<2)
#define	MISTAT_SCAN	(1<<1)
#define	MISTAT_BUSY	(1)

/* ECOCON */
#define	ECOCON_COCON2	(1<<2)
#define	ECOCON_COCON1	(1<<1)
#define	ECOCON_COCON0	(1)

/* EFLOCON */
#define	EFLOCON_FULDPXS	(1<<2)
#define	EFLOCON_FCEN1	(1<<1)
#define	EFLOCON_FCEN0	(1)


/* ETH/MAC/MII Registers Bits Declaration */

/* PHCON1 */
#define	PHCON1_PRST	(1<<15)
#define	PHCON1_PLOOPBK	(1<<14)
#define	PHCON1_PPWRSV	(1<<11)
#define	PHCON1_PDPXMD	(1<<8)

/* PHSTAT1 */
#define	PHSTAT1_PFDPX	(1<<12)
#define	PHSTAT1_PHDPX	(1<<11)
#define	PHSTAT1_LLSTAT	(1<<2)
#define	PHSTAT1_JBSTAT	(1<<1)

/* PHID2 */
#define	PHID2_PID24	(1<<15)
#define	PHID2_PID23	(1<<14)
#define	PHID2_PID22	(1<<13)
#define	PHID2_PID21	(1<<12)
#define	PHID2_PID20	(1<<11)
#define	PHID2_PID19	(1<<10)
#define	PHID2_PPN5	(1<<9)
#define	PHID2_PPN4	(1<<8)
#define	PHID2_PPN3	(1<<7)
#define	PHID2_PPN2	(1<<6)
#define	PHID2_PPN1	(1<<5)
#define	PHID2_PPN0	(1<<4)
#define	PHID2_PREV3	(1<<3)
#define	PHID2_PREV2	(1<<2)
#define	PHID2_PREV1	(1<<1)
#define	PHID2_PREV0	(1)

/* PHCON2 */
#define	PHCON2_FRCLNK	(1<<14)
#define	PHCON2_TXDIS	(1<<13)
#define	PHCON2_JABBER	(1<<10)
#define	PHCON2_HDLDIS	(1<<8)

/* PHSTAT2 */
#define	PHSTAT2_TXSTAT	(1<<13)
#define	PHSTAT2_RXSTAT	(1<<12)
#define	PHSTAT2_COLSTAT	(1<<11)
#define	PHSTAT2_LSTAT	(1<<10)
#define	PHSTAT2_DPXSTAT	(1<<9)
#define	PHSTAT2_PLRITY	(1<<5)

/* PHIE */
#define	PHIE_PLNKIE	(1<<4)
#define	PHIE_PGEIE	(1<<1)

/* PHIR bits */
#define	PHIR_PLNKIF	(1<<4)
#define	PHIR_PGIF	(1<<2)

/* PHLCON */
#define	PHLCON_LACFG3	(1<<11)
#define	PHLCON_LACFG2	(1<<10)
#define	PHLCON_LACFG1	(1<<9)
#define	PHLCON_LACFG0	(1<<8)
#define	PHLCON_LBCFG3	(1<<7)
#define	PHLCON_LBCFG2	(1<<6)
#define	PHLCON_LBCFG1	(1<<5)
#define	PHLCON_LBCFG0	(1<<4)
#define	PHLCON_LFRQ1	(1<<3)
#define	PHLCON_LFRQ0	(1<<2)
#define	PHLCON_STRCH	(1<<1)

#define LOW(data)	(unsigned char)(data)
#define HIGH(data)	(unsigned char)((data) >> 8)

/* Define TX and RX buffer address in internal RAM of the ENC28J60 */
/* Use all internal memory for RX Buffer excluding one TX buffer at the top of internal RAM */
#define RAMSIZE			(8 * 1024)
#define MAX_PACKET_SIZE	1500 /* Must be changed in future for add place for status vector added after TX */

#define RXSTART		0x0000
#define RXSTOP		(RAMSIZE - MAX_PACKET_SIZE - 1)
#define TXSTART		(RAMSIZE - 1500)
#define TXSTOP		(RAMSIZE - 1)

/* Set LED's on the Ethernet jack */
#define enc28j60_Set_LED(LEDState)		enc28j60_Write_PHY_Reg (PHLCON, LEDState)

/* Function ptototypes defined in ENC28J60 module */

void enc28j60_Bank_Select (unsigned int Register);

void enc28j60_BitField_Clear (unsigned int Address, unsigned char BitField);
void enc28j60_BitField_Set (unsigned int Address, unsigned char BitField);
unsigned char enc28j60_Read_ETH_Reg (unsigned int Address);
unsigned char enc28j60_Read_MAC_Reg (unsigned int Address);
void enc28j60_Write_ETH_MAC_Reg (unsigned int Address, unsigned char Value);
unsigned int enc28j60_Read_PHY_Reg (unsigned char Register);
void enc28j60_Write_PHY_Reg (unsigned char Register, unsigned int Value);

int enc28j60_open (void);
void enc28j60_Get_RevID (PETH_Rev RevID);
void enc28j60_Reset (void);

signed enc28j60_Init (unsigned char *pMACAddr);

signed enc28j60_Write_Buffer (unsigned char *pData, unsigned int Length);
void enc28j60_packet_send_prepare (void);
void enc28j60_packet_send (unsigned int TotalLength);

signed enc28j60_Read_Buffer (unsigned char *pData, unsigned int Length);
signed enc28j60_Check_If_Packet_Received (void);
unsigned enc28j60_packet_read_prepare (void);
void enc28j60_free_received_packet (void);
void enc28j60_drop_received_packet (void);

#endif
