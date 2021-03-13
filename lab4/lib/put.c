#include"put.h"
int puts(const char *s)
{
    while (*s != '\0')
    {
        *UART16550A_DR = (unsigned char)(*s);
        s++;
    }
    return 0;
}
static char itoch(int x)
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
void puti(int x)
{
    int digit = 1, tmp = x;
    while (tmp >= 10)
    {
        digit *= 10;
        tmp /= 10;
    }
    while (digit >= 1)
    {
        *UART16550A_DR = (unsigned char)itoch(x/digit);
        x %= digit;
        digit /= 10;
    }
    return;
}
void puth(uint64 x)
{
    uint64 digit = 1, tmp = x;
    while (tmp >= 16)
    {
        digit *= 16;
        tmp /= 16;
    }
    while (digit >= 1)
    {
        *UART16550A_DR = (unsigned char)itoch(x / digit);
        x %= digit;
        digit /= 16;
    }
    return;
}