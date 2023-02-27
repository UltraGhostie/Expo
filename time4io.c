#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

// Everything below is written by Theodor Björkman
int getsw(void)
{
    int r = 0xf00; // T: 1111 0000 0000
    r &= PORTD;
    r >>= 8;
    return r;
}

int getbtns(void)
{
    int r = 0xe0; // T: 1110 0000
    r &= PORTD;
    r >>= 5;
    return r;
}