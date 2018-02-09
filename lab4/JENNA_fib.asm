#fibonacci value function
.data

var1:	.word	0
var2:	.word	1
var3:	.word	5
var4:	.word	10
var5:	.word	20
result:	.word	0

.text


main:
	# Solve for var1
	lw 	$a1, var1	#solve for var1
	jal	fib		#jump to fibonacci function
	
	#Display var1 Result
	sw	$v1, result
	lw	$a0, result	#loads result into $a0
	li	$v0,	1	#sets syscall to print_int service
	syscall			#prints out number in $a0
	
	#Exit
	li	$v0, 	10	#exit
	syscall


#------------------------------------------------------------
#-
#- 	Parameters: 
#-		- $a1: variable we want to solve for
#-		- $v1: answer to fibonacci sequence
#-
#-	Variables:
#-		- $v1: result
#-		- $ra: return address
#-		- $sp: stack pointer
#-		- $s0: sum
#-
#----------------------------------------------------------

fib:	
	#store local variables on stack
	add	$sp, $sp, -12	#space for three values
	sw	$ra, 0($sp)	#store return address
	sw	$s0, 4($sp)	#store local variable
	sw	$s1, 8($sp)
	
	#Base Case
	li $v1 1
	beq $a0, 0, endFib 	#argument = 0
	beq $a0, 1, endFib 	#argument = 1
	
	#fib(n-1)
	add $a0, $a0, -1
	move $s0, $a0
	jal fib
	
	#fib(n-2)
	add $a0, $a0, -1
	move $s1, $a0
	jal fib
	
	#Sum
	add $v1, $s0, $v1
	add $v1, $s1, $v1
	
	endFib:
		lw 	$ra, ($sp) 	#load return address from stack
		lw 	$s0, 4($sp)	#load local variable from stack
		lw	$s1, 8($sp)
		add	$sp, $sp, 12	#restore stack
		jr	$ra		#return
