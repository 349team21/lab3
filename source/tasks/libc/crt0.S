@ crt0.S: Execution startup routine
@
@ Authors: William Westlin <williamawestlin@gmail.com>
@          Martin (Yian) Gao <martin701.gao@gmail.com>
@		   Xinwu Yang <xinwuy@gmail.com>
@ Date:    10/06/2012

#include <asm.h>
#include <bits/swi.h>

	.file	"crt0.S"
	.text

	.global	_start
_start:
	ldr r0, [sp]
	ldr r1, [sp, #4]
	bl	main
	swi	EXIT_SWI
