#include "test.h"
#include "vm.h"

int os_test()
{
    dead_loop();
    return 0;
}

void dead_loop(void)
{
    while (1)
        ;
}

void shutdown()
{
    set_satp(0);
    asm volatile("sh %0, 0(%1)"
                 :
                 : "r"(VIRT_TEST_FINISHER_PASS), "r"(SIFIVE_TEST));
}