#include "put.h"

void print_timer_int()
{
    static int count = 0, line = 0;
    ++count;
    if (count == 100)
    {
        count = 0;
        puts("[S] Supervisor Mode Timer Interrupt ");
        puti(line);
        puts("\n");
        ++line;
    }
}