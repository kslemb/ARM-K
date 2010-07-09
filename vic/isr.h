#ifndef _ISR_H_
#define _ISR_H_

#define IRQ_MASK	0x00000080
#define FIQ_MASK	0x00000040
#define INT_MASK	(IRQ_MASK | FIQ_MASK)

unsigned Disable_INT (void);
unsigned Enable_INT (void);
unsigned Restore_INT (unsigned oldCPSR);

unsigned Disable_IRQ (void);
unsigned Restore_IRQ (unsigned oldCPSR);
unsigned Enable_IRQ (void);

unsigned Disable_FIQ (void);
unsigned Restore_FIQ (unsigned oldCPSR);
unsigned Enable_FIQ (void);

#define ISR_ENTRY() asm volatile(" sub   lr, lr,#4\n" \
                                 " stmfd sp!,{r0-r12,lr}\n" \
                                 " mrs   r1, spsr\n" \
                                 " stmfd sp!,{r1}")

#define ISR_EXIT()  asm volatile(" ldmfd sp!,{r1}\n" \
                                 " msr   spsr_c,r1\n" \
                                 " ldmfd sp!,{r0-r12,pc}^")


#endif
