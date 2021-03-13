#ifndef VM_H
#define VM_H

#include "stddef.h"

#define PAGE_SHIFT 12
#define PAGE_SIZE (1 << PAGE_SHIFT)
#define PROT_BITS 10
#define PTE_VALID 0b1
#define PERM_READ 0b10
#define PERM_WRITE 0b100
#define PERM_EXECUTE 0b1000

// linker symbol
extern const char text_start_lma, _end_lma,
    text_start, rodata_start, data_start;
char *pgtbl_memory = &_end_lma;

void *memset(void *mem, int ch, uint64 sz);

#endif
