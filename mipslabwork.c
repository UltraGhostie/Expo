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
    PORTE -= 0xff;
    counter = 0;
    }
    counter++;
    PORTE++;
    display_update();
    display_test(16, icon);
  }
  if (IFS(0) & 0x80){
    IFSCLR(0) = 0x80;
    display_update();
    display_image(16,icon);
  }
  if (IFS(0) & 0x800){
    IFSCLR(0) = 0x800;
  }
  if (IFS(0) & 0x80000){
    IFSCLR(0) = 0x80000;
  }
  if (IFS(0) & 0x100 != 0x100)
  {
    return;
  }
  IFSCLR(0) = 0x100;
  timeoutcount++;
  if(timeoutcount != 30)
    return;
  timeoutcount = 0;
  /*time2string( textstring, mytime );
  display_string( 3, textstring );
  tick( &mytime ); */
}

/* Lab-specific initialization goes here */
void labinit( void )
{
  TRISE &= 0xffffff00;
  PORTE &= 0xffffff00;
  TRISD |= 0x00000f00; // T: enable all sw
  IFSCLR(0) = 0x88880; // Interrupts for sw
  IECSET(0) = 0x88880;
  IPCCLR(1) = 0x1f;
  IPCSET(1) = 0xa;
  IPCCLR(2) = 0x1f;
  IPCSET(2) = 0xb;
  IPCCLR(3) = 0x1f;
  IPCSET(3) = 0xc;
  IPCCLR(4) = 0x1f;
  IPCSET(4) = 0xd;



  T2CON = 0x70; // T: reset timer and set prescale to 1:256
  TMR2 = 0x0; // T: clear timer register
  PR2 = 0x7a12; // T: 31'250. 80 MHz / 256 / 31250 = 10 Hz
  IFSCLR(0) = 0x100;
  IECSET(0) = 0x100;
  IPCSET(2) = 0xe;
  enable_interrupt();
  T2CONSET = 0x8000; // T: start timer
  
  return;
}

/* This function is called repetitively from the main program */
void labwork( void )
{
  /* prime = nextprime(prime);
  display_string(0, itoaconv(prime));
  display_update(); */
}