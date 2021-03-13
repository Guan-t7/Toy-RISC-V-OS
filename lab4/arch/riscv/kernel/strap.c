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

void error_handler(uint64 cause, uint64 epc)
{
    const char *msg;
    int oops = 0;
    switch (cause)
    {
    case 12: // INSTR_PF
        msg = "INSTR_PF";
        break;
    case 13: // LOAD_PF
        msg = "LOAD_PF";
        break;
    case 15: // STORE_PF
        msg = "STORE_PF";
        break;
    default:
        msg = "!!! UNEXPECTED ERROR !!!";
        oops = 1;
        break;
    }
    puts(msg);
    puts("    epc = 0x");
    puth(epc);
    puts("\n");
    while (oops)
        ;
}