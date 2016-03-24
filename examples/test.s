	.text
	.org	0
	.global _start
_start:
	addi	r1, r0, 1
	addi	r2, r1, 2
	addi	r3, r2, 3
	addi	r4, r3, 4
exit:
	bri	exit

