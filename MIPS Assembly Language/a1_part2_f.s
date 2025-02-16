.data
	prompt: .asciiz "Number of integers "
	prompt2: .asciiz "Enter array "
	prompt3: .asciiz "Enter Number "
	prompt4: .asciiz "Enter the number to be searched "
	prompt5: .asciiz "Searching "
	prompt6: .asciiz "Yes at index "
	prompt7: .asciiz "Not found"
.text
	main:

		#Printing prompt1
		li $v0,4
		la $a0,prompt
		syscall
		#Reading the input
		li $v0,5
		syscall
		move $s5,$v0

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
		move $s2,$v0

		#Storing  arguments
		#move $a0,$s2  #x=$a0
#		move $a1,$s4 #base_addr=$a1
#		move $a2,$0 #low=$a2
#		move $a3,$s5 #high=$a3=n
#		addi $a3,$a3,-1
		move $s6,$0
		addi $s5,$s5,-1
	binary_search:
		blt $s5,$s6,L3
		add $t0,$s6,$s5 #l+h =t0
		div $t0,$t0,2 #l+h/2
		#addi $t0,$t0,-1
		move $t2 $t0
		sll $t2,$t2,2 #addr_offser
		add $t2,$t2,$s4  #base_addr+addr_offset = addr_mid
		lw $t3,0($t2) #t3=arr[addr_mid]
		#Printing prompt5
		#li $v0,4
		#la $a0,prompt5
		#syscall

		beq $s2,$t3,L1  #x=arr[mid]
		blt $t3,$s2,L2 #arr[mid]<x
		addi $t0,$t0,-1  #mid = mid-1
		move $s5,$t0     #high = mid-1
		j binary_search

	L1:
		addi $v0,$t0,0 #return 
		#Printing prompt6
		li $v0,4
		la $a0,prompt6
		syscall
		li $v0 1
		move $a0 $t0
		syscall
		j E 
	L2:
		addi $t0,$t0,1
		move $s6,$t0
		j binary_search
	L3:
		addi $v0,$0,-1 
		#Printing prompt7
		li $v0,4
		la $a0,prompt7
		syscall
		j E

	E:
		li $v0 ,10
		syscall
		


