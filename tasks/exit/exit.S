@ exit.S: exit sycall wrapper
@
@ Authors: William Westlin <williamawestlin@gmail.com>
@          Martin (Yian) Gao <martin701.gao@gmail.com>
@		   Xinwu Yang <xinwuy@gmail.com>
@ Date:    10/06/2012
#include <bits/swi.h>
	.file	"exit.S"
	.text

	.global	_start
_start:
	swi	EXIT_SWI
	mov	r0, #42
