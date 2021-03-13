#include "put.h"
#include "vm.h"

void handler_m(uint64 mcause, uint64 mepc) __attribute__((noreturn));

void handler_m(uint64 mcause, uint64 mepc)
{
    my_printk("[M] FAULT !!! mcause: %d, mepc: %lx\n", mcause, mepc);
    shutdown();
}