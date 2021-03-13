#include "vm.h"
#include "put.h"

void *get_free_page()
{
    char *page = pgtbl_memory;
    pgtbl_memory += PAGE_SIZE;
    return page;
}

/* 
 ! assumption:
 * called when satp.mode == bare
 * 4KB aligned addr
 TODO parameterize
 */
void create_1pg_mapping(uint64 *pgtbl, uint64 va, uint64 pa, int perm)
{
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
            pgtbl = (pte << 2) & ~0xFFF; // base of next level pgtbl
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

void paging_init()
{
    // root pgtbl base addr init as _end
    uint64 *root = get_free_page();
    memset(root, '\0', PAGE_SIZE);
    // kernel: to highest va; to identical va
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
    // UART: to identical va
    create_mapping(root, UART16550A_DR, UART16550A_DR, PAGE_SIZE,
                   PERM_READ | PERM_WRITE | PERM_EXECUTE);
}