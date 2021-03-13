#include "stddef.h"
#include "vm.h"
#include "put.h"
#include "sched.h"

const uint64 ucode_va = 0x00000000, ucode_pa = 0x84000000;
const uint64 UART_VA = 0xffffffdf90000000;
static char *pgtbl_memory = &_end_lma;

void *get_free_page()
{
    char *page = pgtbl_memory;
    memset(page, '\0', PAGE_SIZE);
    pgtbl_memory += PAGE_SIZE;
    return page;
}

/* 
  The procedure uses effective address derived from
`pgtbl` and `va` to index the PTE. Addresses of PT
given must be PA as used by MMU.

TODO parameterize

pgtbl: PA of root page table base
pa: must be 4K-aligned
 */
void create_1pg_mapping(uint64 *pgtbl, uint64 va, uint64 pa, int perm)
{
    pa &= ~(uint64)0x1FF;
    va &= ~(uint64)0x1FF;

    uint64 vpn[3];
    va = va >> PAGE_SHIFT;
    vpn[0] = va & 0x1FF;
    va = va >> 9;
    vpn[1] = va & 0x1FF;
    va = va >> 9;
    vpn[2] = va & 0x1FF;

    // vpn[2-1]
    uint64 *p2pte, pte;
    for (int i = 2; i != 0; i--)
    {
        p2pte = pgtbl + vpn[i];
        pte = *p2pte;
        if (pte & PTE_VALID == PTE_VALID)
        {
            pgtbl = (uint64*)((pte & ~(uint64)0x3FF) << 2); // base of next level pgtbl
        }
        else // allocate physical page to be pointed by this pte
        {
            pgtbl = get_free_page();
            //TODO reserved pte[63:54]
            *p2pte = ((uint64)pgtbl >> 2) | PTE_VALID;
        }
    }
    // leaf level
    p2pte = pgtbl + vpn[0];
    *p2pte = (pa >> 2) | perm | PTE_VALID;
}

void create_mapping(uint64 *pgtbl, uint64 va, uint64 pa, uint64 sz, int perm)
{
    while (sz != 0)
    {
        create_1pg_mapping(pgtbl, va, pa, perm);
        va += PAGE_SIZE;
        pa += PAGE_SIZE;
        sz -= PAGE_SIZE;
    }
}

uint64 paging_init()
{
    // root pgtbl base addr init as _end
    uint64 *root = get_free_page();
    // kernel: Hi, Id
    uint64 pa = 0x80000000;
    uint64 va1 = 0xffffffe000000000, va2 = pa;
    uint64 sz = 16 << 20;
    // others
    create_mapping(root, va1, pa, sz,
                   PERM_READ | PERM_WRITE);
    create_mapping(root, va2, pa, sz,
                   PERM_READ | PERM_WRITE);
    // text; symbol *_start 4k aligned
    sz = &rodata_start - &text_start;
    create_mapping(root, va1, pa, sz,
                   PERM_READ | PERM_EXECUTE);
    create_mapping(root, va2, pa, sz,
                   PERM_READ | PERM_EXECUTE);
    // rodata
    pa += sz;
    va1 += sz;
    va2 += sz;
    sz = &data_start - &rodata_start;
    create_mapping(root, va1, pa, sz,
                   PERM_READ);
    create_mapping(root, va2, pa, sz,
                   PERM_READ);
    // UART: Id
    create_mapping(root, UART16550A_DR, UART16550A_DR, PAGE_SIZE,
                   PERM_READ | PERM_WRITE);
    return root;
}

/*
To be called in S-mode where Id page mapping available
 */
uint64 u_paging_init(struct task_struct *t)
{
    uint64 *root = get_free_page();
    // kernel: Hi
    uint64 pa = 0x80000000;
    uint64 va1 = 0xffffffe000000000;
    uint64 sz = 16 << 20;
    // others
    create_mapping(root, va1, pa, sz,
                   PERM_READ | PERM_WRITE);
    // text; symbol *_start 4k aligned
    sz = &rodata_start - &text_start;
    create_mapping(root, va1, pa, sz,
                   PERM_READ | PERM_EXECUTE);
    // rodata
    pa += sz;
    va1 += sz;
    sz = &data_start - &rodata_start;
    create_mapping(root, va1, pa, sz,
                   PERM_READ);
    // user
    create_mapping(root, ucode_va, ucode_pa, 4 * PAGE_SIZE,
                   PERM_READ | PERM_WRITE | PERM_EXECUTE | PRIV_U);
    uint64 ustk_va = t->thread.usp;
    static uint64 ustk_pa = 0x81000000;
    create_mapping(root, ustk_va - PAGE_SIZE, ustk_pa - PAGE_SIZE,
                   PAGE_SIZE, PERM_READ | PERM_WRITE | PRIV_U);
    ustk_pa -= PAGE_SIZE;
    // UART
    create_mapping(root, UART_VA, UART16550A_DR, PAGE_SIZE,
                   PERM_READ | PERM_WRITE);
    return root;
}

//! VA-PA mapping for locals in func must be preserved
uint64 get_satp()
{
    uint64 satp;
    asm("csrr %0, satp;"
        : "=r"(satp)
        :
        :);
    return satp;
}

void set_satp(uint64 satp)
{
    asm(
        "csrw satp, %0;\
         sfence.vma;"
        :
        : "r"(satp)
        :);
}

void swap_satp()
{
    uint64 saved_satp = get_satp();
    if (saved_satp != current->mm.satp)
    {
        set_satp(current->mm.satp);
        current->mm.satp = saved_satp;
    }
}