	.file	1 "pt-initfini.c"
	.section .mdebug.abi32
	.previous
	.gnu_attribute 4, 3
	.abicalls
 #APP
	
#include "defs.h"
	
#if defined __i686 && defined __ASSEMBLER__
	
#undef __i686
	
#define __i686 __i686
	
#endif
	
/*@HEADER_ENDS*/
	
/*@TESTS_BEGIN*/
 #NO_APP
	.text
	.align	2
	.globl	dummy
	.set	nomips16
	.type	dummy, @function
dummy:
	.set	noreorder
	.set	nomacro
	
	beq	$4,$0,$L5
	move	$25,$4

	jr	$25
	nop

$L5:
	j	$31
	nop

	.set	macro
	.set	reorder
 #APP
	
/*@TESTS_END*/
	
/*@_init_PROLOG_BEGINS*/
	.section .init
 #NO_APP
	.section	.init,"ax",@progbits
	.align	2
	.globl	_init
	.set	nomips16
	.type	_init, @function
_init:
	.set	noreorder
	.cpload	$25
	.set	reorder
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	.cprestore	16
	lw	$25,%got(__pthread_initialize_minimal_internal)($28)
	jalr	$25
 #APP
 # 85 "libpthread/nptl/sysdeps/pthread/pt-initfini.c" 1
	ALIGN
 # 0 "" 2
 # 86 "libpthread/nptl/sysdeps/pthread/pt-initfini.c" 1
	END_INIT
 # 0 "" 2
 # 88 "libpthread/nptl/sysdeps/pthread/pt-initfini.c" 1
	
/*@_init_PROLOG_ENDS*/
 # 0 "" 2
 # 89 "libpthread/nptl/sysdeps/pthread/pt-initfini.c" 1
	
/*@_init_EPILOG_BEGINS*/
 # 0 "" 2
 # 90 "libpthread/nptl/sysdeps/pthread/pt-initfini.c" 1
	.section .init
 # 0 "" 2
 #NO_APP
	lw	$31,28($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,32
	.set	macro
	.set	reorder

 #APP
	END_INIT
	
/*@_init_EPILOG_ENDS*/
	
/*@_fini_PROLOG_BEGINS*/
	.section .fini
 #NO_APP
	.section	.fini,"ax",@progbits
	.align	2
	.globl	_fini
	.set	nomips16
	.type	_fini, @function
_fini:
	.set	noreorder
	.cpload	$25
	.set	reorder
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	.cprestore	16
 #APP
 # 105 "libpthread/nptl/sysdeps/pthread/pt-initfini.c" 1
	ALIGN
 # 0 "" 2
 # 106 "libpthread/nptl/sysdeps/pthread/pt-initfini.c" 1
	END_FINI
 # 0 "" 2
 # 107 "libpthread/nptl/sysdeps/pthread/pt-initfini.c" 1
	
/*@_fini_PROLOG_ENDS*/
 # 0 "" 2
 #NO_APP
	lw	$25,%call16(i_am_not_a_leaf)($28)
	jalr	$25
 #APP
 # 118 "libpthread/nptl/sysdeps/pthread/pt-initfini.c" 1
	
/*@_fini_EPILOG_BEGINS*/
 # 0 "" 2
 # 119 "libpthread/nptl/sysdeps/pthread/pt-initfini.c" 1
	.section .fini
 # 0 "" 2
 #NO_APP
	lw	$31,28($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,32
	.set	macro
	.set	reorder

 #APP
	END_FINI
	
/*@_fini_EPILOG_ENDS*/
	
/*@TRAILER_BEGINS*/
	.hidden	__pthread_initialize_minimal_internal
	.ident	"GCC: (Realtek MSDK-4.4.7 Build 2001) 4.4.7"
