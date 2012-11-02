@ write.S: write sycall wrapper
@
@ Authors: William Westlin <williamawestlin@gmail.com>
@          Martin (Yian) Gao <martin701.gao@gmail.com>
@		   Xinwu Yang <xinwuy@gmail.com>
@ Date:    10/06/2012
#include <bits/swi.h>
	.file	"write.S"
	.text

	.global	write
write:
	@ Needs more code.
	swi	WRITE_SWI
	cmp	r0, #0
	bge	success
	rsb r0, #0
	ldr	r2, =errno
	str	r0, [r2]
	mov	r0, #-1	
success:
	mov	pc, lr
