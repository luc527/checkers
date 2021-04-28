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
 * stored in playery and playerx.
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
 * So its methods, instead of taking a Board_space as parameter,
 * operate on /the/ game's board space, which is stored
 * in the variable bpsace.
 */
Board_space bspace;


/* bspace_reset just clears the board space movement information
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
	bspace.win = newwin(0, 0, 0, 0);
	bspace_reset();
}

// what if the terminal isn't large enough to display the whole board?
// TODO do something to handle this situation

/* bspace_show() loads the visual representation of the board
 * space into its window.
 */
void bspace_show()
{
	// Move to the start of the window,
	// otherwise it'll show the updated board below the previous one
	wmove(bspace.win, 0, 0);

	for (int row = 0; row < 8; row++) {
		// Row separator
		waddch(bspace.win, '+');
		for (int col = 0; col < 8; col++)
			waddstr(bspace.win, "---+");
		waddch(bspace.win, '\n');

		for (int col = 0; col < 8; col++) {
			chtype ch;
			// TODO do some combination of blinking and reversing the color instaed 
			// to indicate where the source and destination are
			if (bspace.chose_dest && row == bspace.desty && col == bspace.destx)
				ch = 'd';
			else if (bspace.chose_src && row == bspace.srcy && col == bspace.srcx)
				ch = 's';
			else
				ch = ' ';
			waddch(bspace.win, '|');
			if (row == bspace.playery && col == bspace.playerx)
				wattron(bspace.win, A_REVERSE);
			waddch(bspace.win, ' ');
			waddch(bspace.win, ch);
			waddch(bspace.win, ' ');

			wattroff(bspace.win, A_REVERSE);
		}
		waddch(bspace.win, '|');
		waddch(bspace.win, '\n');
	}
	waddch(bspace.win, '+');
	for (int col = 0; col < 8; col++)
		waddstr(bspace.win, "---+");
	waddch(bspace.win, '\n');
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

/* bspace_cancel_movement() is called to cancel the player's current movement.
 * Calling it will enable the player to choose a different movement.
 */
void bspace_cancel_movement()
{
	bspace.chose_src = false;
	bspace.chose_dest = false;
}

void init_interface()
{
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	bspace_init();
}

void close_interface()
{
	delwin(bspace.win);
	endwin();
}

void refresh_interface()
{
	// This is *the correct order* in which to call these functions.
	// Under other orders when refresh_interface is called for
	// the first time we don't see the board yet, and it has
	// to be called again for it to actually be shown.
	bspace_show();
	refresh();
	wrefresh(bspace.win);
}


void get_movement_interactively(Position *src, Position *dest) 
{
	bspace.playery = bspace.playerx = 0;
	// TODO remember previous position instead

	bspace_reset();
	refresh_interface();

	// TODO write somewhere what keys the player has to press
	// and what they do (e.g. press wasd to move around, m to
	// mark the current position as source or destination,
	// u to undo the movement and ENTER when you're done)
	while (true) {
		chtype ch = wgetch(bspace.win);
		int yoffset = 0, xoffset = 0;
		switch (ch) {
		case 10:  // Enter
			if (bspace.chose_src && bspace.chose_dest)
				goto done;  // break wouldn't break the loop, but the case
			// TODO else: print that the player has to perform a movement
			break;
		case 'u':
			bspace_cancel_movement();
			break;
		case 'm':
			if (!bspace.chose_src)
				bspace_select_src();
			else if (!bspace.chose_dest)
				bspace_select_dest();
			// TODO else: status message with error (you must cancel the
			// movement or press ENTER at this point)
			break;
		case 'w': yoffset = -1; break;
		case 's': yoffset =  1; break;
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
	init_interface();

	Position src, dest;
	get_movement_interactively(&src, &dest);

	close_interface();

	// Some post-processing of the coordinates might have to be done
	// so that they match the actual positions in the board, but I'm
	// not sure yet.
	printf("(x,y) from 0 to 7\n");
	printf("Source: (%d,%d)\n", src.row, src.col);
	printf("Destination: (%d,%d)\n", dest.row, dest.col);

	return 0;
}

