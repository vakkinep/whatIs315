.data
newline: .asciiz "\n\nNEW\n\n"

.text
	li 	$v0, 	35		#set syscall to print binary
	li	$a0,	0x6608C000	#Setting the given value into the A0 register for initial print
	syscall				#print the original value in binary
	
	la	$a0,	newline		#print new line
	li 	$v0, 	4		#set to print string
	syscall
	
	and	$a0,	$zero,$a0	#clear that register for the real value
	li	$a1,	0x6608C000	#Setting the given value into the A1 register
	
re:	srl	$a0,	$a1,0x19	#Shifting the first 7 bits all the way to the right (25 bits) and storing in a0
	andi	$a2,	$a1,0x0000C000	#Storing just the values of yyyy in a register and it's in the right spot now
	or	$a0,	$a0,$a2		#Or'ing the value to stack the yyyy with the intial shift. Value stored in a0 again
	
	andi	$a2,	$a1,0x00080000	#Get just the X with all 0's so I can shift in the next step
	srl	$a2,	$a2,0xA		#Shift the x by 11 bits
	or	$a0,	$a0,$a2		#And the x part to the previous final answer to get the result in $a0

	li 	$v0, 	35		#set syscall to print binary
	syscall 	