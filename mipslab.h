// Everything from here until otherwise stated is premade from labs
/* mipslab.h
   Header file for all labs.
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   Latest update 2015-08-28 by F Lundevall

   For copyright and licensing, see file COPYING */

/* Declare display-related functions from mipslabfunc.c */
void display_init(void);
void display_string(int line, char *s);
void display_update(void);
uint8_t spi_send_recv(uint8_t data);

/* Declare lab-related functions from mipslabfunc.c */
char * itoaconv( int num );
void labwork(void);
void tick( unsigned int * timep );

/* Declare text buffer for display output */
extern char textbuffer[4][16];
/* Declare bitmap array containing font */
extern const uint8_t const font[128*8];

/* Declare functions written by students.
   Note: Since we declare these functions here,
   students must define their functions with the exact types
   specified in the laboratory instructions. */
/* Written as part of asm lab: delay, time2string */
void delay(int);
void time2string( char *, int );
/* Written as part of i/o lab: getbtns, getsw, enable_interrupt */

// Everything below is written by Theodor Björkman. Last modified 2023-03-01
uint8_t getbtns(void);
uint8_t getsw(void);
void enable_interrupt(void);


// Define variables for project. 
uint8_t scene[512]; // Scene for game
uint8_t winner; // used to show winner
uint8_t sceneselect; // 0 = game, 1 = victory screen, 2 = main menu, 3 = enter initials, 4 = select difficulty, 5 = scoreboard
int counter; // used to avoid going to a menu and then instantly selecting an option
uint8_t difficulty; // difficuly level of AI
uint8_t gamemode; // 0 is pvp 1 is pvAI
char timestring[6]; // used for highscore
unsigned int time; // used for highscore

// Define functions for project.
void display_scene( uint8_t *data); // displays a scene
void clearscene(void); // clears the scene "scene"
void addsprite(uint8_t* sprite, uint8_t x, uint8_t y); // adds a sprite to the scene "scene"
void updateentities(void); // used to run the game iself

//scenes
void game(void); // game
void victoryscreen(void); // after a game has been ended by one player getting 5 points
void mainmenu(void); // can go to game, scoreboard or selectdifficulty from here
void enterinit(void); // allows input of 3 initials into scoreboard
void scoreboard(void); // shows winners who have inputed initials
void selectdifficulty(void); // select AI difficulty