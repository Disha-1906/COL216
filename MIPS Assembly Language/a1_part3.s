.data
	prompt1: .asciiz "Enter number "
	prompt2: .asciiz "Enter exponent "

.text
	main:

		#Printing prompt1
		li $v0,4
		la $a0,prompt1
		syscall
		#Reading the input
		li $v0,5
		syscall
		move $s0,$v0

		#Printing prompt2
		li $v0,4
		la $a0,prompt2
		syscall
		#Reading the input
		li $v0,5
		syscall
		move $s1,$v0

		#loading arguments
		move $a0,$s0
		move $a1,$s1
		move $s2,$sp
	find_power:

		addi $sp,$sp,-12
		sw $a0,0($sp)
		sw $a1,4($sp)
		sw $ra,8($sp)

		beq $a1,1,L1
		beq $a1,0,L3
		div $t0,$a1,2
		mfhi $t0
		beq $t0,$0,L2
		addi $a1,$a1,-1
		div $a1,$a1,2
		jal find_power
		mul $v0,$v0,$v0
		mul $v0,$v0,$a0

		j E

	L1:
		add $v0,$a0,$0
		j E
	L2:
		div $a1,$a1,2
		jal find_power
		mul $v0,$v0,$v0
		j E
	L3:
		addi $v0,$0,1
		j E
	E:
		lw $a0,0($sp)
		lw $a1,4($sp)
		lw $ra,8($sp)
		addi $sp,$sp,12
		beq $sp,$s2,P
		jr $ra

	P:	
		move $t5,$v0
		li $v0 1
		move $a0,$t5
		syscall
		li $v0,10
		syscall