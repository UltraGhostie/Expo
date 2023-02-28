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

int timeoutcount = 0;

/* Interrupt Service Routine */

void user_isr( void )
{
  if (IFS(0) & 0x80){ // sw1
    IFSCLR(0) = 0x80;
    clearscene();
  }
  if (IFS(0) & 0x800){ // sw2
    IFSCLR(0) = 0x800;
    uint8_t testsprite[] = { 1, 1 };
    addsprite(testsprite, 2, 2);
  }
  if (IFS(0) & 0x8000) // sw3
  {
    IFSCLR(0) = 0x8000;
    uint8_t testsprite[] = { 2, 3 };
    addsprite(testsprite, 4, 3);
  }
  if (IFS(0) & 0x80000){ // sw4
    IFSCLR(0) = 0x80000;
    uint8_t testsprite[] = { 3, 9 };
    addsprite(testsprite, 7, 4);
  }
  if (IFS(0) & 0x100 != 0x100) // timer2
  {
    return;
  }
  IFSCLR(0) = 0x100;
  timeoutcount++;
  if(timeoutcount != 1)
    return;
  timeoutcount = 0;
  labwork();
}

/* Lab-specific initialization goes here */
void labinit( void )
{
  TRISE &= 0xffffff00;
  PORTE &= 0xffffff00;
  TRISD |= 0x00000fe0; // T: enable all sw and buttons
  TRISF |= 0x1; // btn1
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
  
  clearscene();
  return;
}

// called by user_isr every timoutcount * 1/10 seconds
void labwork( void )
{
  clearscene();
  updateentities();
  display_scene(scene);
}

/* clears the scene */
void clearscene(void)
{
  int i;
  for (i = 0; i < 512; i++)
  {
    scene[i] = 0;
  }
}

/* adds a square sprite to the scene*/
void addsprite(uint8_t* sprite, uint8_t x, uint8_t y)
{
  // translates y coordinates to start page
  uint8_t page = 0;
  while (y>7)
  {
    y -= 8;
    page++;
  }

  // checks if we are trying to draw outside the screen
  if(page<0 | page>3) return;
  if(x<0 | x>127-sprite[0]) return;
  
  // each page is 8 bits so this checks how many pages we need to draw
  int i;
  int pages = 1;
  for (i = 0; i <= sprite[1] + y; i++) 
  {
    if (i > 8*pages)
    {
      pages++;
    }
  }
  
  // this sets the right values in the canvas by or-ing them so they dont remove things that are already there
  int j,k,l;
  for (i = x; i < sprite[0] + x; i++) // steps x axis
  {
    l = sprite[1];
    for (j = page; j < page + pages; j++) // steps page
    {
      uint8_t byte = 0;
      for (k = 0; k < 8; k++) // sets data for page
      {
        if(k < y && j == page) continue; // skip bits so that we set y coordinates correctly
        if(l < 1) continue; // if we have drawn the entire length then we dont need to draw more
        l--;
        byte |= 1 << k; // set the bit corresponding to the y coordinate we want to draw on w/o destroying data. ex: 0000 1000 -> 0000 1100
      }
      scene[j*128 + i] |= byte; // sets the byte for the corresponding segment to the byte. New page every 128 array index which is indicated by j, i indicates x.
    } // repeat pages*
  } // repeat x*
}