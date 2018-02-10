#fibonacci value function
.data

var1:	.word	0	#returns 0
var2:	.word	1	#returns 1
var3:	.word	5	#returns 5
var4:	.word	10	#returns 
var5:	.word	20	#returns 6765
result:	.word	0

start: 	.asciiz "Fibonacci value for the number: "
newline:.asciiz "\n\n"
space:	.asciiz " "
.text


main:
main1:	la 	$a0, start	#print out intro string
	li 	$v0,	4
	syscall 
	lw 	$a0, var1	#print out the final number
	li 	$v0, 1		
	syscall
	la 	$a0, newline	#print out newline string
	li 	$v0,	4
	syscall 
	
	li	$v0, 0		#clear $v0
	li 	$v1, 0		#clear $v1
	lw 	$a0, var1	#place the number to solve in $a0
	jal	fib		#jump to fibonacci function
	
	#Display Result
	sw	$v1, result
	lw	$a0, result	#loads result into $a0
	li	$v0,	1	#sets syscall to print_int service
	syscall			#prints out number in $a0
	
	la 	$a0, newline	#print out newline string
	li 	$v0,	4
	syscall 
	
main2:	la 	$a0, start	#print out intro string
	li 	$v0,	4
	syscall 
	lw 	$a0, var2	#print out the final number
	li 	$v0, 1		
	syscall
	la 	$a0, newline	#print out newline string
	li 	$v0,	4
	syscall 
	
	li	$v0, 0		#clear $v0
	li 	$v1, 0		#clear $v1
	lw 	$a0, var2	#place the number to solve in $a0
	jal	fib		#jump to fibonacci function
	
	#Display Result
	sw	$v1, result
	lw	$a0, result	#loads result into $a0
	li	$v0,	1	#sets syscall to print_int service
	syscall			#prints out number in $a0
	
	la 	$a0, newline	#print out newline string
	li 	$v0,	4
	syscall 
	
main3:	la 	$a0, start	#print out intro string
	li 	$v0,	4
	syscall 
	lw 	$a0, var3	#print out the final number
	li 	$v0, 1		
	syscall
	la 	$a0, newline	#print out newline string
	li 	$v0,	4
	syscall 
	
	li	$v0, 0		#clear $v0
	li 	$v1, 0		#clear $v1
	lw 	$a0, var3	#place the number to solve in $a0
	jal	fib		#jump to fibonacci function
	
	#Display Result
	sw	$v1, result
	lw	$a0, result	#loads result into $a0
	li	$v0,	1	#sets syscall to print_int service
	syscall			#prints out number in $a0
	
	la 	$a0, newline	#print out newline string
	li 	$v0,	4
	syscall 
	
main4:	la 	$a0, start	#print out intro string
	li 	$v0,	4
	syscall 
	lw 	$a0, var4	#print out the final number
	li 	$v0, 1		
	syscall
	la 	$a0, newline	#print out newline string
	li 	$v0,	4
	syscall 
	
	li	$v0, 0		#clear $v0
	li 	$v1, 0		#clear $v1
	lw 	$a0, var4	#place the number to solve in $a0
	jal	fib		#jump to fibonacci function
	
	#Display Result
	sw	$v1, result
	lw	$a0, result	#loads result into $a0
	li	$v0,	1	#sets syscall to print_int service
	syscall			#prints out number in $a0
	
	la 	$a0, newline	#print out newline string
	li 	$v0,	4
	syscall 
	
main5:	la 	$a0, start	#print out intro string
	li 	$v0,	4
	syscall 
	lw 	$a0, var5	#print out the final number
	li 	$v0, 1		
	syscall
	la 	$a0, newline	#print out newline string
	li 	$v0,	4
	syscall 
	
	li	$v0, 0		#clear $v0
	li 	$v1, 0		#clear $v1
	lw 	$a0, var5	#place the number to solve in $a0
	jal	fib		#jump to fibonacci function
	
	#Display Result
	sw	$v1, result
	lw	$a0, result	#loads result into $a0
	li	$v0,	1	#sets syscall to print_int service
	syscall			#prints out number in $a0
	
	la 	$a0, newline	#print out newline string
	li 	$v0,	4
	syscall 
	
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
#-		- $s0: temp storage for $a0
#-		- $s1: previous value solved
#-	Return: 
#-		- $v1: result
#----------------------------------------------------------

fib:	
	#store local variables on stack
	add	$sp, $sp, -12		#space for three values
	sw	$ra, 0($sp)		#store return address
	sw	$s0, 4($sp)		#store temp $a0
	sw	$s1, 8($sp)		#store previous result
	
	move 	$s0, $a0		#store $a0 into $s0
	
	#Base Case
	beq 	$a0, 0, endFib0	 	#return 0
	beq 	$a0, 1, endFib1 	#return 1
	
	#fib(n-1)
	add 	$a0, $s0, -1		#calculate fib(n-1)
	jal 	fib
	move 	$s1, $v1 		#put results of fib(n-1) into s1
	
	
	#fib(n-2)
	add 	$a0, $s0, -2		#calculate fib(n-1)
	jal 	fib
	
	#Sum
	add 	$v1, $s1, $v1		#add fib(n-2) and fib(n-1)

	endFib:
		lw 	$ra, ($sp) 	#load return address from stack
		lw 	$s0, 4($sp)	#load temp $a0 from stack
		lw	$s1, 8($sp)	#load previous sum from stack
		add	$sp, $sp, 12	#restore stack
		jr	$ra		#return

	endFib0:
		li	$v1, 0		#return 0
		j 	endFib

	
	endFib1:
		li 	$v1, 1		#return 1
		j	endFib
	