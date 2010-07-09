/*
 ARM Startup Module
*/


	/* Stack Sizes */
	.set  UND_STACK_SIZE, 0x00000004	/* stack for "undefined instruction" interrupts is 4 bytes  */
	.set  ABT_STACK_SIZE, 0x00000004	/* stack for "abort" interrupts is 4 bytes                  */
	.set  FIQ_STACK_SIZE, 0x00000020	/* stack for "FIQ" interrupts  is 32 bytes         			*/
	.set  IRQ_STACK_SIZE, 0X00000080	/* stack for "IRQ" normal interrupts is 128 bytes  			*/
	.set  SVC_STACK_SIZE, 0x00000004	/* stack for "SVC" supervisor mode is 4 bytes  				*/



/* Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs (program status registers) */

	.equ  MODE_USR, 0x10           		/* Normal User Mode 										*/
	.equ  MODE_FIQ, 0x11           		/* FIQ Processing Fast Interrupts Mode 						*/
	.equ  MODE_IRQ, 0x12           		/* IRQ Processing Standard Interrupts Mode 					*/
	.equ  MODE_SVC, 0x13           		/* Supervisor Processing Software Interrupts Mode 			*/
	.equ  MODE_ABT, 0x17           		/* Abort Processing memory Faults Mode 						*/
	.equ  MODE_UND, 0x1B           		/* Undefined Processing Undefined Instructions Mode 		*/
	.equ  MODE_SYS, 0x1F           		/* System Running Priviledged Operating System Tasks  Mode	*/

	.equ  I_BIT, 0x80              		/* when I bit is set, IRQ is disabled (program status registers) */
	.equ  F_BIT, 0x40              		/* when F bit is set, FIQ is disabled (program status registers) */



	.section    .reset, "ax"
	.global	__reset
	.code 32

__reset:

# Exception Vectors table

	ldr     PC, Reset_Addr
	ldr     PC, Undef_Addr
	ldr     PC, SWI_Addr
	ldr     PC, PAbt_Addr
	ldr     PC, DAbt_Addr
	nop								/* Reserved Vector (holds Philips ISP checksum) */
	ldr     PC, IRQ_Addr			/*[PC,#-0xFF0]*/		/* Loads PC with the address that is present in VICVectAddr register. */
	ldr     PC, FIQ_Addr

Reset_Addr:     .word   Reset_Handler		/* defined in this module below  */
Undef_Addr:     .word   UNDEF_Routine
SWI_Addr:       .word   SWI_Routine
PAbt_Addr:      .word	pabort_handler
DAbt_Addr:      .word	dabort_handler
                .word   0x00
IRQ_Addr:       .word   IRQ_Routine
FIQ_Addr:       .word   FIQ_Routine


	.text
	.code 32
	.align 0
Reset_Handler:				/* After Reset we start here jump to the setup place */
	b _start
UNDEF_Routine:				/* Currently We don't use this */
	b UNDEF_Routine
SWI_Routine:				/* Currently We don't use this */
	b SWI_Routine
pabort_handler:				/* Currently We don't use this */
	b pabort_handler
dabort_handler:				/* Currently We don't use this */
	b pabort_handler

/* Start here */
/*	.extern Hardware_Setup */

_start:
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
mam_init:
    ldr	r0, _MAMBASE
    mov	r1, #_MAMCR_VALUE
    str r1, [r0, #_MAMCR_OFFSET]

    mov	r1, #_MAMTIM_VALUE
    str r1, [r0, #_MAMTIM_OFFSET]


/*
	 ldr   r0,IODIR
	 ldr   r1,IOSET
     mov   r2, #P0_21
     str   r2,[r0]
     str   r2,[r1]
Stop:			b Stop
*/
_MAMBASE:        .word   0xE01FC000
IODIR:          .word   0xe0028008
IOSET:          .word   0xe0028004

/* MAM configuration */
.equ _MAMCR_OFFSET,   0x0
.equ _MAMTIM_OFFSET,  0x4

.equ _MAMCR_VALUE,    0x2  /* fully enabled */
.equ _MAMTIM_VALUE,   0x3  /* fetch cycles  */

.equ P0_21,  (1 << 21)

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* Perform some Hardware Init */
/*	ldr r0,=Hardware_Setup
	mov lr,pc
	bx r0
*/
	/* Init Stack for each mode */
	ldr	r0, =_stack_end
	msr	CPSR_c, #MODE_UND|I_BIT|F_BIT 	/* Undefined Instruction Mode  */
	mov	sp, r0
	sub	r0, r0, #UND_STACK_SIZE
	msr	CPSR_c, #MODE_ABT|I_BIT|F_BIT 	/* Abort Mode */
	mov	sp, r0
	sub	r0, r0, #ABT_STACK_SIZE
	msr	CPSR_c, #MODE_FIQ|I_BIT|F_BIT 	/* FIQ Mode */
	mov	sp, r0
	sub	r0, r0, #FIQ_STACK_SIZE
	msr	CPSR_c, #MODE_IRQ|I_BIT|F_BIT 	/* IRQ Mode */
	mov	sp, r0
	sub	r0, r0, #IRQ_STACK_SIZE
	msr	CPSR_c, #MODE_SVC|I_BIT|F_BIT 	/* Supervisor Mode */
	mov	sp, r0
	sub	r0, r0, #SVC_STACK_SIZE
	msr	CPSR_c, #MODE_SYS|I_BIT|F_BIT 	/* User Mode */
	mov	sp, r0				/* Stay in user mode */

	.extern	_etext
	.extern	_data
	.extern	_edata

	/* copy .data section (Copy from ROM to RAM) */
	ldr	R1, =_etext
	ldr     R2, =_data
	ldr     R3, =_edata
1:	cmp     R2, R3
	ldrlo   R0, [R1], #4
	strlo   R0, [R2], #4
	blo     1b

	.extern	__bss_start__
	.extern	__bss_end__

	/* Clear .bss section (Zero init)  */
	mov     R0, #0
	ldr     R1, =__bss_start__
	ldr     R2, =__bss_end__
2:	cmp     R1, R2
	strlo   R0, [R1], #4
	blo     2b

	.extern	main

	/* Enter the C code  */
	ldr	r0,=main
	mov 	pc, r0

	/* If we return from main stay in loop forever */
Exit_main:
	b	Exit_main

.end
