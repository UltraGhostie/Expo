/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 
   This file modified 2023-02-27 by Theodor Björkman

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

int mytime = 0x5957;
volatile int* portlights = (volatile int*) 0xbf886110;
int counter = 0;
int timeoutcount = 0;
int prime = 1234567;

char textstring[] = "text, more text, and even more text!";

/* Interrupt Service Routine */

void user_isr( void )
{
  if (IFS(0) & 0x8000)
  {
    IFSCLR(0) = 0x8000;
    if (counter == 0xff) {
    *portlights -= 0xff;
    counter = 0;
    }
    counter++;
    (*portlights)++;
  }
  if (IFS(0) & 0x100 != 0x100)
  {
    return;
  }
  IFSCLR(0) = 0x100;
  timeoutcount++;
  if(timeoutcount != 10)
    return;
  timeoutcount = 0;
  time2string( textstring, mytime );
  display_string( 3, textstring );
  display_update();
  tick( &mytime );
}

/* Lab-specific initialization goes here */
void labinit( void )
{
  // T: for surprise assignment
  volatile int* trislights = (volatile int*) 0xbf886100;
  *trislights &= 0xffffff00;
  *portlights &= 0xffffff01;
  TRISD |= 0x00000400; // T: 0100 0000 0000: <10> = sw3
  IFSCLR(0) = 0x8000;
  IECSET(0) = 0x8000;
  IPCCLR(3) = 0x1c;
  IPCCLR(3) = 0x3;
  IPCSET(3) = 0xe;



  T2CON = 0x70; // T: reset timer and set prescale to 1:256
  TMR2 = 0x0; // T: clear timer register
  PR2 = 0x7a12; // T: 31'250. 80 MHz / 256 / 31250 = 10 Hz
  IFSCLR(0) = 0x100;
  IECSET(0) = 0x100;
  IPCSET(2) = 0xD;
  enable_interrupt();
  T2CONSET = 0x8000; // T: start timer
  
  return;
}

/* This function is called repetitively from the main program */
void labwork( void )
{
  prime = nextprime(prime);
  display_string(0, itoaconv(prime));
  display_update();
}