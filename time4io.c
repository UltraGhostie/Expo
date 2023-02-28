#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

// Everything below is written by Theodor Björkman
int getsw(void)
{
    int r = 0xf00;
    r &= PORTD;
    r >>= 8;
    return r;
}

int getbtns(void)
{
    int r = 0xe0;
    r &= PORTD;
    r >>= 4;
    r |= (PORTF & 2) >> 1;
    return r;
}