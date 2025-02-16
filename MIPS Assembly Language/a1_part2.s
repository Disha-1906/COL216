.data
	prompt: .asciiz "Number of integers "
	prompt2: .asciiz "Enter array "
	prompt3: .asciiz "Enter Number "
	prompt4: .asciiz "Enter the number to be searched "
	prompt5: .asciiz "Searching "
.text
	main:

		#Printing prompt1
		li $v0,4
		la $a0,prompt
		syscall
		#Reading the input
		li $v0,5
		syscall


		#Allocating space of 4*n
		sll $a0,$v0,2
		move $s3,$a0
		li $v0,9
		syscall
		add $s0,$v0,$0
		add $s4,$s0,$0
		add $s1,$s3,$v0

		#Printing prompt2
		li $v0,4
		la $a0,prompt2
		syscall

	loop:
		beq $s1,$s0,Label
		#Printing prompt3
		li $v0,4
		la $a0,prompt3
		syscall
		#Reading array elements one by one
		li $v0,5
		syscall	
		#Storing array elements in heap
		sw $v0,($s0)
		addi $s0,$s0,4
		j loop
	Label: 
		#Printing prompt4
		li $v0,4
		la $a0,prompt4
		syscall
		#Reading input
		li $v0,5
		syscall
		#Storing input in reg
		add $s2,$0,$v0

		#Storing  arguments
		move $a0,$s2  #x=$a0
		move $a1,$s4 #base_addr=$a1
		move $a2,$s4 #low=$a2
		move $a3,$s1 #high=$a3
	
	binary_search:
		addi $sp,$sp,-20
		sw $a0,0($sp)
		sw $a1,4($sp)
		sw $a2,8($sp)
		sw $a3,12($sp)
		sw $ra,16($sp)

		ble $a3,$a2,L3
		add $t0,$a2,$a3
		srl $t0,$t0,1

		#Printing prompt5
		li $v0,4
		la $a0,prompt5
		syscall

		lw $t1,0($t0)
		beq $a0,$t1,L1
		blt $t1,$a0,L2
		addi $t0,$t0,-1
		move $a3,$t0
		jal binary_search
		j E 

	L1:
		addi $v0,$t0,0
		j E 
	L2:
		addi $t0,$t0,1
		move $a2,$t0
		jal binary_search
	L3:
		addi $v0,$0,-1 
		j E
	E:
		lw $a0,0($sp)
		lw $a1,4($sp)
		lw $a2,8($sp)
		lw $a3,12($sp)
		lw $ra,16($sp)		
		addi $sp,$sp,20
		jr $ra

		


