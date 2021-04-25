#include <ncurses.h>
#include "checkers.h"


typedef struct {
	WINDOW *window;
	int srcy,    srcx;
	int desty,   destx;
	bool chose_src;
	bool chose_dest;
} Board_interaction;

Board_interaction board;


void board_init()
{
	board.window = newwin(8, 8, 1, 1);
	board.srcy = board.srcx = 0;
	board.desty = board.destx = 0;
	board.chose_src = board.chose_dest = false;
	wmove(board.window, 0, 0);
}

void board_show()
{
	// Showing the board will move the cursor
	// around, but the cursor is where the player
	// is at and we don't want to change that.
	// So here we save the current y and x positions
	// and restore them later, preserving the player positions.

	// Save player position
	int playery, playerx;
	getyx(board.window, playery, playerx);

	// Erase previous state
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			mvwaddch(board.window, y, x, ' ');
		}
	}
	box(board.window, 0, 0);
	// Print new state (just indicates current player position and src and dest)
	if (board.chose_src) {
		mvwaddch(board.window, board.srcy, board.srcx, 's');
		if (board.chose_dest) {
			mvwaddch(board.window, board.desty, board.destx, 'd');
		}
	}
	mvwchgat(board.window, playery, playerx, 1, A_REVERSE, 0, NULL);

	// Restore player position
	wmove(board.window, playery, playerx);
}

void board_move(int yoffset, int xoffset)
{
	int cury, curx;
	getyx(board.window, cury, curx);

	int height, width;
	getmaxyx(board.window, height, width);

	int newy = clamp(cury + yoffset, 0, height-1);
	int newx = clamp(curx + xoffset, 0, width-1);

	wmove(board.window, newy, newx);
}

void board_select_src()
{
	if (!board.chose_dest && !board.chose_src) {
		int y, x;
		getyx(board.window, y, x);

		board.srcy = y;
		board.srcx = x;

		board.chose_src = true;
	}
}

void board_select_dest()
{
	if (board.chose_src && !board.chose_dest) {
		int y, x;
		getyx(board.window, y, x);

		board.desty = y;
		board.destx = x;

		board.chose_dest = true;
	}
}

void board_cancel_movement()
{
	board.chose_src = false;
	board.chose_dest = false;
}

void init_interface()
{
	initscr();
	cbreak();
	noecho();
	board_init();
}

void close_interface()
{
	delwin(board.window);
	endwin();
}

void refresh_interface()
{
	board_show();
	wrefresh(board.window);
	refresh();
}


int test_interface()
{
	/*board_move(0, 0),*/ refresh_interface(), getch();
	// FIXME ^ this call doesn't show the interface yet and I don't know why

	// TODO better environment for debugging the interface
	// and understand what's happening

	// TODO test edge cases -- moving out of bounds,
	// selecting dest without having selected src,
	// or selecting twice etc.
	board_move(1, 1), refresh_interface(), getch();
	board_move(1, 1), refresh_interface(), getch();
	board_select_src(), refresh_interface(), getch();
	board_move(1, 1), refresh_interface(), getch();
	board_move(1, 1), refresh_interface(), getch();
	board_select_dest(), refresh_interface(), getch();
	board_move(1, 1), refresh_interface(), getch();
	board_move(1, 1), refresh_interface(), getch();
	board_cancel_movement(), refresh_interface(), getch();
}


int main()
{
	init_interface();
	test_interface();
	close_interface();
}

