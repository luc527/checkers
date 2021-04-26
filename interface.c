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
 */
typedef struct {
	WINDOW *window;
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


// bspace_init is like the constructor.
void bspace_init()
{
	bspace.window = newwin(8, 8, 1, 1);
	bspace.srcy = bspace.srcx = 0;
	bspace.desty = bspace.destx = 0;
	bspace.chose_src = bspace.chose_dest = false;
	wmove(bspace.window, 0, 0);
}

/* bspace_show() loads the visual representation of the board
 * space into its window.
 */
void bspace_show()
{
	// Showing the board will move the cursor
	// around, but the cursor is where the player
	// is at and we don't want to change that.
	// So here we save the current y and x positions
	// and restore them later, preserving the player positions.

	// Save player position
	int playery, playerx;
	getyx(bspace.window, playery, playerx);

	// Erase previous state
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			mvwaddch(bspace.window, y, x, ' ');
		}
	}
	box(bspace.window, 0, 0);
	// Print new state (just indicates current player position and src and dest)
	if (bspace.chose_src) {
		mvwaddch(bspace.window, bspace.srcy, bspace.srcx, 's');
		if (bspace.chose_dest) {
			mvwaddch(bspace.window, bspace.desty, bspace.destx, 'd');
		}
	}
	mvwchgat(bspace.window, playery, playerx, 1, A_REVERSE, 0, NULL);

	// Restore player position
	wmove(bspace.window, playery, playerx);
}

/* bspace_move is called to move the player's position in the
 * board according to the given offset. To move left, for instance,
 * yoffset would be 0 and xoffset would be 1.
 * The method guarantees that the final position is not out of bounds.
 */
void bspace_move(int yoffset, int xoffset)
{
	int cury, curx;
	getyx(bspace.window, cury, curx);

	int height, width;
	getmaxyx(bspace.window, height, width);

	int newy = clamp(cury + yoffset, 0, height-1);
	int newx = clamp(curx + xoffset, 0, width-1);

	wmove(bspace.window, newy, newx);
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
		int y, x;
		getyx(bspace.window, y, x);

		bspace.srcy = y;
		bspace.srcx = x;

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
		int y, x;
		getyx(bspace.window, y, x);

		bspace.desty = y;
		bspace.destx = x;

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
	bspace_init();
}

void close_interface()
{
	delwin(bspace.window);
	endwin();
}

void refresh_interface()
{
	bspace_show();
	wrefresh(bspace.window);
	refresh();
}


int test_interface()
{
	/*bspace_move(0, 0),*/ refresh_interface(), getch();
	// FIXME ^ this call doesn't show the interface yet and I don't know why

	// TODO better environment for debugging the interface
	// and understand what's happening

	// TODO test edge cases -- moving out of bounds,
	// selecting dest without having selected src,
	// or selecting twice etc.
	bspace_move(1, 1), refresh_interface(), getch();
	bspace_move(1, 1), refresh_interface(), getch();
	bspace_select_src(), refresh_interface(), getch();
	bspace_move(1, 1), refresh_interface(), getch();
	bspace_move(1, 1), refresh_interface(), getch();
	bspace_select_dest(), refresh_interface(), getch();
	bspace_move(1, 1), refresh_interface(), getch();
	bspace_move(1, 1), refresh_interface(), getch();
	board_cancel_movement(), refresh_interface(), getch();
}


int main()
{
	init_interface();
	test_interface();
	close_interface();
}

