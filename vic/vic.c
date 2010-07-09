#include "../main.h"
#include "vic.h"

void (*fiq_function)();

void FIQ_Routine (void)   __attribute__ ((interrupt("FIQ"))) __attribute__ ((noinline));
void FIQ_Routine (void) {
	/* Call FIQ callback function */
	(*fiq_function)();
}


void IRQ_Routine (void)   __attribute__ ((interrupt("IRQ"))) __attribute__ ((noinline));
void IRQ_Routine (void) {
	void (*irq_function)();

	/* Get signaled interrupt vector */
	irq_function = (void(*)())(VIC_VectAddr);
	/* Call vectored interrupt function */
	(*irq_function)();
	/* Clear interrupt in VIC */
	VIC_VectAddr = 0;
}

void VIC_Default_IRQ (void) {
	GPIO0_FIODIR3 |= 0x80;
	GPIO0_FIOCLR3 |= 0x80;
}

void VIC_Install_FIQ_Callback (unsigned int FIQ_Interrupt_Channel, void(*FIQ_Callback)())
{
	/* Select "Interrupt_Source" interrupt as FIQ */
	VIC_IntSelect |= (1 << FIQ_Interrupt_Channel);
    /* Install FIQ CallBack function */
	fiq_function = FIQ_Callback;
}

void VIC_Install_IRQ_Callback (unsigned int IRQ_Interrupt_Channel, unsigned int IRQ_Interrupt_Slot, void(*IRQ_Callback)())
{
	unsigned int volatile *ptrReg_Base;

	/* Load base address of vectored address registers */
	ptrReg_Base = &VIC_VectAddr0;
	/* Set Address of callback function to corresponding Slot */
	*(ptrReg_Base + IRQ_Interrupt_Slot) = (unsigned int)IRQ_Callback;
	/* load base address of ctrl registers */
	ptrReg_Base = &VIC_VectCntl0;
	/* Set source chanel and enable the slot */
	*(ptrReg_Base + IRQ_Interrupt_Slot) = IRQ_Interrupt_Channel | VIC_VectCntl_ENABLE;
	/* Clear FIQ select bit */
	VIC_IntSelect &= ~(1 << IRQ_Interrupt_Channel);
}

void VIC_Interrupt_Enable (unsigned int VIC_Interrupt_Source)
{
	/* Enable "VIC_Interrupt_Source" in VIC */
	VIC_IntEnable |= VIC_Interrupt_Source;
}

void VIC_Interrupt_Disable (unsigned int VIC_Interrupt_Source)
{
	/* Disable "VIC_Interrupt_Source" in VIC */
	VIC_IntEnClr |= VIC_Interrupt_Source;
}


void VIC_Init (void)
{
	/* Assign all interrupt chanels to IRQ */
	VIC_IntSelect  =  0;
	/* Diasable interrupts */
	VIC_IntEnClr = 0xFFFFFFFF;
	/* Clear software interrutps */
	VIC_SoftIntClr = 0xFFFFFFFF;
	/* VIC registers accessed in User/Privileged mode */
	VIC_Protection = 0;
	/* Clear interrupt */
	VIC_VectAddr = 0;
	/* Set Default ISR address for non-vectored IRQ to VIC_Default_IRQ function - detect spurious interrupts*/
	VIC_DefVectAddr = (unsigned int) (VIC_Default_IRQ);
	/* Clear ISR address for vectored IRQ */
	VIC_VectAddr0 = VIC_VectAddr1 = VIC_VectAddr2 = VIC_VectAddr3 = \
	VIC_VectAddr4 = VIC_VectAddr5 = VIC_VectAddr6 = VIC_VectAddr7 = \
	VIC_VectAddr8 = VIC_VectAddr9 = VIC_VectAddr10 = VIC_VectAddr11 = \
	VIC_VectAddr12 = VIC_VectAddr13 = VIC_VectAddr14 = VIC_VectAddr15 = 0;

	/* Disable all vectored IRQ */
	VIC_VectCntl0 = VIC_VectCntl1 = VIC_VectCntl2 = VIC_VectCntl3 = \
	VIC_VectCntl4 = VIC_VectCntl5 = VIC_VectCntl6 = VIC_VectCntl7 = \
	VIC_VectCntl8 = VIC_VectCntl9 = VIC_VectCntl10 = VIC_VectCntl11 = \
	VIC_VectCntl12 = VIC_VectCntl13 = VIC_VectCntl14 = VIC_VectCntl15 = 0;
}
