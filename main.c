#include "main.h"
#include "backlight/backlight.h"
#include "timer0/timer0.h"
#include "vic/isr.h"
#include "vic/vic.h"
#include "uart/uart.h"
#include "syscall/device.h"
#include "shell/shell.h"
#include "enc28j60/enc28j60.h"
#include "lwip_lib/lwip_lib.h"

#ifdef USE_NEWLIB_PRINTF
#include <stdio.h>
#endif
#ifdef USE_INTERNAL_PRINTF
#include "generic/console.h"
#endif

void CPU_Hardware_Init (void);
//const char Banner[]="Hello world!!!\n\r";

int main (void)
{
 int Level, i;
 unsigned int CurrentTick, lwIPTimersCntr = 0;

	for (i = 0; i < 10; i++) {
	}
 /////////////////////////////////////// <- Debug Start
// ETH_Rev ENC_RevID;
 /////////////////////////////////////// <- Debug End

	MEMMAP = MEMMAP_UFL;				/* Vectors in Flash */
	CPU_Hardware_Init ();				/* Init CPU */
	VIC_Init ();						/* Init VIC */

	console_assign (STDIN_FILENO, "uart0");
	console_assign (STDOUT_FILENO, "uart0");

	BackLightInit ();					/* Init LCD backlight */
	Timer0Init ();						/* Init Timer0 */
	Uart0_Init ();						/* Init Uart0*/
	Timer0_Enable ();					/* Enable Timer0 operation */
////////////////////////////////////
	Enable_IRQ ();						/* Enable Interrupts */
#ifdef TIMER0_INSTALL_AS_FIQ
	Enable_FIQ ();						/* Enable Interrupts */
#endif
////////////////////////////////////
	printf ("Starting lwIP stack...\n\r");
	BackLightLewel (100);

    /*Init lwIP TCP/IP stack (IP address, Net Mask, Gate Address, Enable DHCP, Enable Auto IP)*/
    /*192.168.1.26 = 0xC0A8011A, 255.255.255.0 = 0xFFFFFF00, 192.168.1.1 = 0xC0A80101, 0x0, 0x0 */
	init_lwip_stack (0xC0A8011A, 0xFFFFFF00, 0xC0A80101, 0x0, 0x0);

//	Enable_IRQ ();						/* Enable Interrupts */

/////////////////////////////////////// <- Debug Start
//    enc28j60_open ();
//    enc28j60_Get_RevID (&ENC_RevID);
//    printf ("ENC28J60 Silicon Rev: 0x%x\n\r",ENC_RevID.DEV_RevID);
//    printf ("PHY Rev: 0x%x, Ident: 0x%x, PN: 0x%x\n\r", ENC_RevID.PHI_RevID, ENC_RevID.PHY_Ident, ENC_RevID.PHY_PartNumber);
/////////////////////////////////////// <- Debug End

#ifdef TIMER0_INSTALL_AS_FIQ
//	Enable_FIQ ();						/* Enable Interrupts */
#endif
	printf ("Starting Application\n\r");

	while (1) {
		CurrentTick = TickGet ();

		if (TickGetDiff (CurrentTick, lwIPTimersCntr) >= TICKS_IN_MS(10)) {
			/* Service lwIP timers */
			lwip_process_timers ();
			/* Reload current tick to the service lwip timers counter */
			lwIPTimersCntr = CurrentTick;
		}

		/* Check if packet received into the ENC28J60 internal buffer */
		if (enc28j60_Check_If_Packet_Received() > 0) {
			/* Process input packet */
			ethernetif_input(&enc28j60_netif);
		}
	}

#if 0

	shell ();

	while (1) {
		Level = 0;
		while(Level <= 100) {
			BackLightLewel (Level++);
			i = TickGet ();
			while (TickGetDiff (TickGet (), i) < TICKS_IN_MS(50))
				;
		}
		do {
			BackLightLewel (--Level);
			i = TickGet ();
			while (TickGetDiff (TickGet (), i) < TICKS_IN_MS(50))
				;
		}
		while (Level);

		while (TickGetDiff (TickGet (), i) < TICKS_IN_MS(2000))
				;
		printf("Current Tick: %d\n\r", TickGet ());
	}
#endif

#if 0
	while (1) {
		for (Level = 0; Level < 100; Level++) {
			BackLightLewel (Level);
			for (i=0; i < (250000 + (100 - Level) * 1000); i++);
		}

		for (Level = 100; Level != -1; Level--) {
			BackLightLewel (Level);
			for(i = 0; i < 150000; i++);
		}

		for(i = 0; i < 10000000; i++);
	}
#endif
	return 0;
}

void CPU_Hardware_Init (void)
{
	/* Disable power to the peripheral modules */
	PCONP = PCALLOFF;
    /* Init Memory Acceleration Module: Enable and set MAM Fetch Cycles */
	MAMTIM = MAMTIM_3CLK;
	MAMCR = MAMCR_FULL;

	/*  Setup CCLK to 60MHz based on 12Mhz Fosc: multiply XTAL input by 5, divide by 1 */
	PLLCFG0 = (PLLCFG_MUL5 | PLLCFG_DIV1);
	PLLCON0 = PLLCON_PLLE;
	PLLFEED0 = PLLFEED_FEED1;
	PLLFEED0 = PLLFEED_FEED2;

	/* Wait for the PLL to lock */
	while (!(PLLSTAT0 & PLLSTAT_PLOCK))
		;
	/* Connect PLL as the clock source */
	PLLCON0 = PLLCON_PLLC | PLLCON_PLLE;
	PLLFEED0 = PLLFEED_FEED1;
	PLLFEED0 = PLLFEED_FEED2;

	/* VPB bus clock is the same as the processor clock. Fpclk = Fcclk */
	VPBDIV = VPBDIV_100;

	/* Use fast I/O on P0 and P1 */
	SCS = SCS_GPIO0M | SCS_GPIO1M;

	/*  All CPU pins configured as GPIO, excluding JTAG port (P1.26...P1.31) */
	PINSEL0 = PINSEL0_ALL_GPIO;
	PINSEL1 = PINSEL1_ALL_GPIO;
	PINSEL2 = (PINSEL2_P13126_DEBUG | PINSEL2_P12516_GPIO);

	/* Set all GPIO to input  */
	GPIO0_FIODIR = ~GPIO_IO_ALL;
	GPIO1_FIODIR = ~GPIO_IO_JTAG;
}
