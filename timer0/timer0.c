#include "../main.h"
#include "../vic/vic.h"
#include "timer0.h"

volatile unsigned int TickCount;


#ifdef ENABLE_TIMER0_CALLBACK
/* Pointer for the timer callback function */
void (*timer_function)();
#endif

inline void Timer0_ISR (void)
{
	/* Increment Tick value */
	TickCount++;
	/* Clear all Timer0 Interrupt lines */
	T0IR = TIR_MASK;

#ifdef ENABLE_TIMER0_CALLBACK
	/* Call timer callback function */
	if (timer_function)
		(*timer_function)();
#endif
}

void Timer0Init (void)
{
#ifdef ENABLE_TIMER0_CALLBACK
	/* Reset pointer to the Timer0 callback function */
	timer_function = 0x0;
#endif
	/* Reset Tick counter */
	TickCount = 0;
#if defined TIMER0_INSTALL_AS_FIQ
	/* Install Timer0 as FIQ interrupt */
	VIC_Install_FIQ_Callback (VIC_Channel_Timer0, Timer0_ISR);
	VIC_Interrupt_Enable (VIC_Int_Sorce_Timer0);
#else
#if defined TIMER0_INSTALL_AS_IRQ
	/* Install Timer0 as IRQ interrupt */
	VIC_Install_IRQ_Callback (VIC_Channel_Timer0, INTERRUPT_SLOT_TIMER0, Timer0_ISR);
	VIC_Interrupt_Enable (VIC_Int_Sorce_Timer0);
#else
	#error "You must specify Timer0 Interrupt installation as FIQ or IRQ"
#endif
#endif
	/* Enable Power for Timer0 */
	PCONP |= PCTIM0;
	/* Reset and disable Timer0 */
	T0TCR = TCR_CR;
	/* Set Prescaler to no division */
	T0PC = 0;
	/* Count up to this value. Generate TIMER0_TICK_RATE numbers of interrupts in 1sec*/
	T0MR0 = (CPU_CLOCK_HZ / TIMER0_TICK_RATE_HZ);
	/* Reset timer and interrupt on MR0 (match register 0) */
	T0MCR = TMCR_MR0I | TMCR_MR0R;
	/* Capture is disabled */
	T0CCR = 0;
	/* No external match output */
	T0EMR = 0;
}

void Timer0_Disbale (void)
{
	/* Reset and disable Timer0 */
	T0TCR = TCR_CR;
}

void Timer0_Enable (void)
{
	/* Enable timer 0 */
	T0TCR = TCR_CE;
}

unsigned int TickGet (void)
{
	return TickCount;
}

unsigned int TickGetDiff (unsigned int CurrentTick, unsigned int StartTick)
{
	return (CurrentTick < StartTick) ? ((0xFFFFFFFF - StartTick) + CurrentTick) : (CurrentTick - StartTick);
}
