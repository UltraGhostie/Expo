// Written by Theodor Björkman

#include <stdint.h>  
#include <pic32mx.h>  
#include "mipslab.h" 
#include <math.h>
// 0 = 2-player, 1 = vs AI
int gamemode = 0;

// p1
int p1x = 8;
int p1y = 12;

int p1xsize = 3;
int p1ysize = 8;

uint8_t p1[] = { 3, 8 };

// p2
int p2x = 116;
int p2y = 12;

int p2xsize = 3;
int p2ysize = 8;

uint8_t p2[] = { 3, 8 };

// ball

int ballx = 62;
int bally = 14;

int ballxsize = 3;
int ballysize = 3;

uint8_t ball[] = { 3, 3};

float ballxv = 0;
float ballyv = 0;
// bounce on top/bottom
void bounce(void)
{
    bally = -bally;
}
/*contactpoint(which pixels/side of the ball touches player): 0 top, 1 side, 2 bot
player=1 for p1 player=2 for p2*/
void reflect(int contactpoint, int player)
{
    float xv = ballxv;
    int invert = 0;
    float mindelta = 0;
    float maxdelta;
    if (xv < 0) // makes sure that angle will be between 0 and 180
    {
        xv = -xv;
        invert = 1;
    }
    float angle = acosf(xv); // cos(v) = x, v = arccos(x)
    if (angle < 15) // if angle has some way become way too sharp it is set to a playable amount
        angle = 16;
    if (angle > 165)
        angle = 164;
    float py;
    float by = bally + ballysize/2;
    float delta;
    switch (player) // makes sure that we can change player sizes independently
    {
    case 1:
        py = p1y + (p1ysize/2);
        maxdelta = p1ysize/2;
        break;
    case 2:
        py = p1y + (p2ysize/2);
        maxdelta = p2ysize/2;
        break;
    default:
        return;
        break;
    } 
    delta = by-py;
    // Scale delta to a range between angle and 15 or 165 depending on if delta is positive or negative
    float min = 0; 
    float max;
    if(delta > 0){
        delta -= mindelta; // these first two normalises it in the range [0,1]
        delta = delta/(maxdelta-mindelta);
        delta = delta*(angle-15); // these next maps delta between the angle and 15 where angle > 15
        delta += 15;
        angle -= delta; // since it bounces on the top section and angle is always between 0 and 180 we subtract here
    }
    if(delta < 0){
        delta = -delta; // set it to pos
        delta -= mindelta;
        delta = delta/(maxdelta-mindelta);
        delta = delta*(165-angle); // here angle is < 165 so we subtract
        delta += 15;
        angle += delta; // opposite here, bounces on bottom section, 0<angle<180 so add
    }
}
// move functions are seperated as p2 should be able to disable input
void movep1(void)
{
    int btns = getbtns();
    btns = (btns & 0xc) >> 2;
    switch (btns) 
    {
    case 2: // btn4
        if(p1y != 0)
            p1y--;
        break;
    case 1: // btn3
        if(p1y+p1ysize != 31)
            p1y++;
        break;
    default: // both or none
        break;
    }
}

void movep2(void)
{
    if(gamemode == 1) // if AI is on, will call its own function later
        return;
    int btns = getbtns();
    btns &= 0x3;
    switch (btns)
    {
    case 2: // btn2
        if(p2y == 0)
            break;
            /* OOO we got a big one, breakdown:
            Will raising p2 put it at the same hight or higher than the bottom of ball
            AND
            Is it NOT over the ball
            AND
            Is the ball + ballsize at the same x or higher than p2 x
            AND 
            Is the ball NOT at a higher x than p2 + p2xsize
            AKA will it go into the ball in which case it doesnt*/
        if((p2y - 1 >= bally+ballysize) && !(p2y+p2ysize < bally) && (ballx+ballxsize >= p2x) && !(ballx > p2x+p2xsize))
            reflect(2,2);
            break;
        p2y--;
        break;
    case 1: // btn1
        if(p2y+p2ysize == 31)
            break;
        p2y++;
        break;
    default: // both or none
        break;
    }
}
// calls the move functions
void movesprites(void)
{
    movep1();
    movep2();
}
// updates their position on the scene
void updateentities(void)
{
    
    movesprites();
    addsprite(p1, p1x, p1y);
    addsprite(p2, p2x, p2y);
}