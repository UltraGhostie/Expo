/* mipslab.h
   Header file for all labs.
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   Latest update 2015-08-28 by F Lundevall

   For copyright and licensing, see file COPYING */

/* Declare display-related functions from mipslabfunc.c */
void display_image(int x, const uint8_t *data);
void display_init(void);
void display_string(int line, char *s);
void display_update(void);
uint8_t spi_send_recv(uint8_t data);

/* Declare lab-related functions from mipslabfunc.c */
char * itoaconv( int num );
void labwork(void);
int nextprime( int inval );
void quicksleep(int cyc);
void tick( unsigned int * timep );

/* Declare display_debug - a function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/
void display_debug( volatile int * const addr );

/* Declare bitmap array containing font */
extern const uint8_t const font[128*8];
/* Declare bitmap array containing icon */
extern const uint8_t const icon[128];
/* Declare text buffer for display output */
extern char textbuffer[4][16];

/* Declare functions written by students.
   Note: Since we declare these functions here,
   students must define their functions with the exact types
   specified in the laboratory instructions. */
/* Written as part of asm lab: delay, time2string */
void delay(int);
void time2string( char *, int );
/* Written as part of i/o lab: getbtns, getsw, enable_interrupt */
int getbtns(void);
int getsw(void);
void enable_interrupt(void);

// Define variables for project. Everything below is written by Theodor Björkman
uint8_t scene[512]; // Scene for game
int timeoutcount; // used for timing purposes
int winner; // used to show winner
int sceneselect; // 0 = game, 1 = victory screen, 2 = main menu, 3 = enter initials, 4 = select difficulty, 5 = scoreboard
int counter; // used to avoid going to a menu and then instantly selecting an option
int difficulty; // difficuly level of AI
int gamemode; // 0 is pvp 1 is pvAI

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