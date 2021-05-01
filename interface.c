#include <ncurses.h>
#include "checkers.h"


/* The board space is where the player will move their cursor
 * and choose what movement he is going to perform -- what
 * piece they're gonna move (the source) and where they're moving it
 * to (the destination).
 * The process of moving a piece is this (as implemented currently -- it'll get better):
 * - Player moves cursor over to the piece to be moved (the src)
 *   (bspace_move implements this)
 * - Player presses some key to select that piece,
 *   causing the program to store the coordinates of the piece in srcy and srcx
 *   and set chose_src to true.
 *   (bspace_select_src implements this)
 * - Player moves cursor over to the position where the piece will
 *   land (the destination)
 * - Player presses some key to select that position,
 *   thereby storing it in desty and destx and seting
 *   chose_dest to true.
 *   (bspace_select_dest implements this)
 * > Throughout the process, the player's current y and x coordinates are
 *   stored in playery and playerx.
 */
typedef struct {
    WINDOW *win;
    int playery, playerx;
    int srcy,    srcx;
    int desty,   destx;
    bool chose_src;
    bool chose_dest;
} Board_space;
// TODO since these y and x coordinates won't reflect actual screen
// coordinates, it'd be better to use the same data type I've been
// using to store game board positions -- the Position struct.

/* There's only one board space ever in the game.
 * So the bspace_ functions, instead of taking a Board_space as parameter,
 * operate on /the/ board space, which is stored in the bspace variable.
 */
Board_space bspace;


/* bspace_reset clears the board space movement information
 * but doesn't do anything with the window.
 */
void bspace_reset()
{
    bspace.srcy = bspace.srcx = 0;
    bspace.desty = bspace.destx = 0;
    bspace.playery = bspace.playerx = 0;
    bspace.chose_src = false;
    bspace.chose_dest = false;
}

// bspace_init is like the constructor.
void bspace_init()
{
    // For now it'll occupy the whole screen
    // Change when actually adding other things to the interface
    bspace.win = newwin(LINES/2, COLS/2, 0, 0);
    bspace_reset();
}

/* The isblacksquare bool array is a lookup table
 * that tells whether the given position is black or not.
 * In particular, it implements a checkerboard pattern.
 */
bool isblacksquare[8][8] = {
    { true, false, true, false, true, false, true, false },
    { false, true, false, true, false, true, false, true },
    { true, false, true, false, true, false, true, false },
    { false, true, false, true, false, true, false, true },
    { true, false, true, false, true, false, true, false },
    { false, true, false, true, false, true, false, true },
    { true, false, true, false, true, false, true, false },
    { false, true, false, true, false, true, false, true },
};

// With the current implementation the board will have its colors inverted,
// because currently the code sets A_REVERSE on when it's a black square,
// which makes sense for the light color schemes that I use, but will
// make the square white on terminals with a dark background.
// TODO display the board corretly on terminals with light AND dark background.

// what if the terminal isn't large enough to display the whole board?
// TODO check terminal size and if it's not large enough warn user and exit

/* bspace_show() loads the visual representation of the board
 * space into its window.
 */
void bspace_show()
{
    wmove(bspace.win, 0, 0);

    // Frame top -- 3*8 hyphens (each square is 3 characters wide)
    waddstr(bspace.win, ".------------------------.\n");

    for (int row = 7; row >= 0; row--) {
        waddch(bspace.win, '|');  // Frame left

        for (int col = 0; col < 8; col++) {
            // Left and right padding around the piece
            chtype left  = ' ';
            chtype right = ' ';
            /* They also indicates player (with [] around), source (with < at the left)
             * and destination (with > at the left) positions.
             */
            /* [NOTE: Alternative to < and > symbols:
             * language-dependent mnemonics defined in language.c
             * (like 's' for source and 'd' for destination)]
             */
            /* The order of the following three ifs matters with respect to the padding:
             * its effect is that, on the same square,
             * player cursor [] overwrites destination symbol > overwrites source symbol <.
             */
            if (bspace.chose_src && row==bspace.srcy && col==bspace.srcx) {
                wattron(bspace.win, A_BLINK);
                left = '<';
            }
            if (bspace.chose_dest && row==bspace.desty && col==bspace.destx) {
                wattron(bspace.win, A_BLINK);
                left = '>';
            } 
            if (row == bspace.playery && col == bspace.playerx) {
                left  = '[';
                right = ']';
            }

            if (isblacksquare[row][col])
                wattron(bspace.win, A_REVERSE);

            waddch(bspace.win, left);
            waddch(bspace.win, ' '); // <- Here's where the pieces will be printed
            waddch(bspace.win, right);

            // Turn off all attributes that might have been turned on
            wattroff(bspace.win, A_BLINK | A_UNDERLINE | A_REVERSE);
        }
        waddch(bspace.win, '|');  // Frame right
        waddch(bspace.win, '\n');
    }

    // Frame bottom
    waddstr(bspace.win, "'------------------------'");
}

/* bspace_move is called to move the player's position in the
 * board according to the given offset. To move left, for instance,
 * yoffset would be 0 and xoffset would be 1.
 * The method guarantees that the final position is not out of bounds.
 */
void bspace_move(int yoffset, int xoffset)
{
    int newy = clamp(bspace.playery + yoffset, 0, 7);
    int newx = clamp(bspace.playerx + xoffset, 0, 7);

    bspace.playery = newy;
    bspace.playerx = newx;
}

/* bspace_select_src() is called to mark the player's current position
 * in the board space as the source of the movement to be performed.
 * It'll only work if the player hasn't chosen a source or a destination yet.
 * If he has already chosen a source he has to cancel the movement
 * to choose again.
 */
void bspace_select_src()
{
    if (!bspace.chose_dest && !bspace.chose_src) {
        bspace.srcy = bspace.playery;
        bspace.srcx = bspace.playerx ;
        bspace.chose_src = true;
    }
}

/* bspace_select_dest() is called to mark the player's current position
 * in the board space as the destination of the movement to be performed.
 * It'll only work if the player has already chosen a source and
 * hasn't yet chosen a destination. Again, if he wants to change
 * the destination he'll have to cancel the current movement first.
 */
void bspace_select_dest()
{
    if (bspace.chose_src && !bspace.chose_dest) {
        bspace.desty = bspace.playery;
        bspace.destx = bspace.playerx;
        bspace.chose_dest = true;
    }
}

/* bspace_undo_movement() is called to undo the player's last marked position.
 * If he has selected source and destination, calling it once will
 * just un-select the destination, and calling it once more will
 * un-select the source.
 */
void bspace_undo_movement()
{
    if (bspace.chose_dest) bspace.chose_dest = false;
    else bspace.chose_src = false;
}


// ------------------------------

// TODO change to extern when integrating with checkers.c
Language language = PT;

WINDOW *msgwin;

void msgwin_init()
{
    msgwin = newwin(LINES/2, COLS/2, LINES/2+1, 0);
}

void msgwin_print(char *msg)
{
    wclear(msgwin);
    wmove(msgwin, 0, 0);
    waddstr(msgwin, msg);
}


// TODO make a instructions window that tells what each key does


// ------------------------------


void init_interface()
{
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    msgwin_init();
    bspace_init();
}

void close_interface()
{
    delwin(msgwin);
    delwin(bspace.win);
    endwin();
}

void refresh_interface()
{
    /* This is *the correct order* in which to call these functions.
     * Under other orders only a second call to refresh_interface
     * would display the board for the first time.
     */
    bspace_show();
    refresh();
    wrefresh(bspace.win);
    wrefresh(msgwin);
}


void get_movement_interactively(Position *src, Position *dest) 
{
    bspace.playery = bspace.playerx = 0;
    // TODO remember previous position instead
    // easy with a static variable

    bspace_reset();
    refresh_interface();

    // TODO write somewhere what keys the player has to press
    // and what they do (e.g. "press wasd to move around, m to
    // mark the current position as source or destination,
    // u to undo the movement and ENTER when you're done")
    while (true) {
        chtype ch = wgetch(bspace.win);
        int yoffset = 0, xoffset = 0;
        switch (ch) {

            // FIXME msgwin_print is working, but not with getmsg

        case 10:  // Enter
            if (bspace.chose_src && bspace.chose_dest)
                goto done;  // 'break' wouldn't break the loop, but the switch case
            else
                msgwin_print(getmsg(MUST_SELECT_MOVEMENT, language));
        case 'u':
            bspace_undo_movement();
            break;
        case 'm':
            if (!bspace.chose_src)
                bspace_select_src();
            else if (!bspace.chose_dest)
                bspace_select_dest();
            else
                msgwin_print(getmsg(ALREADY_SELECTED_MOVEMENT, language));
            break;
        case 'w': yoffset =  1; break;
        case 's': yoffset = -1; break;
        case 'd': xoffset =  1; break;
        case 'a': xoffset = -1; break;
        }
        bspace_move(yoffset, xoffset);
        refresh_interface();
    }
done:
    src->row = bspace.srcy;
    src->col = bspace.srcx;
    dest->row = bspace.desty;
    dest->col = bspace.destx;
}


int main()
{
    init_messages_array();
    init_interface();

    Position src, dest;
    get_movement_interactively(&src, &dest);

    close_interface();

    printf("(x,y) from 0 to 7\n");
    printf("Source: (%d,%d)\n", src.col, src.row);
    printf("Destination: (%d,%d)\n", dest.col, dest.row);

    return 0;
}

