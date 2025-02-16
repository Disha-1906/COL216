.data
buffer : .space 100

.text
.globl main
main:
	li $v0,8
	la $a0,buffer
	li $a1, 100
	syscall

	li $v0,4
	la $a0,buffer
	syscall

	li $v0,10
	syscall

	