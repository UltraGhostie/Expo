// Written by Theodor Björkman 2023-03-01
#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

int counter = 0;
uint8_t del = 0;
uint8_t select = 0;
uint8_t check = 0;
uint8_t innum[] = {0, 0, 0};
char init[] = "AAA";
char scrbrd[4][4] = {"AAA", "AAA", "AAA"};
char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
int playtimes[] = {0, 0, 0};
char playtimestring[6][6] = {"00:00", "00:00", "00:00"};

void game(void)
{
    clearscene();
    updateentities();
    display_scene(scene);
}

void victoryscreen(void)
{
    clearscene();
    switch (winner)
    {
    case 1:
        display_string(0, "P1 won");
        break;

    case 2:
        display_string(0, "P2 won");
        if (gamemode == 1)
            display_string(0, "AI won");
        break;
    }

    int accessinit = 1;
    if (gamemode == 1 || time < playtimes[2])
        accessinit = 0;

    display_string(1, "btn1: reset");
    display_string(2, "btn2: enter init");
    if (!accessinit)
        display_string(2, "");
    display_string(3, "btn3: main menu");
    display_update();

    if (counter < 100)
    {
        counter++;
        return;
    }

    switch (getbtns())
    {
    case 1:
        sceneselect = 0;
        counter = 0;
        time = 0;
        break;

    case 2:
        if (!accessinit)
            break;
        sceneselect = 3;
        counter = 0;
        break;

    case 4:
        sceneselect = 2;
        counter = 0;
        break;
    }
}

void mainmenu(void)
{
    time = 0;
    display_string(0, "      PONG");
    display_string(1, "btn1:play");
    display_string(2, "btn2:play v AI");
    display_string(3, "btn3:scoreboard");
    display_update();

    if (counter < 100)
    {
        counter++;
        return;
    }

    switch (getbtns())
    {
    case 1:
        sceneselect = 0;
        counter = 0;
        gamemode = 0;
        break;

    case 2:
        sceneselect = 4;
        counter = 0;
        break;

    case 4:
        sceneselect = 5;
        counter = 0;
        break;
    }
}

void enterinit(void)
{
    if (counter < 500 && check == 0) // Show message for 5 seconds
    {
        counter++;
        display_string(0, "btn1:right");
        display_string(1, "btn2:down");
        display_string(2, "btn3:up");
        display_string(3, "btn4:left");
        display_update();
        return;
    }

    check = 1;
    counter++;
    display_string(0, itoaconv((3500 - counter) / 100)); // shows timer of 30 seconds
    if (del < 25)                                        // anti-ghosting
        del++;
    switch (getbtns()) // button logic
    {
    case 1: // move selector right
        if (select < 2 && del == 25)
        {
            select++;
            del = 0;
        }
        break;

    case 2: // increase selected char and if it is Z loop
        if (del != 25)
            break;
        if (innum[select] == 25)
        {
            innum[select] = 0;
            del = 0;
            break;
        }
        innum[select]++;
        del = 0;
        break;

    case 4: // decrease selected char and if it is A loop
        if (del != 25)
            break;
        if (innum[select] == 0)
        {
            innum[select] = 25;
            del = 0;
            break;
        }
        innum[select]--;
        del = 0;
        break;

    case 8: // move selector left
        if (select > 0 && del == 25)
        {
            select--;
            del = 0;
        }
        break;
    }
    switch (select) // show selected char
    {
    case 0:
        display_string(2, "v");
        break;

    case 1:
        display_string(2, " v");
        break;

    case 2:
        display_string(2, "  v");
        break;
    }

    int8_t i = 2;
    while (i >= 0)
    {
    init[i] = alphabet[innum[i]];
    i--;
    }

    display_string(1, "");
    display_string(3, init); // show current initials
    display_update();

    if (counter > 3500) // if time runs out save initials and time and then return to main menu
    {
        check = 0;
        sceneselect = 2;
        counter = 0;
        uint8_t c = 1; // checks which slot should be used
        if (time >= playtimes[2])
            c = 2;
        if (time >= playtimes[1])
            c = 1;
        if (time >= playtimes[0])
            c = 0;
        
        i = 2;
        int8_t j;
        while (i > c) // moves scores down if needed
        {
            j = 0;
            while (j < 3) // letter by letter
            {
                scrbrd[i][j] = scrbrd[i-1][j];
                j++;
            }
            j = 0;
            while (j < 5) // moves times down if needed
            {
                playtimestring[i][j] = playtimestring[i-1][j];
                j++;
            }
            i--;
        }
        
        i = 0;
        while (i < 3) // sets slot to right initials
        {
            scrbrd[c][i] = init[i];
            i++;
        }

        // sets timestring and time in right slots
        i = 5;
        while (i > 0)
        {
            playtimestring[c][i] = timestring[i];
            i--;
        }
        playtimes[c] = time;
    }
}

void scoreboard(void)
{
    display_string(0, "Anybtn: return");
    display_string(1, scrbrd[0]);
    display_string(2, scrbrd[1]);
    display_string(3, scrbrd[2]);
    counter++;
    if ((counter / 200) % 2 != 0)
    {
        display_string(1, playtimestring[0]);
        display_string(2, playtimestring[1]);
        display_string(3, playtimestring[2]);
    }
    
    display_update();
    if (counter < 50)
    {
        return;
    }
    if (getbtns())
    {
        sceneselect = 2;
        counter = 0;
    }
}

void selectdifficulty(void)
{
    display_string(0, "btn1:easy");
    display_string(1, "btn2:medium");
    display_string(2, "btn3:hard");
    display_string(3, "btn4:return");
    display_update();

    if (counter < 50)
    {
        counter++;
        return;
    }

    switch (getbtns())
    {
    case 1:
        gamemode = 1;
        difficulty = 0;
        sceneselect = 0;
        counter = 0;
        break;

    case 2:
        gamemode = 1;
        difficulty = 1;
        sceneselect = 0;
        counter = 0;
        break;

    case 4:
        gamemode = 1;
        difficulty = 2;
        sceneselect = 0;
        counter = 0;
        break;

    case 8:
        sceneselect = 2;
        counter = 0;
        break;
    }
}