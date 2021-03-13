#include "put.h"

int putchar(int ch)
{
    *UART16550A_DR = (unsigned char)ch;
    return ch;
}

int puts(const char *s)
{
    while (*s != '\0')
    {
        putchar(*s);
        s++;
    }
    return 0;
}

char itoch(int x)
{
    char ch = 0;
    if (x >= 0)
    {
        if (x <= 9)
        {
            ch = (char)(x + '0');
        }
        else if (x <= 15)
        {
            ch = (char)(x - 10 + 'a');
        }
    }
    return ch;
}

void puti(long x)
{
    if (x < 0)
    {
        putchar('-');
        x = -x;
    }
    int digit = 1;
    long tmp = x;
    while (tmp >= 10)
    {
        digit *= 10;
        tmp /= 10;
    }
    while (digit >= 1)
    {
        int c = itoch(x / digit);
        if (c)
            putchar(c); 
        x %= digit;
        digit /= 10;
    }
    return;
}

void puth(uint64 x, int longarg)
{
    puts("0x");
    char s[17];
    int hexdigits = longarg ? 16 : 8;
    s[hexdigits] = '\0';
    int i = 0;
    do
    {
        --hexdigits;
        s[hexdigits] = itoch(x & 0xF);
        x = x >> 4;
    } while (hexdigits != 0);
    puts(s);
    return;
}

static int vprintfmt(int fd, int(*putch)(int), const char *fmt, va_list vl) {
    int in_format = 0, longarg = 0;

    for( ; *fmt; fmt++) {
        if (in_format) {
            switch(*fmt) {
                case 'l': { 
                    longarg = 1; break; 
                }

                case 'x': {
                    uint64 num = longarg ? va_arg(vl, long) : va_arg(vl, int);
                    puth(num, longarg);
                    longarg = 0;
                    in_format = 0;
                    break;
                }
            
                case 'd': {
                    long num = longarg ? va_arg(vl, long) : va_arg(vl, int);
                    puti(num);
                    longarg = 0;
                    in_format = 0;
                    break;
                }
                // TODO possible bug underlying; 0x8000000000000000 not passing
                case 'u': {
                    unsigned long num = longarg ? va_arg(vl, long) : va_arg(vl, int);
                    int bits = 0;
                    char decchar[25] = {'0', 0};
                    for (long tmp = num; tmp; bits++)
                    {
                        decchar[bits] = (tmp % 10) + '0';
                        tmp /= 10;
                    }

                    if (bits == 0)
                        bits++;

                    for (int i = bits - 1; i >= 0; i--)
                    {
                        putchar(decchar[i]);
                    }
                    longarg = 0;
                    in_format = 0;
                    break;
                }

                case 's': {
                    const char* str = va_arg(vl, const char*);
                    puts(str);
                    longarg = 0;
                    in_format = 0;
                    break;
                }

                case 'c': {
                    char ch = (char)va_arg(vl,int);
                    putchar(ch);
                    longarg = 0;
                    in_format = 0;
                    break;
                }
                default:
                    break;
            }
        }
        else if(*fmt == '%') {
          in_format = 1;
        }
        else {
            putchar(*fmt);
        }
    }
    return 0;
}

int my_printk(const char *s, ...)
{
    va_list vl;
    va_start(vl, s);
    vprintfmt(1, putchar, s, vl);
    va_end(vl);
    return 0;
}