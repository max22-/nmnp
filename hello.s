.globl __start
.data
.word 42
.asciiz "Hello, world !\n"
.text
__start:
    li $v0, 1
    li $a0, -45
    syscall
    j __start
