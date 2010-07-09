#ifndef _VIC_H_
#define _VIC_H_

void VIC_Init (void);
void VIC_Interrupt_Enable (unsigned int VIC_Interrupt_Source);
void VIC_Interrupt_Disable (unsigned int VIC_Interrupt_Source);
void VIC_Install_FIQ_Callback (unsigned int FIQ_Interrupt_Channel, void(*FIQ_Callback)());
void VIC_Install_IRQ_Callback (unsigned int IRQ_Interrupt_Channel, unsigned int IRQ_Interrupt_Slot, void(*IRQ_Callback)());

#endif
