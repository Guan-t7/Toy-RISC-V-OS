#include "put.h"
#include "sched.h"
#include "syscall.h"
#include "vm.h"

pt_regs regs;

void handler_s(size_t scause, size_t sepc, pt_regs *regs)
{
    const char *msg = "";
    int oops = 0;
    if (scause == 8) // ECALL_U
    {
        ecall_u(regs);
    }
    else
    {
        switch (scause)
        {
        case 12: // INSTR_PF
            msg = "INSTR_PF";
            oops = 1;
            break;
        case 13: // LOAD_PF
            msg = "LOAD_PF";
            oops = 1;
            break;
        case 15: // STORE_PF
            msg = "STORE_PF";
            oops = 1;
            break;
        default:
            msg = "!!! UNEXPECTED ERROR !!!";
            oops = 1;
            break;
        }
        puts(msg);
        puts("    epc = 0x");
        puth(sepc, 1);
        puts("\n");
    }
    while (oops)
        ;
}

void ecall_u(pt_regs *regs)
{
    switch (regs->a7)
    {
    case 64:
        regs->a0 = sys_write(regs->a0, regs->a1, regs->a2);
        break;
    case 172:
        regs->a0 = sys_getpid();
        break;
    default:
        break;
    }
}

int sys_write(unsigned int fd, const char *buf, size_t count)
{
    int rv = 0;
    switch (fd)
    {
    case 1:
        swap_satp();
        for (size_t i = 0; i < count; i++)
        {
            *(uint64 *)UART_VA = (unsigned char)(*buf++);
        }
        swap_satp();
        rv = count;
        break;
    default:
        break;
    }
    return rv;
}

long sys_getpid()
{
    return current->pid;
}