	.text

	.extern main
	.extern __stack_end__
	.extern __bss_start__
	.extern __bss_end__

	.globl _entry
	.set noreorder

_entry:
	// Clear bss
	la	$8, __bss_start__
	la	$9, __bss_end__
_bss:
	sw	$0, ($8)
	addi	$8, $8, 4
	bne	$8, $9, _bss
	nop

	// Setup stack, jump to C code
	la	$29, __stack_end__
	j	main
	nop

	.set reorder
