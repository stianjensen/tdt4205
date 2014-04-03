.syntax unified
.cpu cortex-a15
.fpu vfpv3-d16
.data
.align	2
.DEBUG: .ascii "Hit Debug\n\000"
.DEBUGINT: .ascii "Hit Debug, r0 was: %d\n\000"
.INTEGER: .ascii "%d \000"
.FLOAT: .ascii "%f \000"
.NEWLINE: .ascii "\n \000"
.globl main
.align	2
.text
#0 Starting PROGRAM
_Simple_construct:
#1 Starting FUNCTION (construct) with depth 2
	push	{lr}
	push	{fp}
	mov	fp, sp
#2 Starting ASSIGNMENT_STATEMENT
#3 Starting VARIABLE
	push	{fp}
	ldr	r0, [fp, #12]
	pop	{fp}
	push	{r0}
#4 End VARIABLE a, depth difference: 0, stack offset: 12
#5 Starting EXPRESSION of type THIS
	ldr	r1, [fp, #8]
	push	{r1}
#6 Ending EXPRESSION of type THIS
	pop	{r2}
	mov	r1, #0
	add	r0, r1, r2
	pop	{r1}
	str	r1, [r0]
#7 End ASSIGNMENT_STATEMENT
	mov	sp, fp
	pop	{fp}
	pop	{pc}
#8 Leaving FUNCTION (construct) with depth 2
_Wrapper_construct:
#9 Starting FUNCTION (construct) with depth 2
	push	{lr}
	push	{fp}
	mov	fp, sp
#10 Starting ASSIGNMENT_STATEMENT
#11 Starting VARIABLE
	push	{fp}
	ldr	r0, [fp, #12]
	pop	{fp}
	push	{r0}
#12 End VARIABLE inner, depth difference: 0, stack offset: 12
#13 Starting EXPRESSION of type THIS
	ldr	r1, [fp, #8]
	push	{r1}
#14 Ending EXPRESSION of type THIS
	pop	{r2}
	mov	r1, #0
	add	r0, r1, r2
	pop	{r1}
	str	r1, [r0]
#15 End ASSIGNMENT_STATEMENT
	mov	sp, fp
	pop	{fp}
	pop	{pc}
#16 Leaving FUNCTION (construct) with depth 2
_main:
#17 Starting FUNCTION (main) with depth 2
	push	{lr}
	push	{fp}
	mov	fp, sp
#18 Starting DECLARATION: adding space on stack
	push	{r0}
#19 Ending DECLARATION
#20 Starting ASSIGNMENT_STATEMENT
#21 Starting EXPRESSION of type NEW
	movw	r0, #:lower16:32
	movt	r0, #:upper16:32
	push	{r0}
	bl	_malloc
	push	{r0}
#22 Ending EXPRESSION of type NEW
	push	{fp}
	pop	{r1}
	pop	{r0}
	str	r0, [fp, #-4]
	mov	fp, r1
#23 End ASSIGNMENT_STATEMENT
#24 Starting EXPRESSION of type METH_CALL
#25 Starting CONSTANT
	movw	r0, #:lower16:3
	movt	r0, #:upper16:3
	push	{r0}
#26 End CONSTANT
#27 Pushing THIS
#28 Starting VARIABLE
	push	{fp}
	ldr	r0, [fp, #-4]
	pop	{fp}
	push	{r0}
#29 End VARIABLE s, depth difference: 0, stack offset: -4
	bl	_Simple_construct
	push	{r0}
#30 Ending EXPRESSION of type METH_CALL
#31 Starting PRINT_STATEMENT
#32 Starting EXPRESSION of type CLASS_FIELD
#33 Starting VARIABLE
	push	{fp}
	ldr	r0, [fp, #-4]
	pop	{fp}
	push	{r0}
#34 End VARIABLE s, depth difference: 0, stack offset: -4
	pop	{r1}
	mov	r2, #0
	add	r3, r1, r2
	ldr	r0, [r3]
	push	{r0}
#35 Ending EXPRESSION of type CLASS_FIELD
	movw	r0, #:lower16:.INTEGER
	movt	r0, #:upper16:.INTEGER
	pop	{r1}
	bl	printf
	movw	r0, #:lower16:0x0A
	movt	r0, #:upper16:0x0A
	bl	putchar
#36 Ending PRINT_STATEMENT
#37 Starting DECLARATION: adding space on stack
	push	{r0}
#38 Ending DECLARATION
#39 Starting ASSIGNMENT_STATEMENT
#40 Starting EXPRESSION of type NEW
	movw	r0, #:lower16:32
	movt	r0, #:upper16:32
	push	{r0}
	bl	_malloc
	push	{r0}
#41 Ending EXPRESSION of type NEW
	push	{fp}
	pop	{r1}
	pop	{r0}
	str	r0, [fp, #-8]
	mov	fp, r1
#42 End ASSIGNMENT_STATEMENT
#43 Starting EXPRESSION of type METH_CALL
#44 Starting CONSTANT
	movw	r0, #:lower16:4
	movt	r0, #:upper16:4
	push	{r0}
#45 End CONSTANT
#46 Pushing THIS
#47 Starting VARIABLE
	push	{fp}
	ldr	r0, [fp, #-8]
	pop	{fp}
	push	{r0}
#48 End VARIABLE s2, depth difference: 0, stack offset: -8
	bl	_Simple_construct
	push	{r0}
#49 Ending EXPRESSION of type METH_CALL
#50 Starting DECLARATION: adding space on stack
	push	{r0}
#51 Ending DECLARATION
#52 Starting ASSIGNMENT_STATEMENT
#53 Starting EXPRESSION of type NEW
	movw	r0, #:lower16:32
	movt	r0, #:upper16:32
	push	{r0}
	bl	_malloc
	push	{r0}
#54 Ending EXPRESSION of type NEW
	push	{fp}
	pop	{r1}
	pop	{r0}
	str	r0, [fp, #-12]
	mov	fp, r1
#55 End ASSIGNMENT_STATEMENT
#56 Starting EXPRESSION of type METH_CALL
#57 Starting VARIABLE
	push	{fp}
	ldr	r0, [fp, #-8]
	pop	{fp}
	push	{r0}
#58 End VARIABLE s2, depth difference: 0, stack offset: -8
#59 Pushing THIS
#60 Starting VARIABLE
	push	{fp}
	ldr	r0, [fp, #-12]
	pop	{fp}
	push	{r0}
#61 End VARIABLE w, depth difference: 0, stack offset: -12
	bl	_Wrapper_construct
	push	{r0}
#62 Ending EXPRESSION of type METH_CALL
#63 Starting PRINT_STATEMENT
#64 Starting EXPRESSION of type CLASS_FIELD
#65 Starting EXPRESSION of type CLASS_FIELD
#66 Starting VARIABLE
	push	{fp}
	ldr	r0, [fp, #-12]
	pop	{fp}
	push	{r0}
#67 End VARIABLE w, depth difference: 0, stack offset: -12
	pop	{r1}
	mov	r2, #0
	add	r3, r1, r2
	ldr	r0, [r3]
	push	{r0}
#68 Ending EXPRESSION of type CLASS_FIELD
	pop	{r1}
	mov	r2, #0
	add	r3, r1, r2
	ldr	r0, [r3]
	push	{r0}
#69 Ending EXPRESSION of type CLASS_FIELD
	movw	r0, #:lower16:.INTEGER
	movt	r0, #:upper16:.INTEGER
	pop	{r1}
	bl	printf
	movw	r0, #:lower16:0x0A
	movt	r0, #:upper16:0x0A
	bl	putchar
#70 Ending PRINT_STATEMENT
#71 Starting ASSIGNMENT_STATEMENT
#72 Starting CONSTANT
	movw	r0, #:lower16:5
	movt	r0, #:upper16:5
	push	{r0}
#73 End CONSTANT
#74 Starting VARIABLE
	push	{fp}
	ldr	r0, [fp, #-8]
	pop	{fp}
	push	{r0}
#75 End VARIABLE s2, depth difference: 0, stack offset: -8
	pop	{r2}
	mov	r1, #0
	add	r0, r1, r2
	pop	{r1}
	str	r1, [r0]
#76 End ASSIGNMENT_STATEMENT
#77 Starting PRINT_STATEMENT
#78 Starting EXPRESSION of type CLASS_FIELD
#79 Starting EXPRESSION of type CLASS_FIELD
#80 Starting VARIABLE
	push	{fp}
	ldr	r0, [fp, #-12]
	pop	{fp}
	push	{r0}
#81 End VARIABLE w, depth difference: 0, stack offset: -12
	pop	{r1}
	mov	r2, #0
	add	r3, r1, r2
	ldr	r0, [r3]
	push	{r0}
#82 Ending EXPRESSION of type CLASS_FIELD
	pop	{r1}
	mov	r2, #0
	add	r3, r1, r2
	ldr	r0, [r3]
	push	{r0}
#83 Ending EXPRESSION of type CLASS_FIELD
	movw	r0, #:lower16:.INTEGER
	movt	r0, #:upper16:.INTEGER
	pop	{r1}
	bl	printf
	movw	r0, #:lower16:0x0A
	movt	r0, #:upper16:0x0A
	bl	putchar
#84 Ending PRINT_STATEMENT
#85 Starting PRINT_STATEMENT
#86 Starting EXPRESSION of type CLASS_FIELD
#87 Starting VARIABLE
	push	{fp}
	ldr	r0, [fp, #-8]
	pop	{fp}
	push	{r0}
#88 End VARIABLE s2, depth difference: 0, stack offset: -8
	pop	{r1}
	mov	r2, #0
	add	r3, r1, r2
	ldr	r0, [r3]
	push	{r0}
#89 Ending EXPRESSION of type CLASS_FIELD
	movw	r0, #:lower16:.INTEGER
	movt	r0, #:upper16:.INTEGER
	pop	{r1}
	bl	printf
	movw	r0, #:lower16:0x0A
	movt	r0, #:upper16:0x0A
	bl	putchar
#90 Ending PRINT_STATEMENT
#91 Starting ASSIGNMENT_STATEMENT
#92 Starting CONSTANT
	movw	r0, #:lower16:6
	movt	r0, #:upper16:6
	push	{r0}
#93 End CONSTANT
#94 Starting EXPRESSION of type CLASS_FIELD
#95 Starting VARIABLE
	push	{fp}
	ldr	r0, [fp, #-12]
	pop	{fp}
	push	{r0}
#96 End VARIABLE w, depth difference: 0, stack offset: -12
	pop	{r1}
	mov	r2, #0
	add	r3, r1, r2
	ldr	r0, [r3]
	push	{r0}
#97 Ending EXPRESSION of type CLASS_FIELD
	pop	{r2}
	mov	r1, #0
	add	r0, r1, r2
	pop	{r1}
	str	r1, [r0]
#98 End ASSIGNMENT_STATEMENT
#99 Starting PRINT_STATEMENT
#100 Starting EXPRESSION of type CLASS_FIELD
#101 Starting VARIABLE
	push	{fp}
	ldr	r0, [fp, #-8]
	pop	{fp}
	push	{r0}
#102 End VARIABLE s2, depth difference: 0, stack offset: -8
	pop	{r1}
	mov	r2, #0
	add	r3, r1, r2
	ldr	r0, [r3]
	push	{r0}
#103 Ending EXPRESSION of type CLASS_FIELD
	movw	r0, #:lower16:.INTEGER
	movt	r0, #:upper16:.INTEGER
	pop	{r1}
	bl	printf
	movw	r0, #:lower16:0x0A
	movt	r0, #:upper16:0x0A
	bl	putchar
#104 Ending PRINT_STATEMENT
	mov	sp, fp
	pop	{fp}
	pop	{pc}
#105 Leaving FUNCTION (main) with depth 2
debugprint:
	push {r0-r11, lr}
	movw	r0, #:lower16:.DEBUG
	movt	r0, #:upper16:.DEBUG
	bl	printf
	pop {r0-r11, pc}
debugprint_r0:
	push {r0-r11, lr}
	mov	r1, r0
	movw	r0, #:lower16:.DEBUGINT
	movt	r0, #:upper16:.DEBUGINT
	bl	printf
	pop {r0-r11, pc}
_malloc:
	push	{lr}
	ldr	r0, [sp, #4]
	bl	malloc
	pop {pc}
main:
	push	{lr}
	push	{fp}
	mov	fp, sp
	mov	r5, r0
	subs	r5, #1
	beq	noargs
	mov	r6, r1
pusharg:
	ldr	r0, [r6, #4]
	add	r6, r6, #4
	mov	r1, #0
	mov	r2, #10
	bl	strtol
	push	{r0}
	subs	r5, #1
	bne	pusharg
noargs:
	bl	_main
#106 End PROGRAM
	mov	sp, fp
	pop	{fp}
	bl	exit
.end
