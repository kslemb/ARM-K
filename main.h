#ifndef _MAIN_H_
#define _MAIN_H_

#include "lpc214x.h"

/* Use our internal printf implementation insted of newlib printf */
/* This will decrease the amount of used flash memory and increase performance */
//#define USE_INTERNAL_PRINTF

#ifndef USE_INTERNAL_PRINTF
/* If we do not defined internal printf */
/* use the newlib printf prototype */
#define USE_NEWLIB_PRINTF
#endif

#ifdef USE_INTERNAL_PRINTF
/* Define printf buffer size for our internal printf implementation */
#define PRINTF_BUFFER_SIZE      64
/* Use internal functions for printf */
#define printf      internal_printf
#define fprintf     internal_fprintf
#endif

/* Define our CPU clock, asume that VPB bus clock is same as CPU clock */
#define	CPU_CLOCK_HZ		60000000	/* 60MHz = 12MHz main quartz multiple for '5' by PLL0 */

/* Define Timer0 tick rate in Hz */
#define	TIMER0_TICK_RATE_HZ	100			/* Timer0 period is 10ms */

/* Return amount of Timer0 Ticks equal to 'ms' variable. 'ms' value must be in milliseconds*/
#define TICKS_IN_MS(ms) ((TIMER0_TICK_RATE_HZ * ms) / 1000)

/* Enable Uart0 */
#define ENABLE_UART0	1

/* Enable SPI1 */
#define ENABLE_SPI1     1

/* Enable I2C0 */
#define ENABLE_I2C0		1

/* Enable EEPROM Support */
#define ENABLE_EEPROM	1

/* Enable I2C0 if EEPROM is enabled */
#ifdef ENABLE_EEPROM
#define ENABLE_I2C0		1
#endif

/* Define interrupt used by Timer0: FIQ or IRQ */
#define TIMER0_INSTALL_AS_FIQ
/*#define TIMER0_INSTALL_AS_IRQ*/

/* Define interrupt slots used by LPC internal peripheral */
#define INTERRUPT_SLOT_TIMER0	VIC_Slot0
#define INTERRUPT_SLOT_UART0	VIC_Slot3

#endif
