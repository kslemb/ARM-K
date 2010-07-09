#include "isr.h"

static inline unsigned __get_cpsr (void)
{
	unsigned long retval;
	asm volatile (" mrs  %0, cpsr" : "=r" (retval) : /* no inputs */  );
	return retval;
}

static inline void __set_cpsr (unsigned val)
{
	asm volatile (" msr  cpsr, %0" : /* no outputs */ : "r" (val)  );
}

unsigned Disable_INT (void)
{
	unsigned _cpsr;

	_cpsr = __get_cpsr();
	__set_cpsr(_cpsr | INT_MASK);
	return _cpsr;
}

unsigned Enable_INT (void)
{
	unsigned _cpsr;

	_cpsr = __get_cpsr();
	__set_cpsr(_cpsr | ~INT_MASK);
	return _cpsr;
}

unsigned Restore_INT (unsigned oldCPSR)
{
	unsigned _cpsr;

	_cpsr = __get_cpsr();
	__set_cpsr((_cpsr & ~INT_MASK) | (oldCPSR & INT_MASK));
	return _cpsr;
}

unsigned Disable_IRQ (void)
{
	unsigned _cpsr;

	_cpsr = __get_cpsr();
	__set_cpsr(_cpsr | IRQ_MASK);
	return _cpsr;
}

unsigned Restore_IRQ (unsigned oldCPSR)
{
	unsigned _cpsr;

	_cpsr = __get_cpsr();
	__set_cpsr((_cpsr & ~IRQ_MASK) | (oldCPSR & IRQ_MASK));
	return _cpsr;
}

unsigned Enable_IRQ (void)
{
	unsigned _cpsr;

	_cpsr = __get_cpsr();
	__set_cpsr(_cpsr & ~IRQ_MASK);
	return _cpsr;
}

unsigned Disable_FIQ (void)
{
	unsigned _cpsr;

	_cpsr = __get_cpsr();
	__set_cpsr(_cpsr | FIQ_MASK);
	return _cpsr;
}

unsigned Restore_FIQ (unsigned oldCPSR)
{
	unsigned _cpsr;

	_cpsr = __get_cpsr();
	__set_cpsr((_cpsr & ~FIQ_MASK) | (oldCPSR & FIQ_MASK));
	return _cpsr;
}

unsigned Enable_FIQ (void)
{
	unsigned _cpsr;

	_cpsr = __get_cpsr();
	__set_cpsr(_cpsr & ~FIQ_MASK);
	return _cpsr;
}


