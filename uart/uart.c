#include "../main.h"
#include "../vic/vic.h"
#include "../vic/isr.h"
#include "uart.h"
#include <string.h>

#define	FD_UART0			(3)			/* Uart0 File Descriptor */
#define	FD_UART1			(4)			/* Uart1 File Descriptor */

#ifdef ENABLE_UART0
/* Uart0 TX & RX Bufers and Read/Write pointers */
unsigned char  uart0_rx_buffer[UART0_RX_BUFFER_SIZE];
unsigned int   uart0_rx_insert_idx, uart0_rx_extract_idx;

unsigned char  uart0_tx_buffer[UART0_TX_BUFFER_SIZE];
unsigned int   uart0_tx_insert_idx, uart0_tx_extract_idx;

/* Define UART0 DCB structure    */
DCB				Uart0_DCB;
/* Define Uart0 Error structure  */
UART_EERROR		Uart0_Error;
/* Define Uart0 Status structure */
UART_STATUS		Uart0_Status;
/* Define Uart0 Timeout structure */
TIMEOUTS		Uart0_Timeout;
#endif

#ifdef ENABLE_UART1
/* Uart0 TX & RX Bufers and Read/Write pointers */
unsigned char  uart1_rx_buffer[UART1_RX_BUFFER_SIZE];
unsigned int   uart1_rx_insert_idx, uart1_rx_extract_idx;

unsigned char  uart1_tx_buffer[UART1_TX_BUFFER_SIZE];
unsigned int   uart1_tx_insert_idx, uart1_tx_extract_idx;

/* Define UART0 DCB structure    */
DCB				Uart1_DCB;
/* Define Uart0 Error structure  */
UART_EERROR		Uart1_Error;
/* Define Uart0 Status structure */
UART_STATUS		Uart1_Status;
/* Define Uart0 Timeout structure */
TIMEOUTS		Uart1_Timeout;
#endif

#ifdef ENABLE_UART0
void Uart0_ISR (void)
{
	unsigned int Interrupt_ID;
	unsigned int temp_idx;
	unsigned int LineStatus, Count;

	/* While if at least one interrupt is pending */
	while (((Interrupt_ID = UART0_IIR) & UIIR_NO_INT) == 0) {
		switch (Interrupt_ID & UIIR_ID_MASK) {
			/* Receive Line Status */
			case UIIR_RLS_INT:
				/* Read Line Status Register */
				LineStatus = UART0_LSR;
				/* Overrun Error active */
				if (LineStatus & 2)
					/* Set OE flag in Uart0 Error structure */
					Uart0_Error.EF.OVERRUN = 1;
				/* Parity error active */
				if (LineStatus & 4)
						/* Set PE flag in Uart0 Error structure */
						Uart0_Error.EF.RXPARITY = 1;
				/* Frame error active */
				if (LineStatus & 8)
					/* Set FE flag in Uart0 Error structure */
					Uart0_Error.EF.FRAME = 1;
				/* If break detected */
				if (LineStatus & 16)
					/* Set BREAK flag in Uart0 Error structure */
					Uart0_Error.EF.BREAK = 1;
				break;
			/* Character Timeout Indicator - used for accumulate data to packet */
			case UIIR_CTI_INT:
				/* Set flag that we see character timeout indicator - may send all RX buffer to host */
				Uart0_Status.RX_Timeout = 1;
			/* Receive Data Available */
			case UIIR_RDA_INT:
				/* Read from FIFO amount less for 1 byte then trigger level (for CTI interrupt) */
				Count = Uart0_DCB.FIFO_Trigger - 1;
				do {
					/* calculate next insert index & store character */
					temp_idx = (uart0_rx_insert_idx + 1) % UART0_RX_BUFFER_SIZE;
					/* Read byte from FIFO and put it to the buffer */
					uart0_rx_buffer[uart0_rx_insert_idx] = UART1_RBR;
					/* Check for more room in queue */
					if (temp_idx != uart0_rx_extract_idx)
						/* Update insert index */
						uart0_rx_insert_idx = temp_idx;
					/* if no room in RX buffer */
					else
						/* An input buffer overflow has occurred */
						Uart0_Error.EF.RXOVER = 1;
					/* Decrement the trigger level counter */
					Count--;
					/* If we reed all data  with the exception of one (used for CTI interrupt) */
					if (!Count)
						/* Go out from read loop */
						break;
				} while (UART0_LSR & 1);		/* while any data is in RX FIFO: DR=1 */
				break;
			 /* Transmit Holding Register Empty */
			case UIIR_THRE_INT:
				/* While U0THR is empty THRE=1 */
				while (UART0_LSR & 32) {
					/* Check if more data to send */
					if (uart0_tx_extract_idx != uart0_tx_insert_idx) {
						/* Write data from TX buffer to the Uart */
						UART0_THR = uart0_tx_buffer[uart0_tx_extract_idx++];
						uart0_tx_extract_idx %= UART0_TX_BUFFER_SIZE;
					}
					/* If we don't have any data to transmit */
					else {
						/* Clear TX running flag */
						Uart0_Status.TX_RUNNING = 0;
						break;
					}
				}
				break;
		}
    }
}
#endif

#ifdef ENABLE_UART0
unsigned Uart0_Init (void)
{
	unsigned int volatile Tmp;
	DCB InitDCB;
    TIMEOUTS Timeouts;

	/* Set port pins for UART0 TX & RX */
	PINSEL0 = (PINSEL0 & ~(PINSEL0_P00_MASK | PINSEL0_P01_MASK)) | (PINSEL0_P00_TXD0 | PINSEL0_P01_RXD0);
	/* Switch Power On for Uart0 */
	PCONP |= PCUART0;
	/* Disable all interrupts */
	UART0_IER = 0x00;
	/* Read IIR register - clear interrupt ID */
	Tmp = UART0_IIR;
	/* Read U0LSR clear line status register */
	Tmp = UART0_LSR;

	/* Set default values into the Uart0 DCB structure: "9600 8N1" */
	InitDCB.BaudRate = CBR_115200;
	InitDCB.StopBits = ONESTOPBIT;
	InitDCB.Parity = NOPARITY;
	InitDCB.ByteSize = CHAR_LENGTH_8;
	InitDCB.FIFO_Trigger = 8;
	/* Set InitDCB structure to the Uart0 registers */
	Uart_SetCommState(FD_UART0, &InitDCB);

	/* Reset all errors flags for Uart0 */
	Uart0_Error.ERROR_BYTE = 0;
	/* Initialize the transmit data queue */
	uart0_tx_extract_idx = uart0_tx_insert_idx = 0;
	/* Clear running flag */
	Uart0_Status.TX_RUNNING = 0;
	/* Clear flag that was timeout when we receive data from other RS-232 device */
	Uart0_Status.RX_Timeout = 0;
	/* Enable THRE interrupt */
	UART0_IER |= 2;
	/* Initialize the receive data queue */
	uart0_rx_extract_idx = uart0_rx_insert_idx = 0;
	/* Enable receiver interrupts */
	UART0_IER |= 1;
	/* Enable line status interrupt */
	UART0_IER |= 4;

	/* Set Uart RX timeout: without timeout */
	Timeouts.RX_TIMEOUT = 0;
	/* Set Uart TX timeout: equal 100ms */
	Timeouts.TX_TIMEOUT = 100;
	Uart_SetTimeouts (FD_UART0, &Timeouts);

  	/* Install Uart0 as IRQ interrupt */
	VIC_Install_IRQ_Callback (VIC_Channel_UART0, INTERRUPT_SLOT_UART0, Uart0_ISR);
	/* Enable UART0 interrupt */
	VIC_Interrupt_Enable (VIC_Int_Sorce_UART0);

	return 1;
}
#endif

#if defined(ENABLE_UART0) || defined(ENABLE_UART1)
unsigned int Uart_GetCommState (int FD, DCB *Dcb)
{
#if defined(ENABLE_UART0)
	/* If File Descriptor belong Uart0 */
	if (FD == FD_UART0) {
		memcpy(Dcb, &Uart0_DCB, sizeof(DCB));
		return 1;
	}
#endif

#if defined(ENABLE_UART1)
	/* If File Descriptor belong Uart1 */
	if (FD == FD_UART1) {
		memcpy(Dcb, &Uart1_DCB, sizeof(DCB));
		return 1;
	}
#endif
	/* File Descriptor don't belong any of our Uart's */
	return 0;
}

unsigned int Uart_SetCommState (int FD, DCB *Dcb)
{
	unsigned char Tmp = 0;

#if defined(ENABLE_UART0)
	/* If File Descriptor belong Uart0 */
	if (FD == FD_UART0) {
		/* Save settings to the Uart0 DCB structure */
		memcpy (&Uart0_DCB, Dcb, sizeof(DCB));
		/* Convert fields of the DCB structure to the LCR value */
		Tmp |= Uart0_DCB.ByteSize;
		Tmp |= (Uart0_DCB.StopBits << 2);
		Tmp |= (Uart0_DCB.Parity << 3);
		/* Write new value to the LCR register */
		UART0_LCR = Tmp;
		/* Set desired BaudRate */
		Uart_SetBaudRate (FD_UART0);
		/* Set RX FIFO settings */
		Uart_SetFIFOControl (FD_UART0);
		return 1;
	}
#endif

#if defined(ENABLE_UART1)
	/* If File Descriptor belong Uart1 */
	if (FD == FD_UART1) {
		/* Save settings to the Uart1 DCB structure */
		memcpy (&Uart1_DCB, Dcb, sizeof(DCB));
		/* Convert fields of the DCB structure to the LCR value */
		Tmp |= Uart1_DCB.ByteSize;
		Tmp |= (Uart1_DCB.StopBits << 2);
		Tmp |= (Uart1_DCB.Parity << 3);
		/* Write new value to the LCR register */
		UART1_LCR = Tmp;
		/* Set desired BaudRate */
		Uart_SetBaudRate (FD_UART1);
		/* Set RX FIFO settings */
		Uart_SetFIFOControl (FD_UART1);
		return 1;
	}
#endif
	return 0;
}

unsigned int Uart_SetTimeouts (int FD, TIMEOUTS *Timeout)
{
#if defined(ENABLE_UART0)
	/* If File Descriptor belong Uart0 */
	if (FD == FD_UART0) {
		/* Save settings to the Uart0 Timeout structure */
		memcpy (&Uart0_Timeout, Timeout, sizeof(TIMEOUTS));
		return 1;
	}
#endif

#if defined(ENABLE_UART1)
	/* If File Descriptor belong Uart1 */
	if (FD == FD_UART1) {
	/* Save settings to the Uart1 DCB structure */
		memcpy (&Uart1_Timeout, Timeout, sizeof(TIMEOUTS));
		return 1;
	}
#endif
	return 0;
}

unsigned int Uart_GetTimeouts (int FD, TIMEOUTS *Timeout)
{
#if defined(ENABLE_UART0)
	/* If File Descriptor belong Uart0 */
	if (FD == FD_UART0) {
		/* Save settings to the Uart0 Timeout structure */
		memcpy (Timeout, &Uart0_Timeout, sizeof(TIMEOUTS));
		return 1;
	}
#endif

#if defined(ENABLE_UART1)
	/* If File Descriptor belong Uart1 */
	if (FD == FD_UART1) {
	/* Save settings to the Uart1 DCB structure */
		memcpy (Timeout, &Uart1_Timeout, sizeof(TIMEOUTS));
		return 1;
	}
#endif
	return 0;
}

unsigned int Uart_SetFIFOControl (int FD)
{
#if defined(ENABLE_UART0)
	/* If File Descriptor belong Uart0 */
	if (FD == FD_UART0) {
		/* Clear TX and RX FIFO */
		UART0_FCR = UART_FCR_CLR;
		/* Write new settings to the FCR register */
		switch (Uart0_DCB.FIFO_Trigger) {
			case 0:
				UART0_FCR = UART_FIFO_DISABLE;
				break;
			case 1:
				UART0_FCR = UART_FIFO_RX_TRIG1;
				break;
			case 4:
				UART0_FCR = UART_FIFO_RX_TRIG4;
				break;
			case 8:
				UART0_FCR = UART_FIFO_RX_TRIG8;
				break;
			case 14:
				UART0_FCR = UART_FIFO_RX_TRIG14;
				break;
			default:
				UART0_FCR = UART_FIFO_DISABLE;
		}
		return 1;
	}
#endif

#if defined(ENABLE_UART1)
	if (FD == FD_UART1) {
		/* Clear TX and RX FIFO */
		UART1_FCR = UART_FCR_CLR;
		/* Write new settings to the FCR register */
		switch (Uart1_DCB.FIFO_Trigger) {
			case 0:
				UART1_FCR = UART_FIFO_DISABLE;
				break;
			case 1:
				UART1_FCR = UART_FIFO_RX_TRIG1;
				break;
			case 4:
				UART1_FCR = UART_FIFO_RX_TRIG4;
				break;
			case 8:
				UART1_FCR = UART_FIFO_RX_TRIG8;
				break;
			case 14:
				UART1_FCR = UART_FIFO_RX_TRIG14;
				break;
			default:
				UART1_FCR = UART_FIFO_DISABLE;
		}
		return 1;
	}
#endif
	return 0;
}


unsigned int Uart_SetBaudRate(int FD)
{
	unsigned int BaudValue;

	/* Calculate Divisor Latch */
	BaudValue = CPU_CLOCK_HZ / (Uart0_DCB.BaudRate * 16);

#if defined(ENABLE_UART0)
	/* If File Descriptor belong Uart0 */
	if (FD == FD_UART0) {
		/* Select divisor latches (DLAB = 1: Enable access to Divisor Latches) */
		UART0_LCR |= UART_LCR_DLAB;
		/* Set Divisor Latch low byte LSB */
		UART0_DLL = (unsigned char)(BaudValue);
		/* Set Divisor Latch high byte MSB */
		UART0_DLM = (unsigned char)(BaudValue >> 8);
		/* Disable divisor latches (DLAB = 0: Disable access to Divisor Latches) */
		UART0_LCR &= ~UART_LCR_DLAB;

		return 1;
	}
#endif

#if defined(ENABLE_UART1)
	/* If File Descriptor belong Uart1 */
	if (FD == FD_UART1) {
		/* Select divisor latches (DLAB = 1: Enable access to Divisor Latches) */
		UART1_LCR |= UART_LCR_DLAB;
		/* Set Divisor Latch low byte LSB */
		UART1_DLL = (unsigned char)(BaudValue);
		/* Set Divisor Latch high byte MSB */
		UART1_DLM = (unsigned char)(BaudValue >> 8);
		/* Disable divisor latches (DLAB = 0: Disable access to Divisor Latches) */
		UART1_LCR &= ~UART_LCR_DLAB;

		return 1;
	}
#endif
	return 0;
}


unsigned int Uart_ClearCOMError (int FD)
{
#if defined(ENABLE_UART0)
	if (FD == FD_UART0) {
		/* Clear all errors */
		Uart0_Error.ERROR_BYTE = 0;
		return 1;
	}
#endif

#if defined(ENABLE_UART1)
	if (FD == FD_UART1) {
		/* Clear all errors */
		Uart1_Error.ERROR_BYTE = 0;
		return 1;
	}
#endif
	return 0;
}

unsigned int Uart_PurgeCOM (int FD, unsigned int Flush)
{
	unsigned int CPSR_Save;

#if defined(ENABLE_UART0)
	if (FD == FD_UART0) {
		/* Enter Critical Section */
		CPSR_Save = Disable_INT ();
		if (Flush & PURGE_RXCLEAR) {
			/* Clear Uart RX FIFO */
			UART0_FCR |= 2;
			/* Reset receive data queue */
			uart0_rx_extract_idx = uart0_rx_insert_idx = 0;
		}
		if (Flush & PURGE_TXCLEAR) {
			/* Clear Uart TX FIFO */
			UART0_FCR |= 4;
			/* Reset receive data queue */
			uart0_tx_extract_idx = uart0_tx_insert_idx = 0;
		}
		/* Exit Critical Section */
		Restore_INT (CPSR_Save);

		return 1;
	}
#endif

#if defined(ENABLE_UART1)
	if (FD == FD_UART1) {
		/* Enter Critical Section */
		CPSR_Save = Disable_INT ();
		if (Flush & PURGE_RXCLEAR) {
			/* Clear Uart RX FIFO */
			UART1_FCR |= 2;
			/* Reset receive data queue */
			uart1_rx_extract_idx = uart1_rx_insert_idx = 0;
		}
		if (Flush & PURGE_TXCLEAR) {
			/* Clear Uart TX FIFO */
			UART1_FCR |= 4;
			/* Reset receive data queue */
			uart1_tx_extract_idx = uart1_tx_insert_idx = 0;
		}
		/* Exit Critical Section */
		Restore_INT (CPSR_Save);

		return 1
	}
#endif
	return 0;
}
#endif
