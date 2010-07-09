#include "../main.h"
#include "../timer0/timer0.h"
#include "../vic/isr.h"
#include "uart.h"

#ifdef ENABLE_UART0

signed Uart0_TX_Space (void)
{
	int space;

	if((space = (uart0_tx_extract_idx - uart0_tx_insert_idx)) <= 0)
		space += UART0_TX_BUFFER_SIZE;

	return (unsigned int)(space - 1);
}

signed Uart0_Putchar (unsigned char TX_char)
{
	unsigned int temp, StartTime, IsSpace, CPSR_Save;

	/* Get index of the next available position in TX buffer */
	temp = (uart0_tx_insert_idx + 1) % UART0_TX_BUFFER_SIZE;
	/* If in TX buffer is no room */
	if (temp == uart0_tx_extract_idx) {
		StartTime = TickGet ();
		/* Stay in loop while wait time elapsed or we see the room in TX buffer */
		while (TickGetDiff (TickGet (), StartTime) < TICKS_IN_MS (Uart0_Timeout.TX_TIMEOUT)) {
			if((IsSpace = Uart0_TX_Space ()) != 0)
				break;
		}
		if(!IsSpace) {
			/* Set Flag of TX Buffer OverRun */
			Uart0_Error.EF.TXFULL = 1;
			return -1;
		}
	}
	/* Enter Critical Section */
	CPSR_Save = Disable_INT ();
	/* Check if process of sending data is running */
	if (Uart0_Status.TX_RUNNING) {
		/* add to queue */
		uart0_tx_buffer[uart0_tx_insert_idx] = TX_char;
		/* save new TX buffer write index */
		uart0_tx_insert_idx = temp;
	}
	/* If process of sending data is not running */
	else {
		/* Set running flag */
		Uart0_Status.TX_RUNNING = 1;
		/* Write data to output register */
		UART0_THR = TX_char;
	}
	/* Exit Critical Section */
	Restore_INT (CPSR_Save);

	return 1;
}

signed Uart0_Write (unsigned char *buffer, unsigned int Amount)
{
	signed Writen = 0;

	while (Amount--) {
		/* Write charcter to the TX queue */
		if (Uart0_Putchar (*buffer++) > 0)
			Writen++;
		else
			break;
	}

	return (Writen > 0) ? Writen : -1;
}


signed Uart0_IsGetReady (void)
{
	unsigned Amount;

	/* Calculate amount of data received */
	if((Amount = (uart0_rx_insert_idx - uart0_rx_extract_idx)) < 0)
		Amount += UART0_RX_BUFFER_SIZE;

	return (signed) (Amount > 0) ? Amount : -1;
}

signed Uart0_Getch (void)
{
	unsigned int StartTime;
	signed IsReady = -1, RX_char;

	/* Check if character is available */
	if (Uart0_IsGetReady() < 0) {
			StartTime = TickGet ();
			/* Stay in loop while wait time elapsed or we get character from RX queue */
			while (TickGetDiff (TickGet (), StartTime) < TICKS_IN_MS (Uart0_Timeout.RX_TIMEOUT)) {
				if ((IsReady = Uart0_IsGetReady ()) > 0)
					break;

			}
			/* If timeout elapsed but we don't receive charater */
			if (IsReady < 0)
				return -1;
		}
	/* Get character from RX buffer and increment pointer */
	RX_char = (signed) uart0_rx_buffer[uart0_rx_extract_idx++];
	/* Limit the pointer to RX buffer size */
	uart0_rx_extract_idx %= UART0_RX_BUFFER_SIZE;

	return RX_char;
}

signed Uart0_Read (unsigned char *buffer, unsigned int Amount)
{
	signed Received;
	unsigned i = 0;

	while (i != Amount) {
		Received = Uart0_Getch ();
		if (Received < 0)
			break;
		*buffer++ = (unsigned char) Received;
		i++;
	}
	return (i > 0) ? i : -1;
}

unsigned Uart0_Open (void)
{
	return 0;
}

unsigned Uart0_Close (void)
{
	return 0;
}

#endif
