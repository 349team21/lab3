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
#define SWI_IMM_MASK 0x00ffffff
#define CPSR_IRQ_FLAG  0x80
#define CPSR_FIQ_FLAG  0x40
#define CPSR_USR_MODE  0x10

#define LOAD_ADDR 0xa0000000
#define USR_STACK 0xa3000000
#define IRQ_STACK 0xa2000000

	.file   "start.S"
	.text
	.global handleSWI
	.type handleSWI, %function
	.global toUSER
	.type toUSER, %function
	.global handleIRQ
	.type handleIRQ, %function
	.global exit_kernel
	.type exit_kernel, %function
	
	/*
	 * Branch immediately to main without altering the execution environment.
	 * This indirection is needed to guarantee that the program entry point is
	 * LOAD_ADDR (probably 0xa3000000) without placing strict
	 * requirements on the layout of kernel.c.  Bit first, provide main with the
	 * jump table address as one of its arguments.
	 */
FUNC(_start)
	mov     r2, r8
	b       kmain

handleSWI:
	stmfd	sp!, {r0-r12, lr}

	@ Restore jump table in r8.
	ldr	ip, =global_data
	ldr	r8, [ip]

	@ Determine the SWI number and call the dispatch function.
	ldr	r0, [lr, #-4]
	bic	r0, r0, #~SWI_IMM_MASK
	mov	r1, sp
	bl	my_swi_dispatcher

	@ Return to user mode.
	add		sp, sp, #4
	ldmfd	sp!, {r1-r12, pc}^

handleIRQ:
	@ correct lr to the real return address
	sub 	lr, lr, #4
	
	ldr 	sp, =IRQ_STACK
	stmfd	sp!, {r0-r12, lr}

	@ Restore jump table in r8.
	ldr	ip, =global_data
	ldr	r8, [ip]

	bl	interrupt

	@ Return to user mode.
	ldmfd	sp!, {r0-r12, pc}^

toUSER:	
    @ Stash regs, jump table in r8, and sp for exit_kernel.
	stmfd	sp!, {r4-r7, r9-r11, lr}
	ldr	ip, =global_data
	str	r8, [ip]
	ldr	ip, =saved_sp
	str	sp, [ip]

	@ Switch to user mode & load user stack.
	msr	cpsr_c, #(CPSR_FIQ_FLAG | CPSR_USR_MODE)
	ldr	sp, =USR_STACK

	@ Push argv NULL terminator.
	mov	ip, $0
	str	ip, [sp, #-4]!

	@ Calculate address of argv[argc].
	add	r1, r1, r0, asl #2
	mov	r2, r0

	@ Push argv pointers from argv[argc-1] .. argv[0].
copy_loop:
	cmp	r2, $0
	ldrgt	ip, [r1, #-4]!
	strgt	ip, [sp, #-4]!
	subgt	r2, r2, #1
	bgt	copy_loop

	@ Push argc.
	str	r0, [sp, #-4]!

	@ Jump to program.
	ldr	pc, =LOAD_ADDR
	


	@ Exits from the enter_user_mode function by restoring the values
	@ saved on the stack there.
exit_kernel:
	@ Restore regs & enter_user_mode sp (pops SWI stack junk).
	ldr	ip, =saved_sp
	ldr	sp, [ip]
	ldmfd	sp!, {r4-r7, r9-r11, pc}

	.comm	saved_sp, 4, 4
	
