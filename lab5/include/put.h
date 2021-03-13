#ifndef PUT_H
#define PUT_H

#include "stddef.h"

#define UART16550A_DR (volatile unsigned char *)0x10000000

int putchar(int ch);
int puts(const char *s);
char itoch(int x);
void puti(long x);
void puth(uint64 x, int longarg);
int my_printk(const char *s, ...) __attribute__((format(printf, 1, 2)));

#endif
