#ifndef VM_H
#define VM_H

#include "stddef.h"
#include "sched.h"

#define PAGE_SHIFT 12
#define PAGE_SIZE (1 << PAGE_SHIFT)
#define PROT_BITS 10
#define PTE_VALID 0b1
#define PERM_READ 0b10
#define PERM_WRITE 0b100
#define PERM_EXECUTE 0b1000
#define PRIV_U 0b10000

// linker symbol
extern char text_start_lma, _end_lma,
    text_start, rodata_start, data_start,
    _end;

// U-mode task
extern const uint64 ucode_va, ucode_pa;
extern const uint64 UART_VA;

uint64 u_paging_init(struct task_struct *t);

// utility
uint64 get_satp();
void set_satp(uint64 satp);
void swap_satp();
void *memset(void *mem, int ch, uint64 sz);

#endif
