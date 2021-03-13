#ifndef PUT_H
#define PUT_H

#include "stddef.h"

#define UART16550A_DR (volatile unsigned char *)0x10000000
void puti(int num);
int puts(const char *s);
void puth(uint64 x);

#endif
