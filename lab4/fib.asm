#fibonacci value function
.data

var1:	.word	0
var2:	.word	1
var3:	.word	5
var4:	.word	10
var5:	.word	20
result:	.word	0

start: 	.asciiz "Fibonacci values for the number: "
newline:.asciiz "\n\n"
space:	.asciiz " "
.text


main:
	la 	$a0, start	#print out intro string
	li 	$v0,	4
	syscall 
	lw 	$a0, var3	#print out the final number
	li 	$v0, 1		
	syscall
	la 	$a0, newline	#print out newline string
	li 	$v0,	4
	syscall 
	
	lw 	$a0, var3	#place the number to solve in $a0
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
#-		- $a0: variable we want to solve for
#-		- $v1: answer to fibonacci sequence
#-
#-	Variables:
#-		- $v1: result
#-		- $ra: return address
#-		- $sp: stack pointer
#-		- $s0: temp sum
#-		- $s1: temp variable to solve for
#-	Return: 
#-	VOID - 	Prints out the values of the fib sequence
#-		up to the end value and prints that too
#----------------------------------------------------------

fib:	
	#store local variables on stack
	add	$sp, $sp, -12	#space for three values
	sw	$ra, 0($sp)	#store return address
	sw	$s0, 4($sp)	#store local variable
	sw	$s1, 8($sp)
	
	move 	$s0, $a0		#store $a0 into $s1
	
	#Base Case
	beq 	$a0, 0, endFib	 	#argument = 0
	#add	$v1, $v1, 1
	beq 	$a0, 1, endFib1 	#argument = 1
	
	#fib(n-1)
	#add 	$a0, $a0, -1
	add 	$a0, $s0, -1
	jal 	fib
	
	#fib(n-2)
	add 	$a0, $s0, -2
	jal 	fib
	
	#Sum
	move	$v0, $v1		#save into another register
	add 	$v1, $s1, $v1
	#add $v1, $s1, $v1
	
	endFib1:
		add 	$v1, $v1, 1
	
	endFib:
		lw 	$ra, ($sp) 	#load return address from stack
		lw 	$s0, 4($sp)	#load local variable from stack
		lw	$s1, 8($sp)
		add	$sp, $sp, 12	#restore stack
		jr	$ra		#return
