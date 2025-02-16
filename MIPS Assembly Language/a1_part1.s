.text
.globl main
main:
	li $v0,9
	li $a0,100
	syscall
	move $s0,$v0
	li $v0,8
	move $a0,$s0
	li $a1,100
	syscall
	li $v0,4
	move $a0,$s0
	syscall
	li $v0,10
	move $a0,$s0
	syscall
	li $v0,10
	syscall