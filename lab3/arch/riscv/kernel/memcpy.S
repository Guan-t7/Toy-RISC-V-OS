.global memcpy

/* void *memcpy(void *, const void *, size_t) */
memcpy:
	mv t6, a0  /* Preserve return value */

	/* Defer to byte-oriented copy for small sizes */
	sltiu a3, a2, 128
	bnez a3, 4f
	/* Use word-oriented copy only if low-order bits match */
	andi a3, t6, 8-1
	andi a4, a1, 8-1
	bne a3, a4, 4f

	beqz a3, 2f  /* Skip if already aligned */
	/*
	 * Round to nearest double word-aligned address
	 * greater than or equal to start address
	 */
	andi a3, a1, ~(8-1)
	addi a3, a3, 8
	/* Handle initial misalignment */
	sub a4, a3, a1
1:
	lb a5, 0(a1)
	addi a1, a1, 1
	sb a5, 0(t6)
	addi t6, t6, 1
	bltu a1, a3, 1b
	sub a2, a2, a4  /* Update count */

2:
	andi a4, a2, ~((16*8)-1)
	beqz a4, 4f
	add a3, a1, a4
3:
	ld a4,       0(a1)
	ld a5,   8(a1)
	ld a6, 2*8(a1)
	ld a7, 3*8(a1)
	ld t0, 4*8(a1)
	ld t1, 5*8(a1)
	ld t2, 6*8(a1)
	ld t3, 7*8(a1)
	ld t4, 8*8(a1)
	ld t5, 9*8(a1)
	sd a4,       0(t6)
	sd a5,   8(t6)
	sd a6, 2*8(t6)
	sd a7, 3*8(t6)
	sd t0, 4*8(t6)
	sd t1, 5*8(t6)
	sd t2, 6*8(t6)
	sd t3, 7*8(t6)
	sd t4, 8*8(t6)
	sd t5, 9*8(t6)
	ld a4, 10*8(a1)
	ld a5, 11*8(a1)
	ld a6, 12*8(a1)
	ld a7, 13*8(a1)
	ld t0, 14*8(a1)
	ld t1, 15*8(a1)
	addi a1, a1, 16*8
	sd a4, 10*8(t6)
	sd a5, 11*8(t6)
	sd a6, 12*8(t6)
	sd a7, 13*8(t6)
	sd t0, 14*8(t6)
	sd t1, 15*8(t6)
	addi t6, t6, 16*8
	bltu a1, a3, 3b
	andi a2, a2, (16*8)-1  /* Update count */

4:
	/* Handle trailing misalignment */
	beqz a2, 6f
	add a3, a1, a2

	/* Use word-oriented copy if co-aligned to word boundary */
	or a5, a1, t6
	or a5, a5, a3
	andi a5, a5, 3
	bnez a5, 5f
7:
	lw a4, 0(a1)
	addi a1, a1, 4
	sw a4, 0(t6)
	addi t6, t6, 4
	bltu a1, a3, 7b

	ret

5:
	lb a4, 0(a1)
	addi a1, a1, 1
	sb a4, 0(t6)
	addi t6, t6, 1
	bltu a1, a3, 5b
6:
	ret