/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog
   This file modified 2023-03-01 by Theodor Björkman

   For copyright and licensing, see file COPYING */

#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "mipslab.h" /* Declatations for these labs */

// All variable declarations and content of all functions below are written by Theodor Björkman
uint8_t sceneselect = 2;

/* Interrupt Service Routine */

void user_isr(void)
{
  IFSCLR(0) = 0x100;
  labwork();
}

/* Lab-specific initialization goes here */
void labinit(void)
{
  TRISESET &= 0xffffff00;
  PORTE &= 0xffffff00;
  TRISD |= 0x000000e0; // T: enable all buttons
  TRISF |= 0x1;        // btn1

  // Timerstuff
  T2CON = 0x70;      // T: reset timer and set prescale to 1:256
  TMR2 = 0x0;        // T: clear timer register
  PR2 = 0xc35;       // T: 31'25. 80 MHz / 256 / 3125 = 100 Hz
  IFSCLR(0) = 0x100; // clear its flag
  IECSET(0) = 0x100; // enable interrupts from timer
  IPCSET(2) = 0xe;   // set priority for timer interrupts
  enable_interrupt();
  T2CONSET = 0x8000; // T: start timer

  clearscene(); // empty screen
  return;
}

// called by user_isr every timoutcount * 1/10 seconds
void labwork(void)
{
  switch (sceneselect)
  {
  case 0:
    game();
    break;

  case 1:
    victoryscreen();
    break;

  case 2:
    mainmenu();
    break;

  case 3:
    enterinit();
    break;

  case 4:
    selectdifficulty();
    break;

  case 5:
    scoreboard();
    break;
  }
}

// Functions below added by Theodor Björkman
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
void addsprite(uint8_t *sprite, uint8_t x, uint8_t y)
{
  // translates y coordinates to start page
  uint8_t page = 0;
  while (y > 7)
  {
    y -= 8;
    page++;
  }

  // checks if we are trying to draw outside the screen
  if (page<0 | page> 3)
    return;
  if (x<0 | x> 127 - sprite[0])
    return;

  // each page is 8 bits so this checks how many pages we need to draw
  int i;
  int pages = 1;
  for (i = 0; i <= sprite[1] + y; i++)
  {
    if (i > 8 * pages)
    {
      pages++;
    }
  }

  // this sets the right values in the canvas by or-ing them so they dont remove things that are already there
  int j, k, l;
  for (i = x; i < sprite[0] + x; i++) // steps x axis
  {
    l = sprite[1];
    for (j = page; j < page + pages; j++) // steps page
    {
      uint8_t byte = 0;
      for (k = 0; k < 8; k++) // sets data for page
      {
        if (k < y && j == page)
          continue; // skip bits so that we set y coordinates correctly
        if (l < 1)
          continue; // if we have drawn the entire length then we dont need to draw more
        l--;
        byte |= 1 << k; // set the bit corresponding to the y coordinate we want to draw on w/o destroying data. ex: 0000 1000 -> 0000 1100
      }
      scene[j * 128 + i] |= byte; // sets the byte for the corresponding segment to the byte. New page every 128 array index which is indicated by j, i indicates x.
    }                             // repeat pages*
  }                               // repeat x*
}