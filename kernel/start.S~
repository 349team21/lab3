/**
 * @file  start.S
 *
 * @brief Kernel startup routine
 *
 * @author Mike Kasick <mkasick@andrew.cmu.edu>
 * @date   Tue, 23 Oct 2007 11:20:33 -0400
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date   2008-07-06
 */

#include <asm.h>

	.file   "start.S"
	.text
	.global handleSWI
	.type handleSWI, %function
	.global toUSER
	.type toUSER, %function
	.global handleIRQ
	.type handleIRQ, %function

	/*
	 * Branch immediately to main without altering the execution environment.
	 * This indirection is needed to guarantee that the program entry point is
	 * LOAD_ADDR (probably 0xa3000000) without placing strict
	 * requirements on the layout of kernel.c.  Bit first, provide main with the
	 * jump table address as one of its arguments.
	 */
FUNC(_start)
	stmfd sp!, {r1-r12}
	ldr r11, =svc_stack_register
	str sp, [r11]
	ldr r11, =svc_link_register
	str lr, [r11]
	mov     r2, r8
	b       kmain

handleSWI:
	@taken from lecture slides
	sub 	sp, sp, #4 
	stmfd 	sp!, {r0-r12, lr}
	mrs 	r2, spsr
	str 	r2, [sp, #14*4]
	mov 	r1, sp 
	ldr 	r0, [lr, #-4] 
	bic 	r0, r0, #0xff000000
	bl	my_swi_dispatcher
	ldr 	r2, [sp, #14*4]
	msr 	spsr, r2
	add	sp, sp, #4
	ldmfd 	sp!, {r1-r12, lr} 
	add 	sp, sp, #4 
	movs 	pc, lr

handleIRQ:
	bl temp_int
	mov pc, lr

toUSER:	
    mrs r2, cpsr
    bic r2, r2, #0xed  
	@ b00010010
    msr cpsr, r2
	@switch to irq mode and mask IRQ/FIQ
    ldr sp, =0xa1000000
    ldr sl, =0xa0000000
	@setup irq stack
    mrs r2, cpsr
    bic r2, r2, #0x2f  
	@ b11010000
    msr cpsr, r2
	@switch to user mode and enable IRQ/FIQ
    ldr sp, =0xa3000000
    ldr sl, =0xa1000000
	@setup user stack
    stmfd sp!, {r0, r1}
	@push user program arguments
    ldr r2, =0xa0000000
	@jump to user program
    mov lr, pc
    mov pc, r2
	@return

