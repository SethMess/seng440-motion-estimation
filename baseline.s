	.cpu cortex-a15
	.arch armv7-a
	.arch_extension virt
	.arch_extension idiv
	.arch_extension sec
	.arch_extension mp
	.fpu neon
	.eabi_attribute 28, 1
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 2
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"main.c"
	.text
	.section	.rodata.str1.4,"aMS",%progbits,1
	.align	2
.LC0:
	.ascii	"rb\000"
	.align	2
.LC1:
	.ascii	"P5 %d %d %d\000"
	.align	2
.LC2:
	.ascii	"Read image of size: %d by %d.\012\000"
	.text
	.align	1
	.p2align 2,,3
	.global	load_frame
	.syntax unified
	.thumb
	.thumb_func
	.type	load_frame, %function
load_frame:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	str	r4, [sp, #-20]!
	mov	r4, r1
	ldr	r1, .L8
	strd	r5, r6, [sp, #4]
	mov	r5, r2
	strd	r7, lr, [sp, #12]
	sub	sp, sp, #20
.LPIC0:
	add	r1, pc
	bl	fopen64(PLT)
	mov	r6, r0
	cbz	r0, .L5
	ldr	r1, .L8+4
	add	r3, sp, #12
	mov	r2, r4
	str	r3, [sp]
	mov	r3, r5
.LPIC1:
	add	r1, pc
	bl	__isoc99_fscanf(PLT)
	ldr	r3, [r4]
	ldr	r0, [r5]
	mul	r0, r3, r0
	bl	malloc(PLT)
	mov	r7, r0
	cbz	r0, .L1
	mov	r0, r6
	bl	fgetc(PLT)
	ldr	r1, [r5]
	mov	r3, r6
	mov	r0, r7
	ldr	r2, [r4]
	mul	r2, r1, r2
	mov	r1, #1
	bl	fread(PLT)
	ldr	r0, .L8+8
	ldr	r1, [r4]
	ldr	r2, [r5]
.LPIC2:
	add	r0, pc
	bl	printf(PLT)
.L1:
	mov	r0, r7
	add	sp, sp, #20
	ldrd	r4, r5, [sp]
	ldrd	r6, r7, [sp, #8]
	@ sp needed
	add	sp, sp, #16
	ldr	pc, [sp], #4
.L5:
	mov	r7, r0
	b	.L1
.L9:
	.align	2
.L8:
	.word	.LC0-(.LPIC0+4)
	.word	.LC1-(.LPIC1+4)
	.word	.LC2-(.LPIC2+4)
	.size	load_frame, .-load_frame
	.align	1
	.p2align 2,,3
	.global	sad_baseline
	.syntax unified
	.thumb
	.thumb_func
	.type	sad_baseline, %function
sad_baseline:
	@ args = 12, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	strd	r4, r5, [sp, #-16]!
	ldr	r5, [sp, #16]
	mov	r4, r3
	strd	r6, lr, [sp, #8]
	mov	r6, r0
	add	r4, r4, #15
	mov	r0, #0
	mla	lr, r6, r5, r1
	ldr	r1, [sp, #20]
	add	lr, lr, r4
	add	r3, r3, r1
	ldr	r1, [sp, #24]
	add	r3, r3, #-1
	add	r5, r5, r1
	mla	r3, r6, r5, r3
	mov	r5, #16
	add	r4, r2, r3
.L14:
	sub	r2, lr, #16
	mov	ip, r4
.L13:
	ldrb	r3, [r2, #1]!	@ zero_extendqisi2
	ldrb	r1, [ip, #1]!	@ zero_extendqisi2
	subs	r3, r3, r1
	ite	pl
	addpl	r0, r0, r3
	submi	r0, r0, r3
	cmp	lr, r2
	bne	.L13
	subs	r5, r5, #1
	add	lr, lr, r6
	add	r4, r4, r6
	bne	.L14
	ldrd	r4, r5, [sp]
	add	sp, sp, #8
	pop	{r6, pc}
	.size	sad_baseline, .-sad_baseline
	.align	1
	.p2align 2,,3
	.global	sad_unrolling
	.syntax unified
	.thumb
	.thumb_func
	.type	sad_unrolling, %function
sad_unrolling:
	@ args = 12, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	strd	r4, r5, [sp, #-24]!
	ldr	r4, [sp, #24]
	add	r5, r3, #1
	strd	r6, r7, [sp, #8]
	mov	r6, r0
	ldr	r0, [sp, #32]
	strd	r8, lr, [sp, #16]
	mov	r8, #16
	add	r0, r0, r4
	mul	r4, r6, r4
	mul	ip, r6, r0
	ldr	r0, [sp, #28]
	add	r5, r5, r4
	add	r7, r1, r5
	add	r0, r0, r3
	add	r3, r3, r1
	add	lr, r3, r4
	add	r3, r0, #1
	add	r3, r3, ip
	add	r0, r0, r2
	add	r4, r0, ip
	add	r5, r2, r3
	mov	r0, #0
.L24:
	mov	r3, #0
.L23:
	ldrb	r1, [r4, r3, lsl #1]	@ zero_extendqisi2
	ldrb	r2, [lr, r3, lsl #1]	@ zero_extendqisi2
	ldrb	ip, [r5, r3, lsl #1]	@ zero_extendqisi2
	subs	r2, r2, r1
	sub	r1, r0, r2
	it	pl
	addpl	r1, r0, r2
	ldrb	r2, [r7, r3, lsl #1]	@ zero_extendqisi2
	add	r3, r3, #1
	subs	r2, r2, ip
	sub	r0, r1, r2
	it	pl
	addpl	r0, r1, r2
	cmp	r3, #8
	bne	.L23
	subs	r8, r8, #1
	add	r7, r7, r6
	add	lr, lr, r6
	add	r5, r5, r6
	add	r4, r4, r6
	bne	.L24
	ldrd	r4, r5, [sp]
	ldrd	r6, r7, [sp, #8]
	add	sp, sp, #16
	pop	{r8, pc}
	.size	sad_unrolling, .-sad_unrolling
	.align	1
	.p2align 2,,3
	.global	sad_pipelining
	.syntax unified
	.thumb
	.thumb_func
	.type	sad_pipelining, %function
sad_pipelining:
	@ args = 12, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	strd	r4, r5, [sp, #-16]!
	ldr	r4, [sp, #16]
	mov	r5, #16
	strd	r6, lr, [sp, #8]
	mov	r6, r0
	mov	r0, #0
	mla	lr, r6, r4, r1
	ldr	r1, [sp, #20]
	add	r1, r1, r3
	add	r3, r3, #15
	add	lr, lr, r3
	ldr	r3, [sp, #24]
	add	r4, r4, r3
	mla	r3, r6, r4, r1
	add	r4, r2, r3
.L34:
	mov	r2, lr
	ldrb	ip, [r4]	@ zero_extendqisi2
	mov	r1, r4
	ldrb	r3, [r2, #-15]!	@ zero_extendqisi2
	sub	r3, r3, ip
.L31:
	cmp	r3, #0
	ldrb	ip, [r1, #1]!	@ zero_extendqisi2
	ite	lt
	sublt	r0, r0, r3
	addge	r0, r0, r3
	ldrb	r3, [r2, #1]!	@ zero_extendqisi2
	cmp	lr, r2
	sub	r3, r3, ip
	bne	.L31
	cmp	r3, #0
	add	r4, r4, r6
	ite	lt
	sublt	r0, r0, r3
	addge	r0, r0, r3
	subs	r5, r5, #1
	add	lr, lr, r6
	bne	.L34
	ldrd	r4, r5, [sp]
	add	sp, sp, #8
	pop	{r6, pc}
	.size	sad_pipelining, .-sad_pipelining
	.align	1
	.p2align 2,,3
	.global	sad_neon
	.syntax unified
	.thumb
	.thumb_func
	.type	sad_neon, %function
sad_neon:
	@ args = 12, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	strd	r4, r5, [sp, #-8]!
	sub	sp, sp, #16
	vmov.i32	q10, #0  @ v8hi
	ldrd	r4, r5, [sp, #24]
	vmov	q11, q10  @ v8hi
	add	ip, r3, r5
	mla	r3, r0, r4, r3
	add	r1, r1, r3
	ldr	r3, [sp, #32]
	add	r4, r4, r3
	mov	r3, #16
	mla	ip, r0, r4, ip
	add	r2, r2, ip
.L39:
	vld1.8	{d16-d17}, [r1], r0
	subs	r3, r3, #1
	vld1.8	{d18-d19}, [r2], r0
	vabal.u8	q11, d16, d18
	vabal.u8	q10, d17, d19
	bne	.L39
	vadd.i16	q10, q10, q11
	vpaddl.u16	q10, q10
	vpaddl.u32	q8, q10
	vst1.64	{d16-d17}, [sp:64]
	ldr	r0, [sp]
	ldr	r3, [sp, #8]
	add	r0, r0, r3
	add	sp, sp, #16
	ldrd	r4, r5, [sp]
	@ sp needed
	add	sp, sp, #8
	bx	lr
	.size	sad_neon, .-sad_neon
	.align	1
	.p2align 2,,3
	.global	sad_custom_asm
	.syntax unified
	.thumb
	.thumb_func
	.type	sad_custom_asm, %function
sad_custom_asm:
	@ args = 12, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	str	r4, [sp, #-12]!
	strd	r5, lr, [sp, #4]
	mov	r5, r0
	ldrd	r4, r0, [sp, #12]
	add	r0, r0, r3
	mla	r3, r5, r4, r3
	add	lr, r1, r3
	ldr	r3, [sp, #20]
	add	r4, r4, r3
	mla	r0, r5, r4, r0
	mov	r4, #16
	add	ip, r2, r0
	mov	r0, #0
.L43:
	mov	r3, #0
.L44:
	ldr	r2, [lr, r3]	@ unaligned
	ldr	r1, [ip, r3]	@ unaligned
	add	r3, r3, #4
	cmp	r3, #16
	.syntax unified
@ 195 "main.c" 1
	usada8 r0, r2, r1, r0
@ 0 "" 2
	.thumb
	.syntax unified
	bne	.L44
	subs	r4, r4, #1
	add	lr, lr, r5
	add	ip, ip, r5
	bne	.L43
	ldrd	r4, r5, [sp]
	add	sp, sp, #8
	ldr	pc, [sp], #4
	.size	sad_custom_asm, .-sad_custom_asm
	.align	1
	.p2align 2,,3
	.global	find_motion_vector
	.syntax unified
	.thumb
	.thumb_func
	.type	find_motion_vector, %function
find_motion_vector:
	@ args = 16, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	str	r4, [sp, #-36]!
	mov	r4, #0
	strd	r5, r6, [sp, #4]
	mov	r6, r2
	mov	r2, r3
	strd	r7, r8, [sp, #12]
	mov	r5, r1
	mov	r1, r6
	strd	r9, r10, [sp, #20]
	mov	r9, r0
	strd	fp, lr, [sp, #28]
	sub	sp, sp, #36
	ldrd	r10, r7, [sp, #72]
	mov	r3, r10
	strd	r7, r4, [sp]
	add	fp, r10, #1
	str	r4, [sp, #8]
	strd	r6, r2, [sp, #20]
	sub	r6, r9, #15
	bl	sad_baseline(PLT)
	sub	r3, r5, #15
	mov	r8, r0
	mvn	r5, #15
	str	r3, [sp, #28]
	ldr	r3, [sp, #80]
	str	r4, [r3]
	ldr	r3, [sp, #84]
	str	r4, [r3]
	b	.L54
.L49:
	add	r5, r5, #1
	cmp	r5, #17
	beq	.L62
.L54:
	adds	r2, r7, r5
	bmi	.L49
	ldr	r3, [sp, #28]
	cmp	r3, r2
	ble	.L49
	mvn	r4, #15
	b	.L53
.L51:
	ldrd	r1, r2, [sp, #20]
	mov	r3, r10
	mov	r0, r9
	strd	r7, r4, [sp]
	str	r5, [sp, #8]
	bl	sad_baseline(PLT)
	cmp	r0, r8
	bcs	.L50
	ldr	r3, [sp, #80]
	mov	r8, r0
	str	r4, [r3]
	ldr	r3, [sp, #84]
	str	r5, [r3]
.L50:
	add	r4, r4, #1
	cmp	r4, #17
	beq	.L49
.L53:
	adds	r3, r10, r4
	bmi	.L50
	cmp	r6, r3
	ble	.L50
	orrs	r3, r4, r5
	bne	.L51
	cmp	fp, #0
	blt	.L56
	cmp	r6, fp
	ble	.L56
	mov	r4, #1
	b	.L51
.L56:
	mov	r4, #2
	b	.L53
.L62:
	add	sp, sp, #36
	ldrd	r4, r5, [sp]
	ldrd	r6, r7, [sp, #8]
	@ sp needed
	ldrd	r8, r9, [sp, #16]
	ldrd	r10, fp, [sp, #24]
	add	sp, sp, #32
	ldr	pc, [sp], #4
	.size	find_motion_vector, .-find_motion_vector
	.align	1
	.p2align 2,,3
	.global	find_all_motion_vectors
	.syntax unified
	.thumb
	.thumb_func
	.type	find_all_motion_vectors, %function
find_all_motion_vectors:
	@ args = 8, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	str	r4, [sp, #-36]!
	strd	r5, r6, [sp, #4]
	strd	fp, lr, [sp, #28]
	mov	fp, r0
	add	r0, r0, #15
	ands	r0, r0, fp, asr #32
	it	cc
	movcc	r0, fp
	cmp	r1, #0
	strd	r7, r8, [sp, #12]
	strd	r9, r10, [sp, #20]
	sub	sp, sp, #36
	ldrd	r8, r9, [sp, #72]
	ble	.L63
	add	r4, r1, #-1
	cmp	fp, #0
	bic	r4, r4, #15
	add	lr, r4, #16
	ble	.L63
	add	r7, fp, #-1
	mov	r10, #0
	strd	fp, r1, [sp, #16]
	asr	r0, r0, #4
	bic	r7, r7, #15
	mov	r6, r10
	add	r7, r7, #16
	mov	r10, r9
	mov	fp, r2
	mov	r9, r8
	strd	r0, lr, [sp, #24]
	mov	r8, r3
.L66:
	ldr	r3, [sp, #24]
	asr	r5, r6, #4
	mov	r4, #0
	mul	r5, r3, r5
	lsl	r5, r5, #2
.L65:
	asr	ip, r4, #4
	ldrd	r0, r1, [sp, #16]
	strd	r4, r6, [sp]
	mov	r3, r8
	add	ip, r5, ip, lsl #2
	mov	r2, fp
	add	r4, r4, #16
	add	lr, r10, ip
	add	ip, ip, r9
	strd	ip, lr, [sp, #8]
	bl	find_motion_vector(PLT)
	cmp	r4, r7
	bne	.L65
	ldr	r3, [sp, #28]
	add	r6, r6, #16
	cmp	r6, r3
	bne	.L66
.L63:
	add	sp, sp, #36
	ldrd	r4, r5, [sp]
	ldrd	r6, r7, [sp, #8]
	@ sp needed
	ldrd	r8, r9, [sp, #16]
	ldrd	r10, fp, [sp, #24]
	add	sp, sp, #32
	ldr	pc, [sp], #4
	.size	find_all_motion_vectors, .-find_all_motion_vectors
	.section	.rodata.str1.4
	.align	2
.LC3:
	.ascii	"frames/frame1.pgm\000"
	.align	2
.LC4:
	.ascii	"frames/frame2.pgm\000"
	.align	2
.LC5:
	.ascii	"--selftest\000"
	.align	2
.LC6:
	.ascii	"vectors.pgm\000"
	.align	2
.LC7:
	.ascii	"Freed frame memory\000"
	.section	.text.startup,"ax",%progbits
	.align	1
	.p2align 2,,3
	.global	main
	.syntax unified
	.thumb
	.thumb_func
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	str	r4, [sp, #-36]!
	strd	r5, r6, [sp, #4]
	mov	r6, r1
	strd	r7, r8, [sp, #12]
	strd	r9, r10, [sp, #20]
	strd	fp, lr, [sp, #28]
	sub	sp, sp, #36
	mov	fp, r0
	ldr	r0, .L75
	add	r7, sp, #16
	add	r8, r7, #12
	add	r5, r7, #8
	mov	r2, r8
	mov	r1, r5
	str	sp, [r7]
.LPIC3:
	add	r0, pc
	bl	load_frame(PLT)
	mov	r4, r0
	ldr	r0, .L75+4
	mov	r2, r8
	mov	r1, r5
.LPIC4:
	add	r0, pc
	bl	load_frame(PLT)
	cmp	fp, #1
	ldrd	r8, r9, [r7, #8]
	mov	r10, r0
	ble	.L71
	ldr	r1, .L75+8
	ldr	r0, [r6, #4]
.LPIC5:
	add	r1, pc
	bl	strcmp(PLT)
	cmp	r0, #0
	beq	.L74
.L71:
	add	r5, r8, #15
	add	r6, r9, #15
	ands	r5, r5, r8, asr #32
	it	cc
	movcc	r5, r8
	ands	r6, r6, r9, asr #32
	it	cc
	movcc	r6, r9
	asr	r5, r5, #4
	asr	r6, r6, #4
	mov	r3, r10
	mov	r2, r4
	mul	ip, r6, r5
	mov	r1, r9
	mov	r0, r8
	lsl	ip, ip, #2
	add	ip, ip, #7
	bic	ip, ip, #7
	sub	sp, sp, ip
	str	r5, [r7, #4]
	add	r5, sp, #16
	sub	sp, sp, ip
	add	fp, sp, #16
	strd	r5, fp, [sp]
	bl	find_all_motion_vectors(PLT)
	ldr	r2, .L75+12
	mov	r1, r9
	mov	r0, r8
	str	fp, [sp, #8]
	strd	r6, r5, [sp]
	ldr	r3, [r7, #4]
	mov	r5, #0
.LPIC6:
	add	r2, pc
	str	r2, [sp, #12]
	mov	r2, r4
	bl	draw_motion_vectors(PLT)
	mov	r0, r4
	bl	free(PLT)
	mov	r0, r10
	bl	free(PLT)
	ldr	r0, .L75+16
.LPIC7:
	add	r0, pc
	bl	puts(PLT)
	ldr	sp, [r7]
.L70:
	mov	r0, r5
	add	r7, r7, #20
	mov	sp, r7
	ldrd	r4, r5, [sp]
	ldrd	r6, r7, [sp, #8]
	@ sp needed
	ldrd	r8, r9, [sp, #16]
	ldrd	r10, fp, [sp, #24]
	add	sp, sp, #32
	ldr	pc, [sp], #4
.L74:
	movs	r2, #1
	mov	r1, r9
	movs	r3, #3
	mov	r0, r8
	str	r2, [sp]
	mov	r2, r4
	bl	run_selftest(PLT)
	mvn	r1, #1
	mov	r2, r4
	mvn	r3, #2
	mov	r5, r0
	str	r1, [sp]
	ldrd	r0, r1, [r7, #8]
	bl	run_selftest(PLT)
	mov	r3, r0
	mov	r0, r4
	add	r5, r5, r3
	bl	free(PLT)
	mov	r0, r10
	bl	free(PLT)
	b	.L70
.L76:
	.align	2
.L75:
	.word	.LC3-(.LPIC3+4)
	.word	.LC4-(.LPIC4+4)
	.word	.LC5-(.LPIC5+4)
	.word	.LC6-(.LPIC6+4)
	.word	.LC7-(.LPIC7+4)
	.size	main, .-main
	.ident	"GCC: (Debian 14.2.0-19) 14.2.0"
	.section	.note.GNU-stack,"",%progbits
