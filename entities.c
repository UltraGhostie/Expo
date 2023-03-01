// Written by Theodor Björkman

#include <stdint.h>  
#include <pic32mx.h>  
#include "mipslab.h" 
#include <math.h>

int moveticks = 0;
int p1speed = 10;
int p2speed = 10;
int ballspeed = 2;
int start = 0;
// 0 = 2-player, 1 = vs AI
int gamemode = 0;

// p1
int p1x = 8;
int p1y = 12;
int p1xstart = 8;
int p1ystart = 12;

int p1xsize = 3;
int p1ysize = 8;

uint8_t p1[] = { 3, 8 };

// p2
int p2x = 116;
int p2y = 12;
int p2xstart = 116;
int p2ystart = 12;

int p2xsize = 3;
int p2ysize = 8;

uint8_t p2[] = { 3, 8 };

// ball

float ballx = 62;
float bally = 14;
float ballxstart = 62;
float ballystart = 14;

int ballxsize = 3;
int ballysize = 3;

uint8_t ball[] = { 3, 3};

double ballxv = 0;
double ballyv = 0;

// bounce on top/bottom
void bounce(void)
{
    ballyv = -ballyv;
}
/*contactpoint(which pixels/side of the ball touches player): 0 top, 1 side, 2 bot
player=1 for p1 player=2 for p2*/
void reflect(int player)
{
    // regular bounce
    int px, py, pxsize, pysize;

    /* yv */
    switch (player) // sets the stats to the relevant player
    {
    case 1:
        px = p1x;
        py = p1y;
        pxsize = p1xsize;
        pysize = p1ysize;
        ballx = p1x+p1xsize + 1;
        break;
    default:
        px = p2x;
        py = p2y;
        pxsize = p2xsize;
        pysize = p2ysize;
        ballx = p2x-ballxsize - 1;
        break;
    }
    float delta = (bally+(ballysize/2))-(py+(pysize/2)); // delta is negative if middle of ball is above middle of player and reverse for reverse
    float maxdelta = (pysize/2)+(ballysize/2); // this is if the outer most pixels touch
    float maxangle = 0.4; // sets the max angle to arcsin(maxangle)
    switch (delta > 0)
    {
    case 0:// delta is negative
        delta = -delta; // make it pos
        maxangle = -maxangle; // we want to shift it toward the opposite side otherwise the same op
        delta /= maxdelta; // put it between [0,1]
        delta *= (maxangle-ballyv); // increase range to [0,maxangle-ballyv] where ballyv is smallest angle we want
        delta += ballyv; // shift range to [ballyv,maxangle]
        break;
    
    default:// delta is positive
        delta /= maxdelta;
        delta *= (maxangle-ballyv);
        delta += ballyv;
        break;
    }
    ballyv = delta; // set to new y velocity
    /* xv */
    float prevxv = ballxv; // save previous xv so we can see if we need to invert it
    switch (ballyv < 0)
    {
    case 0: // if ballyv is pos
        ballxv = 1-ballyv; // keep total v to abs(1)
        break;
    
    default: // if ballyv is neg
        ballxv = -1-ballyv; // same here
        break;
    }
    switch (prevxv < 0) 
    {
    case 0: // prevxv is pos
        if (ballxv > 0) // new xv is also pos
            ballxv = -ballxv; // we dont want that
        break;
    
    default: // prevxv is neg
        if (ballxv < 0) // new xv is also neg
            ballxv = -ballxv; // we dont want that
        break;
    }
}
/* void reflect(int contactpoint, int player)
{
    switch (contactpoint)
    {
    case 0:
        ballyv = 0.9;
        ballxv = -(ballxv/ballxv)*0.1;
        bally = p1y+p1ysize;
        if (player == 2)
        {
            bally = p2y+p2ysize;
        }
        return;
        break;
    case 2:
        ballyv = -0.9;
        ballxv = -(ballxv/ballxv)*0.1;
        bally = p1y-ballysize;
        if (player == 2)
        {
            bally = p2y-ballysize;
        }
        
        return;
        break;
    default:
        break;
    }
    float yv = ballyv;
    float xv = ballxv;
    float maxdelta;
    double sin75 = 0.9659258263; // sharpest angles i want on yspeed
    double sin285 = -sin75; 
    double sina = yv; // cos(v) = x, v = arccos(x)
    if (sina > 0.9659258263) // if angle has some way become way too sharp it is set to a playable amount
        sina = 0.9612616959;
    if (sina < -0.9659258263)
        sina = -0.9612616959;
    float by = bally + ballysize/2;
    float delta;
    switch (player) // makes sure that we can change player sizes independently
    {
    case 1:
        py = p1y + (p1ysize/2);
        maxdelta = p1ysize/2;
        ballx = p1x+p1xsize;
        break;
    case 2:
        py = p1y + (p2ysize/2);
        maxdelta = p2ysize/2;
        ballx = p2x-ballxsize;
        break;
    default:
        return;
        break;
    } 
    delta = by-py;
    // Scale delta to a range between angle and 15 or 165 depending on if delta is positive or negative
    float min = 0; 
    float max;
    double newyv, newxv;
    if(delta > 0){
        newyv = delta/maxdelta;// this normalises it in the range [0,1]
        newyv = newyv*(sin75 - sina); // these next maps delta between sina and sin75 where sin75>sina
        newyv += sina;
    }
    if(delta < 0){
        newyv = newyv/maxdelta;// set it to pos
        newyv = newyv*(sina-sin285); // sina > sin285
        newyv += sin285;
    }

    newxv = 1-newyv;
    if (newyv < 0)
        newxv = 1+newyv;
    if ((xv < 0 && newxv < 0) || (xv > 0 && newxv > 0))
        newxv = -newxv;
    newxv = -newxv;

    ballyv = newyv;
    ballxv = newxv;
} */
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
    if (!start && btns != 0)
    {
        ballxv = -1;
        if (moveticks % 2 == 0)
            ballxv = 1;
        start = 1;
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
    if (!start && btns != 0)
    {
        ballxv = -1;
        if (moveticks % 2 == 0)
            ballxv = 1;
        start = 1;
    }
}

int checkbounce(void)
{
    if (bally<0)
    {
        return 1;
    }
    if (bally+ballysize>31)
    {
        return 1;
    }
    return 0;
}

int checkcollision(void)
{
    int px;
    int py;
    int pysize;
    int bx;
    int by;
    int p = 1;
    if (ballx > 31) // if ball is on right side then it has to bounce at p2
        p = 2;
    
    switch (p)
    {
    case 1: //case for p1
        px = p1x;
        py = p1y;
        pysize = p1ysize;
        bx = ballx;
        by = bally;

        // IF on paddle AND between top AND bottom of paddle return 1
        if((bx <= px+p1xsize) && (bx+ballxsize > px) && (by <= (py+pysize)) && (by+ballysize >= py))
            return 1;
        break;
    case 2:
        px = p2x;
        py = p2y;
        pysize = p2ysize;
        bx = ballx;
        by = bally;
        // IF on ppaddle AND between top AND bottom of paddle return 1
        if((bx+ballxsize >= px) && (bx < px+(p2xsize)) && (by <= (py+pysize)) && (by+ballysize >= py))
            return 1;
        break;
    }

    return -1;
}

void moveball(void)
{
    ballx += ballxv;
    bally += ballyv;
    if (ballx < 4)
    {
        ballx = 22;
    }
    if (ballx > 123)
    {
        ballx = 22;
    }
    
    if(checkbounce())
    {
        bounce();
        bally += ballyv*2;
    }

    int c = checkcollision();
    if(c == -1)
        return;
    int p = 1;
    if(ballx > 64)
        p = 2;
    reflect(p);
}
// calls the move functions
void movesprites(void)
{
    if ( moveticks % ballspeed == 0)
        moveball();
    if ( moveticks % p1speed  == 0)
        movep1();
    if ( moveticks % p2speed == 0)
        movep2();
    if ( moveticks == 100 )
        moveticks = 0;
    moveticks++;
}
// updates their position on the scene
void updateentities(void)
{
    movesprites();
    addsprite(p1, p1x, p1y);
    addsprite(p2, p2x, p2y);
    addsprite(ball,(uint8_t)ballx,(uint8_t)bally);
    PORTESET = start;
}