#include <ncurses.h>
#include <string.h>

static WINDOW* board_win;

struct {
	WINDOW *win;
	int rows, cols;
	int y, x;
} prompt_win;


void init_interface()
{
	initscr();
	cbreak();
	noecho();

	// Board occupies 2/3 of the screen // Prompt occupies 1/3 of the screen
	// 'bound' gives where one ends and the other starts
	int bound = 2*COLS/3;
	board_win = newwin(LINES, bound, 0, 0);

	prompt_win.rows = LINES;
	prompt_win.cols = COLS - bound;
	prompt_win.win = newwin( prompt_win.rows, prompt_win.cols, 0, bound );
	prompt_win.y = 0;
	prompt_win.x = 0;

	// box(board_win, 0, 0);
	// box(prompt_win, 0, 0);
}

void refresh_interface()
{
	refresh();
	wrefresh(board_win);
	wrefresh(prompt_win.win);
}

static void clear_prompt_win()
{
	wclear(prompt_win.win);
	prompt_win.y = 0;
	prompt_win.x = 0;
}

static void addch_prompt_win(char c)
{
	// Just so I don't have to write prompt_win.(...) all the time
	int *y = &prompt_win.y;
	int *x = &prompt_win.x;

	mvwaddch(prompt_win.win, *y, *x, c);
	++*x;
	if (*x >= prompt_win.cols) {
		// Go to next line
		++*y;
		*x = 0;
	}
	if (*y >= prompt_win.rows) {
		// Restart at the top
		*y = 0;
		clear_prompt_win();
	}
}

static int chars_left()
{
	int n = 0;
	// Characters left in the current line
	n += prompt_win.cols - prompt_win.x;
	// Characters left in lines *below* (that's the reason for the -1)
	n += (prompt_win.rows - prompt_win.y - 1) * (prompt_win.cols);
	return n;
}

void prompt(char *s)
{
	// A string may be printed halfway through,
	// reach the end of the window and thus make the window be cleared.
	// Then only the other half of the string appears at the top.
	// Doing this vvv prevents that ^^^ from happening.
	if (strlen(s) > chars_left()) {
		clear_prompt_win();
	}

	while (*s) {
		addch_prompt_win(*s);
		++s;
	}

	wrefresh(prompt_win.win);
}

void prompt_test()
{
	while (true) {
		prompt("Hello, world! ");
		getch();
	}
}

void close_interface()
{
	delwin(board_win);
	delwin(prompt_win.win);
	endwin();
}
