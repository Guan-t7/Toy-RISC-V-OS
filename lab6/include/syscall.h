#ifndef SYSCALL_H
#define SYSCALL_H

#include "stddef.h"

typedef struct
{
    uint64 a0;
    uint64 a1;
    uint64 a2;
    uint64 a3;
    uint64 a4;
    uint64 a5;
    uint64 a6;
    uint64 a7;
} pt_regs;

extern pt_regs regs;

#define SYS_OPENAT      56
#define SYS_CLOSE       57
#define SYS_READ        63
#define SYS_WRITE       64
#define SYS_EXIT        93
#define SYS_GETPID      172
#define SYS_MUNMAP      215
#define SYS_FORK        220 // clone
#define SYS_EXECVE      221
#define SYS_MMAP        222
#define SYS_MPROTECT    226
#define SYS_WAIT        260 // wait4

void ecall_u(pt_regs *regs);
int sys_write(unsigned int fd, const char *buf, size_t count);
long sys_getpid();

#endif