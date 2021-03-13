#include "test.h"
#include "put.h"
#include "sched.h"

int os_test()
{
	const char *msg = "ZJU OS LAB 4             3180103008\n";
    puts(msg);

    puts("trying to write data to text section:\n");
    *((uint64 *)puts) = 0;
    puts("end of testing\n");

    task_init();
    while (1)
        ;

    return 0;
}
