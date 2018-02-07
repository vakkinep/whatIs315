.data
var1:	.word	0xFFFFFFFF
var2:	.word	0xFFFFFFFF
var3:	.word	0xFFFFFFFF
null:	.word	0

string1: .asciiz "the sly brown fox jumped over the lazy dog."
hex_2_ascii_table:	.asciiz "0123456789ABCDEF"

.text
main:	li 	$a0,	0xABCDEF12
	la	$a1,	var1	#start of our memory we will change
	la	$a2, 	var1	#start of our memory 
	jal	bin2hex		# output to string $a1
	la	$a0, ($a2)	#load og pointer

	#sw	$a0,	var1	# store the value in $a0 in var1
	#la	$a0,	string1	# loads address of string1 into $a0
	li	$v0,	4	#sets syscall to print_string service

	syscall			#prints out string in $a0
	
	li	$v0, 	10
	syscall

	
		
				
bin2hex:
#----------------------------------------
#-
#-	parameters - $a0, $a1 ------
#-		-$a0	32 bit argument to be turned into hex string
#-		-$a1	address we want string stored at
#-
#-
#-	variables - $t3, $t5, $t4, $t0
#-		-$t3	counter value
#-		-$t5	temp pointer to address of lookup table
#-		-$t4	temp value of $a0 we are shifting
#-		-$t0	temp ascii character to output
#-#----------------------------------------

	li	$t3, 8		# counter for the storage of 8 hex digits
	la	$t5, hex_2_ascii_table	# load the address of table in $t5
loop1:		
	srl	$t4, $a0, 28	# take high 4 bits and put it in the bottom of a temporary register
	add	$t4, $t4, $t5	# add to base of table to get corresponding ascii value
	lb 	$t0, ($t4)	# get the corresponding character to output from $t4 into $t0
	sb	$t0, 0($a1)	# store what was in $t0 to address at $a1 which is table[count]
	addi	$a1, $a1, 1	# advance pointer by 1 word
	sll	$a0, $a0, 4	# shift the value at $a0 left 4 bits (1 word)
	addi	$t3, $t3, -1	# decrement counter 
	bne	$t3, $zero, loop1 # if the count is done, leave loop
	sb	$zero, 0($a1)	# store terminating nul char in memory
	jr	$ra		#return
	
	
	#NOTE: Table is not being changed to correct values
	





