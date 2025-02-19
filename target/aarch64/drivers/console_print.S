.equ CONSOLE_UART_BASE, 0xfe660000

	.globl	asm_print_str
	.globl	asm_print_hex
	.globl	asm_print_hex_bits
	.globl	asm_print_newline
	.globl  console_putc

/* The offset to add to get ascii for numerals '0 - 9' */
#define ASCII_OFFSET_NUM	0x30

	/*
	 * This macro is intended to be used to print the
	 * line number in decimal. Used by asm_assert macro.
	 * The max number expected is 65536.
	 * In: x4 = the decimal to print.
	 * Clobber: x30, x0, x1, x2, x5, x6
	 */
	.macro asm_print_line_dec
	mov	x6, #10		/* Divide by 10 after every loop iteration */
	mov	x5, #MAX_DEC_DIVISOR
dec_print_loop:
	udiv	x0, x4, x5			/* Get the quotient */
	msub	x4, x0, x5, x4			/* Find the remainder */
	add	x0, x0, #ASCII_OFFSET_NUM	/* Convert to ascii */
	bl	console_putc
	udiv	x5, x5, x6			/* Reduce divisor */
	cbnz	x5, dec_print_loop
	.endm

/*
 * This function prints a string from address in x4.
 * In: x4 = pointer to string.
 * Clobber: x30, x0, x1, x2, x3
 */
asm_print_str:
	mov	x3, x30
1:
	ldrb	w0, [x4], #0x1
	cbz	x0, 2f
	bl	console_putc
	b	1b
2:
	ret	x3

/*
 * This function prints a hexadecimal number in x4.
 * In: x4 = the hexadecimal to print.
 * Clobber: x30, x0 - x3, x5
 */
asm_print_hex:
	mov	x5, #64  /* No of bits to convert to ascii */

	/* Convert to ascii number of bits in x5 */
asm_print_hex_bits:
	mov	x3, x30
1:
	sub	x5, x5, #4
	lsrv	x0, x4, x5
	and	x0, x0, #0xf
	cmp	x0, #0xA
	b.lo	2f
	/* Add by 0x27 in addition to ASCII_OFFSET_NUM
	 * to get ascii for characters 'a - f'.
	 */
	add	x0, x0, #0x27
2:
	add	x0, x0, #ASCII_OFFSET_NUM
	bl	console_putc
	cbnz	x5, 1b
	ret	x3

/*
 * Helper function to print newline to console
 * Clobber: x0
 */
asm_print_newline:
	mov	x0, '\n'
	b	console_putc


	/* --------------------------------------------------------
	 * int console_pl011_core_putc(int c, uintptr_t base_addr)
	 * Function to output a character over the console. It
	 * returns the character printed on success or -1 on error.
	 * In : w0 - character to be printed
	 * Out : return -1 on error else return character.
	 * Clobber list : x1, x2
	 * --------------------------------------------------------
	 */
//.equ PL011_UARTFR_TXFF_BIT, 5	/* Transmit FIFO full bit in UARTFR register */
//.equ PL011_UARTFR_RXFE_BIT, 4	/* Receive FIFO empty bit in UARTFR register */
//.equ PL011_UARTFR_BUSY_BIT, 3	/* UART busy bit in UARTFR register */
//.equ PL011_UARTDR, 0x000
//.equ PL011_UARTFR, 0x018

#define PL011_UARTFR_TXFF_BIT 5
#define PL011_UARTFR_RXFE_BIT 4
#define PL011_UARTFR_BUSY_BIT 3
#define PL011_UARTDR 0x000
#define PL011_UARTFR 0x018


console_putc:
	ldr	x1, =CONSOLE_UART_BASE

	/* Prepend '\r' to '\n' */
	cmp	w0, #0xA
	b.ne	2f
1:
	/* Check if the transmit FIFO is full */
	ldr	w2, [x1, #PL011_UARTFR]
	tbnz	w2, #PL011_UARTFR_TXFF_BIT, 1b
	mov	w2, #0xD
	str	w2, [x1, #PL011_UARTDR]
2:
	/* Check if the transmit FIFO is full */
	ldr	w2, [x1, #PL011_UARTFR]
	tbnz	w2, #PL011_UARTFR_TXFF_BIT, 2b
	str	w0, [x1, #PL011_UARTDR]
	ret


