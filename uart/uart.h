#ifndef _UART_H_
#define _UART_H_

/* Define Uart0 TX and RX buffer size */
#define UART0_RX_BUFFER_SIZE	256
#define UART0_TX_BUFFER_SIZE	UART0_RX_BUFFER_SIZE
/* Define Uart1 TX and RX buffer size */
#define UART1_RX_BUFFER_SIZE	256
#define UART1_TX_BUFFER_SIZE	UART1_RX_BUFFER_SIZE

/* Define PURGE TX/RX FIFO constants */
#define	PURGE_RXCLEAR		(1)
#define	PURGE_TXCLEAR		(2)

/* Definitions for typical UART Stop Bit settings */
#define	ONESTOPBIT			(0)			/* 1 stop bit */
#define	TWOSTOPBITS			(1)			/* 2 stop bits */


/* Definitions for typical UART 'Baud Rate' settings */
#define	CBR_1200			(1200)
#define	CBR_2400			(2400)
#define	CBR_4800			(4800)
#define	CBR_9600			(9600)
#define	CBR_14400			(14400)
#define	CBR_19200			(19200)
#define	CBR_38400			(38400)
#define	CBR_57600			(57600)
#define	CBR_115200			(115200)


/* Definitions for typical UART Parity settings */
#define	NOPARITY			(0)			/* No Parity        */
#define	ODDPARITY			(1)			/* Odd Parity       */
#define	EVENPARITY			(3)			/* Even Parity      */
#define	MARKPARITY			(5)			/* MARK "1" Parity  */
#define	SPACEPARITY			(7)			/* SPACE "0" Parity */


/* Definitions for typical UART number of bits in the data bytes transmitted and received */
#define	CHAR_LENGTH_8		(3)
#define	CHAR_LENGTH_7		(2)
#define	CHAR_LENGTH_6		(1)
#define	CHAR_LENGTH_5		(0)


/* Define RX trigger level and Enable bits for FIFO control register */
#define UART_FIFO_ENABLE	(0x01)
#define UART_FIFO_DISABLE	(0x00)
#define UART_FIFO_RX_TRIG1	(UART_FIFO_ENABLE + 0)
#define UART_FIFO_RX_TRIG4	(UART_FIFO_ENABLE + (1<<6))
#define UART_FIFO_RX_TRIG8	(UART_FIFO_ENABLE + (1<<7))
#define UART_FIFO_RX_TRIG14	(UART_FIFO_ENABLE + (3<<6))

/* UART DCB setting structure */
typedef struct DCB
{
	unsigned int  BaudRate :32;			/* Int value of the UART Baud Rate     */
	unsigned char StopBits;				/* Amount of Stop Bits: 1 or 2         */
	unsigned char Parity;				/* Used Parity: No, Even, Odd, Mark    */
	unsigned char ByteSize;				/* Data Byte Size: 8, 7, 6, 5          */
	unsigned char FIFO_Trigger;			/* Possible values are: 1,4,8,14 bytes */
}DCB;

typedef struct ERROR_FLAG
{
	unsigned char	OVERRUN	:1;			/* A character-buffer overrun has occurred. The next character is lost.               */
	unsigned char	RXOVER	:1;			/* An input buffer overflow has occurred. There is either no room in the input buffer */
	unsigned char	FRAME	:1;			/* Hardware detected a framing error.                                                 */
	unsigned char	BREAK	:1;			/* Hardware detected a break condition.                                               */
	unsigned char	RXPARITY:1;			/* Hardware detected a parity error.                                                  */
	unsigned char	TXFULL	:1;			/* The application transmit character, but the output buffer was full.                */
	unsigned char	Reserved:2;			/* Reserved bits                                                                      */
}ERROR_FLAG;

typedef union UART_EERROR
{
	ERROR_FLAG EF;						/* ERROR_FLAG Structure */
	unsigned char ERROR_BYTE;			/* Whole eeror byte     */
}UART_EERROR;

typedef struct UART_STATUS
{
	unsigned char TXEMPTY		:1;		/* TX FIFO and TX SHIFT Register are Empty */
	unsigned char TX_RUNNING	:1;		/* We have data in Uart TX Buffer if '1' or data is not avalaible in buffer if '0' */
	unsigned char RX_Timeout	:1;		/* Flag is set when we see the Uart Character Time-out Indicator (CTI) */
	unsigned char Reserved		:5;		/* Reserved bits */
}UART_STATUS;

typedef struct TIMEOUTS
{
	unsigned int RX_TIMEOUT;			/* Timeout used to perform single character read operation from queue */
	unsigned int TX_TIMEOUT;			/* Timeout used for wait the free room in TX queue in single character transmit operation */
}TIMEOUTS;

/* Define UIIR register masks */
#define	UIIR_NO_INT			(1 << 0)    /* No Interrupts Pending           */
#define	UIIR_MS_INT			(0 << 1)    /* Modem Status Interrupt          */
#define	UIIR_THRE_INT		(1 << 1)    /* Transmit Holding Register Empty */
#define	UIIR_RDA_INT		(2 << 1)    /* Receive Data Available          */
#define	UIIR_RLS_INT		(3 << 1)    /* Receive Line Status             */
#define	UIIR_CTI_INT		(6 << 1)    /* Character Timeout Indicator     */
#define	UIIR_ID_MASK		(0x0E)      /* Register mask                   */

#if defined(ENABLE_UART0) || defined(ENABLE_UART1)
unsigned int Uart_SetFIFOControl (int FD);
unsigned int Uart_SetBaudRate(int FD);
unsigned int Uart_ClearCOMError (int FD);
unsigned int Uart_GetCommState (int FD, DCB *Dcb);
unsigned int Uart_SetCommState (int FD, DCB *Dcb);
unsigned int Uart_GetTimeouts (int FD, TIMEOUTS *Timeout);
unsigned int Uart_SetTimeouts (int FD, TIMEOUTS *Timeout);
#endif

#ifdef ENABLE_UART0
extern unsigned char	uart0_rx_buffer[];
extern unsigned int		uart0_rx_insert_idx, uart0_rx_extract_idx;

extern unsigned char	uart0_tx_buffer[];
extern unsigned int		uart0_tx_insert_idx, uart0_tx_extract_idx;

extern DCB			Uart0_DCB;
extern UART_EERROR	Uart0_Error;
extern UART_STATUS	Uart0_Status;
extern TIMEOUTS		Uart0_Timeout;

unsigned Uart0_Init (void);
unsigned Uart0_Open (void);
unsigned Uart0_Close (void);
signed Uart0_TX_Space (void);
signed Uart0_Putchar (unsigned char TX_char);
signed Uart0_Write (unsigned char *buffer, unsigned int Amount);
signed Uart0_IsGetReady (void);
signed Uart0_Getch (void);
signed Uart0_Read (unsigned char *buffer, unsigned int Amount);
#endif

#endif
