.data
var1:	.word	0xFFFFFFFF
var2:	.word	0xFFFFFFFF
var3:	.word	0xFFFFFFFF
null:	.word	0

string1: .asciiz "the sly brown fox jumped over the lazy dog."
table:	.asciiz "0123456789ABCDEF"

.text
main:	li 	$a0,	0xABCDEF01
	la	$a1,	table
	jal	bin2hex		# output to string $a1
	

	sw	$a0,	var1
	la	$a0,	string1
	addi	$v0,	$zero,	4	#sets syscall to print_string service

	syscall
	
	li	$v0, 10
	syscall
	
bin2hex:
	li	$t3, 8	# 8 hex digits
	la	$t5, table	# address of table
loop1:		
	srl	$t4, $a0, 28	# take high 4 bits and put it in the bottom
	add	$t4, $t4, $t5	# add to base of table
	lb 	$t0, 0($t4)		# get a character to output
	sb	$t0, 0($a1)
	addi	$a1, $a1, 1	# bump output string pointer
	sll	$a0, $a0, 4	# shift it left 4
	addi	$t3, $t3, -1
	bne	$t3, $zero, loop1
	sb	$zero, 0($a1)	# store terminating nul char
	jr	$ra
	
	





