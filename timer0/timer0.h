#ifndef _TIMER0_H_
#define _TIMER0_H_

/* Pointer for the timer callback function */
extern void (*timer_function)();

void Timer0Init (void);
void Timer0_Disbale (void);
void Timer0_Enable (void);

unsigned int TickGet (void);
unsigned int TickGetDiff (unsigned int CurrentTick, unsigned int StartTick);

#endif
