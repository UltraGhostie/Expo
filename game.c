// Written by Theodor Björkman

#include <stdint.h>  
#include <pic32mx.h>  
#include "mipslab.h" 

int difficulty;
int moveticks = 0;
int start = 0;
int score[] = { 0, 0 };
int winner;
// 0 = 2-player, 1 = vs AI
int gamemode = 0;

// p1
int p1x = 8;
int p1y = 12;
int p1ystart = 12;

int p1xsize = 3;
int p1ysize = 8;

uint8_t p1[] = { 3, 8 };
int p1speed = 10;

// p2
int p2x = 116;
int p2y = 12;
int p2ystart = 12;

int p2xsize = 3;
int p2ysize = 8;

uint8_t p2[] = { 3, 8 };
int p2speed = 10;

// ball

int ballspeed = 2;
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
    float delta = (bally+((float)ballysize/2))-(py+((float)pysize/2)); // delta is negative if middle of ball is above middle of player and reverse for reverse
    float maxdelta = (pysize/2)+(ballysize/2); // this is if the outer most pixels touch
    float maxangle = 0.4; // sets the max angle to arcsin(maxangle)
    if (delta < 1 && delta > 0) // set a deadzone in the middle
        delta = 0;
    if (delta > -1 && delta < 0)
        delta = 0;
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

void victory(int player)
{
    sceneselect = 1;
    winner = player;
    score[0] = 0;
    score[1] = 0;
}

void updatescoreboard(void)
{
    PORTECLR = 0xff;
    if ( score[0] >= 1)
        PORTESET = 1 << (8-score[0]);
    if ( score[1] >= 1)
        PORTESET = 1 << score[1] - 1;
}

void restart()
{
    ballyv = 0;
    ballxv = 0;
    ballx = ballxstart;
    bally = ballystart;

    p1y = p1ystart;
    p2y = p2ystart;

    start = 0;

    if ( score[0] == 5 )
        victory(1);
    if ( score[1] == 5 )
        victory(2);
    updatescoreboard();
    counter = 0;
    
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
    if (!start && btns != 0)
    {
        ballxv = -1;
        if ((score[0] + score[1]) % 2 == 0)
            ballxv = 1;
        start = 1;
    }
}

void changep2y(int direction) // 0 = up, 1 = down
{
    switch (direction)
    {
    case 0:
        if(p2y == 0)
            break;
        p2y--;
        break;
    
    case 1:
        if(p2y+p2ysize == 31)
            break;
        p2y++;
        break;
    }
}

void aimove(void)
{
    switch (difficulty)
    {
    case 0:
        p2speed = 15;
        break;
    
    case 1:
        p2speed = 13;
        break;

    case 2:
        p2speed = 10;
        break;
    }
    if (ballx < 70)
        return;
    switch (bally+(float)ballysize/2 < p2y + p2ysize/2)
    {
    case 0:
        changep2y(1);
        break;
    
    case 1:
        changep2y(0);
        break;
    }

   
    
}

void movep2(void)
{
    if(gamemode == 1)
    {
        aimove();
        return;
    }
    p2speed = 10;
    int btns = getbtns();
    btns &= 0x3;
    switch (btns)
    {
    case 2: // btn2
        changep2y(0);
        break;
    case 1: // btn1
        changep2y(1);
        break;
    default: // both or none
        break;
    }
    if (!start && btns != 0)
    {
        ballxv = -1;
        if ((score[0] + score[1]) % 2 == 0)
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

int getplayer(void)
{
    if (ballx > 63)
    {
        return 2;
    }
    return 1;
}

int checkcollision(void) // 1 front, 2 top, 3 bot
{
    int p = getplayer();
    
    switch (p)
    {
    case 1: //case for p1

        // check top: IF slightly behind paddle front AND on paddle top return 3
        if((ballx <= p1x+p1xsize-1) && (ballx+ballxsize > p1x) && (bally < ((float)p1y+((float)p1ysize/3))) && (bally+ballysize >= p1y))
            return 3;
        // check bot: IF slightly behind paddle front AND on paddle bottom return 2
        if((ballx <= p1x+p1xsize-1) && (ballx+ballxsize > p1x) && (bally > ((float)p1y+(2*(float)p1ysize/3)) && (bally <= p1y+p1ysize)))
            return 2;
        // check front: IF on paddle AND between top AND bottom of paddle return 1
        if((ballx <= p1x+p1xsize) && (ballx+ballxsize > p1x) && (bally <= (p1y+p1ysize)) && (bally+ballysize >= p1y))
            return 1;
        break;
    case 2:
        // check top: IF slightly behind paddle front AND on paddle top return 3
        if((ballx+ballxsize > p2x+1) && (ballx <= p2x+p2xsize) && (bally < ((float)p2y+((float)p2ysize/3))) && (bally+ballysize >= p2y))
            return 3;
        if((ballx+ballxsize > p2x+1) && (ballx <= p2x+p2xsize) && (bally > ((float)p2y+(2*((float)p2ysize/3)))) && (bally <= p2y+p2ysize))
            return 2;
        // IF on ppaddle AND between top AND bottom of paddle return 1
        if((ballx+ballxsize >= p2x) && (ballx < p2x+(p2xsize)) && (bally <= (p2y+p2ysize)) && (bally+ballysize >= p2y))
            return 1;
        break;
    }

    return -1;
}

void bouncebottom(int p)
{
    int py, pysize, direction;
    switch (p)
    {
    case 1:
        py = p1y;
        pysize = p1ysize;
        direction = 1;
        break;
    
    case 2:
        py = p2y;
        pysize = p2ysize;
        direction = -1;
        break;
    }
    bally = py+pysize+1;
    ballyv = 0.4;
    ballxv = direction*(1-ballyv);
}

void bouncetop(int p)
{
    int py, direction;
    switch (p)
    {
    case 1:
        py = p1y;
        direction = 1;
        break;
    
    case 2:
        py = p2y;
        direction = -1;
        break;
    }
    bally = py - ballysize - 1;
    ballyv = -0.4;
    ballxv = direction*(1+ballyv);
}

void moveball(void)
{
    ballx += ballxv;
    bally += ballyv;
    if (ballx < 4)
    {
        score[1]++;
        restart();
    }
    if (ballx > 123)
    {
        score[0]++;
        restart();
    }
    
    if(checkbounce())
    {
        bounce();
        bally += ballyv*2;
    }

    int c = checkcollision();
    int p = getplayer();
    switch(c)
    {
        case 1:     
            reflect(p);
            break;
        case 2:
            bouncebottom(p);
            break;
        case 3:
            bouncetop(p);
            break;
    }

}
// calls the move functions
void movesprites(void)
{
    if ( counter < 100 )
    {
        counter++;
        return;
    }
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
}