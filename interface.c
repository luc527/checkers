#include <ncurses.h>
#include "checkers.h"

extern Language language;

// Board space {{{

/* The board space is where the player will move their cursor and choose what
 * movement they're going to perform -- what piece they're gonna move (the
 * source) and where they're moving it to (the destination).
 * The process of moving a piece is this (as implemented currently -- it'll get better):
 * > Throughout the process, the player's current y and x coordinates are
 *   stored in playery and playerx.
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
/* TODO consider making the functions actually take a Board_space*,
 * but also provide macros that expand to calls of those functions with /the/
 * board space as parameter?
 */

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

/* bspace_init is like the constructor.  */
void bspace_init()
{
    bspace.win = newwin(LINES/2, COLS/2, 0, 0);
    bspace_reset();
}

/* The isblacksquare bool array is a lookup table
 * that tells whether the given position is black or not.
 * In particular, it implements a checkerboard pattern.
 */
bool isblacksquare[BOARD_SIZE][BOARD_SIZE] = {
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

/* piece_to_char associates each piece with a character that displays it. */
char piece_to_char[] = {
    [EMPTY]       = ' ',
    [WHITE_STONE] = 'o',
    [BLACK_STONE] = '*',
    [WHITE_DAME]  = '@',
    [BLACK_DAME]  = 'X'

};

/* bspace_show() loads the visual representation of the board
 * space into its window.
 */
void bspace_show(Game_state *state)
{   // {{{
    wmove(bspace.win, 0, 0);

    // Frame top -- 3*8 hyphens (each square is 3 characters wide)
    waddstr(bspace.win, ".------------------------.\n");

    for (int row = 7; row >= 0; row--) {
        waddch(bspace.win, '|');  // Frame left

        for (int col = 0; col < BOARD_SIZE; col++) {
            // Left and right padding around the piece
            chtype left  = ' ';
            chtype right = ' ';
            /* The padding also indicates player (with [] around), source (with < at the left)
             * and destination (with > at the left) positions.
             */
            /* [TODO consider alternative to < and > symbols:
             * language-dependent mnemonics defined in language.c
             * (like 's' for source and 'd' for destination)]
             */
            /* The /order/ of the following IFs matters with respect to the padding:
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
            Position pos = { row, col };
            waddch(bspace.win, piece_to_char[get_piece(state, pos)]);
            waddch(bspace.win, right);

            // Turn off all attributes that might have been turned on
            wattroff(bspace.win, A_BLINK | A_UNDERLINE | A_REVERSE);
        }
        waddch(bspace.win, '|');  // Frame right
        waddch(bspace.win, '\n');
    }

    // Frame bottom
    waddstr(bspace.win, "'------------------------'\n");

    // Show current player
    waddstr(bspace.win, getmsg(CURRENT_PLAYER, language));
    Message cur_player_msg = state->current_player == WHITE
                          ? WHITE_PLAYER : BLACK_PLAYER;
    waddstr(bspace.win, getmsg(cur_player_msg, language));
    waddch(bspace.win, '\n');
}   // }}}


/* bspace_select_src() is called to mark the player's current position
 * in the board space as the source of the movement to be performed.
 * It'll only work if the player hasn't chosen a source or a destination yet.
 * If he has already chosen a source he has to cancel the movement
 * to choose again.
 */
void bspace_select_src()
{
    if (!bspace.chose_dest && !bspace.chose_src)
    {
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
    if (bspace.chose_src && !bspace.chose_dest)
    {
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
    if (bspace.chose_dest)  bspace.chose_dest = false;
    else  bspace.chose_src = false;
}
// }}}

// Message window {{{
/* msgwin is the message window where messages will be displayed to the user,
 * warning them when he does something wrong, telling him if he 
 * must perform a capture etc.
 */
WINDOW *msgwin;

/* msgwin_init is msgwin's constructor */
void msgwin_init()
{
    msgwin = newwin(LINES/2, COLS/2, LINES/2+1, 0);
}

/* msgwin_print prints the given message on the messages window.
 * It overwrites the previous message.
 */
void msgwin_print(char *msg)
{
    wclear(msgwin);
    wmove(msgwin, 0, 0);
    waddstr(msgwin, msg);
}
// }}}

// Instructions and controls window {{{

/* instrwin is the instructions window where the instructions
 * and controls are displayed.
 */
WINDOW *instrwin;

/* instrwin_init is instrwin's constructor */
void instrwin_init()
{
    instrwin = newwin(LINES, COLS/2, 0, COLS/2+1);
    waddstr(instrwin, getmsg(INSTRUCTIONS, language));
}
// }}}

// Interface {{{
/* setup_interface configures ncurses and initializes each window.
 * It /expects initscr()/ to have already been called.
 */
void setup_interface()
{
    cbreak();
    noecho();
    curs_set(0);
    msgwin_init();
    instrwin_init();
    bspace_init();
}

/* close_interface deletes the windows and ends ncurses;
 * it /expects init_interface()/ to have been called previously. */
void close_interface()
{
    delwin(msgwin);
    delwin(instrwin);
    delwin(bspace.win);
    endwin();
}

/* refresh_interface updates the screen */
void refresh_interface(Game_state *state)
{
    /* This is *the correct order* in which to call these functions.
     * [Not sure precisely why; with other orders only a second call to
     * refresh_interface would display the board for the first time.] */
    bspace_show(state);
    refresh();
    wrefresh(bspace.win);
    wrefresh(msgwin);
    wrefresh(instrwin);
}


/* get_movement_interactively is the interaction loop where the player can move
 * around the board, select source and destination positions, and confirm the
 * movement, thereby loading the selected positions into the given src and dest
 * position pointers.  */
void get_movement_interactively(
        Game_state *state,
        Mov_options *mov_opts,
        Position *src,
        Position *dest
) 
{   //{{{
    bspace_reset();

	/* To choose a movement to perform the player first cycles through the
	 * pieces which he can move.  During this process, mov_opt_index stores the
	 * index of those pieces in the player options array.  After choosing which
	 * piece to move, the player cycles through its possible destinations.
	 * During this process, dest_opt_index stores the index of those positions
	 * in the dest_opts->array.  Along the whole interaction, 'cursor' is set
	 * to the option currently selected in the cycle (be it the mov_opts or
	 * dest_opts cycle).  */
    int mov_opt_index = 0;
    Position cursor = mov_opts->array[mov_opt_index].src;
    bspace.playery = cursor.row;
    bspace.playerx = cursor.col;

    Dest_options *dest_opts;
    int dest_opt_index;

    refresh_interface(state);

    while (true)
    {
        chtype ch = wgetch(bspace.win);
        switch (ch) {
        case ' ':
            // goto used because 'break' would break the switch case, not the loop
            if (bspace.chose_src && bspace.chose_dest) {
                goto done;  
            } else {
                msgwin_print(getmsg(MUST_SELECT_MOVEMENT, language));
            }
        case 'u':
            bspace_undo_movement();
            break;
        case 'm':
            if (!bspace.chose_src) {
                bspace_select_src();
                dest_opts = &mov_opts->array[mov_opt_index];
                dest_opt_index = 0;
            } else if (!bspace.chose_dest) {
                bspace_select_dest();
            } else {
                msgwin_print(getmsg(ALREADY_SELECTED_MOVEMENT, language));
            }
            break;
        case 'd':
            if (!bspace.chose_src) {
                ++mov_opt_index;
                mov_opt_index %= mov_opts->length;
            } else {
                ++dest_opt_index;
                dest_opt_index %= dest_opts->length;
            }
            break;
        case 'a':
            if (!bspace.chose_src) {
                --mov_opt_index;
                if (mov_opt_index < 0) {
                    mov_opt_index = mov_opts->length - 1;
                }
            } else {
                --dest_opt_index;
                if (dest_opt_index < 0) {
                    dest_opt_index = dest_opts->length - 1;
                }
            }
            break;
        }

        if (!bspace.chose_src) {
            cursor = mov_opts->array[mov_opt_index].src;
        } else {
            cursor = dest_opts->array[dest_opt_index];
        }
        bspace.playery = cursor.row;
        bspace.playerx = cursor.col;

        refresh_interface(state);
    }
done:
    src->row = bspace.srcy;
    src->col = bspace.srcx;
    dest->row = bspace.desty;
    dest->col = bspace.destx;

    wclear(msgwin);
}   // }}}
// }}}

