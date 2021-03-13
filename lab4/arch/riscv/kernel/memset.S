.global memset

/* void *memset(void *, int, size_t) */
memset:
    mv t0, a0  /* Preserve return value */
    beqz a2, d
    add a3, t0, a2
l:
    sb a1, 0(t0)
	addi t0, t0, 1
	bltu t0, a3, l
d:
    ret