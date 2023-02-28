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

float ballx = 62;
float bally = 14;

int ballxsize = 3;
int ballysize = 3;

uint8_t ball[] = { 3, 3};

double ballxv = 0;
double ballyv = 0;

// bounce on top/bottom
void bounce(void)
{
    bally = -bally;
}
/*contactpoint(which pixels/side of the ball touches player): 0 top, 1 side, 2 bot
player=1 for p1 player=2 for p2*/
void reflect(int contactpoint, int player)
{
    switch (contactpoint)
    {
    case 0:
        /* code */
        break;
    case 2:
        /*code*/
        break;
    default:
        break;
    }
    float yv = ballyv;
    float xv = ballxv;
    int invert = 0;
    float maxdelta;
    double sin75 = 0.9659258263;
    double sin285 = -sin75; 
    if (xv < 0) // makes xv pos for easier math
    {
        xv = -xv;
        invert = 1;
    }
    double sina = yv; // cos(v) = x, v = arccos(x)
    if (sina > 0.9659258263) // if angle has some way become way too sharp it is set to a playable amount
        sina = 0.9612616959;
    if (sina < -0.9659258263)
        sina = -0.9612616959;

    double cosa = xv;
    double cos75 = 0.2588190451;
    if (cosa > cos75)
        cosa = 0.2756373558;
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
    newxv = delta/maxdelta;
    newxv = newxv*(cosa - cos75); 
    newxv += cosa;

    ballyv = newyv;
    ballxv = newxv;
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
        px = p1x+p1xsize;
        py = p1y;
        pysize = p1ysize;
        bx = ballx;
        by = bally;
        if (bally < p1y)
            by += ballysize;

        // IF on paddle surface AND between top AND bottom of paddle return 1 for side
        if((bx == px) && (by <= (py+pysize)) && (by >= py))
            return 1;
        // IF behind paddle surface AND higher up than half of paddle AND lower than top of paddle return 0 for top
        if((bx < px) && (by < (py+(pysize/2))) && (by >= py))
            return 0;
        // IF behind paddle surface AND lower than half of paddle AND higher than bottom of paddle return 2 for bot
        if((bx < px) && (by > (py+(pysize/2))) && (by <= py+pysize))
            return 2;
        break;
    case 2:
        px = p2x;
        py = p2y;
        pysize = p2ysize;
        bx = ballx+ballxsize;
        by = bally;
        if (bally < p2y)
            by += ballysize;
        // IF on paddle surface AND between top AND bottom of paddle return 1
        if((bx == px) && (by <= (py+pysize)) && (by >= py))
            return 1;
        // IF behind paddle surface AND higher up than half of paddle AND lower than top of paddle return 0 for top
        if((bx > px) && (by < (py+(pysize/2))) && (by >= py))
            return 0;
        // IF behind paddle surface AND lower than half of paddle AND higher than bottom of paddle return 2 for bot
        if((bx > px) && (by > (py+(pysize/2))) && (by <= py+pysize))
            return 2;
        break;
    }

    return -1;
}

void moveball(void)
{
    ballx += ballxv;
    bally += ballyv;
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
    ballx -= ballxv;
    bally -= ballyv;
    reflect(c, p);
    ballx += ballxv;
    bally += ballyv;
}
// calls the move functions
void movesprites(void)
{
    moveball();
    movep1();
    movep2();
}
// updates their position on the scene
void updateentities(void)
{
    movesprites();
    addsprite(p1, p1x, p1y);
    addsprite(p2, p2x, p2y);
    addsprite(ball,(uint8_t)ballx,(uint8_t)bally);
}