#include <stdio.h>
#include "checkers.h"

Piece get_piece(Game_state *state, Position pos)
{
    return is_valid_position(pos)
         ? state->board[pos.row][pos.col]
         : -1;
}


void set_piece(Game_state *state, Position pos, Piece piece)
{
    if (is_valid_position(pos))
        state->board[pos.row][pos.col] = piece;
}

// game_setup reads this to initailize the board
char initial_board[BOARD_SIZE][BOARD_SIZE+1] = {
    "o o o o ",  // white pieces
    " o o o o",
    "o o o o ",
    "        ",
    "        ",
    " * * * *",  // black pieces
    "* * * * ",
    " * * * *",
};

void game_setup(Game_state *state)
{ 
    Position p;
    for (p.row = 0; p.row < BOARD_SIZE; p.row++)
        for (p.col = 0; p.col < BOARD_SIZE; p.col++)
            switch (initial_board[p.row][p.col]) {
                case ' ': set_piece(state, p, EMPTY); break;
                case 'o': set_piece(state, p, WHITE_STONE); break;
                case '*': set_piece(state, p, BLACK_STONE); break;
                 
            }
    // Game rule: white goes first
    state->current_player = WHITE;
    state->situation = ONGOING;
}


void update_situation(Game_state *state)
{
    int white_piece_count = 0;
    int black_piece_count = 0;

    Position p;
    for (p.row = 0; p.row < BOARD_SIZE; p.row++) {
        for (p.col = 0; p.col < BOARD_SIZE; p.col++) {
            Piece piece = get_piece(state, p);
            if      (is_white(piece))  white_piece_count++;
            else if (is_black(piece))  black_piece_count++;
        }
    }

    if      (white_piece_count == 0)  state->situation = BLACK_WINS;
    else if (black_piece_count == 0)  state->situation = WHITE_WINS;
    else                              state->situation = ONGOING;
}


void switch_player(Game_state *state)
{
    if (state->current_player == WHITE)
        state->current_player = BLACK;
    else
        state->current_player = WHITE;
}


/* upgrade_stones_to_dames upgrades stones that have reached the opposite side of
 * the board into dames. */
void upgrade_stones_to_dames(Game_state *state)
{
    Position top    = { 7, 0 };
    Position bottom = { 0, 0 };

    for (int col = 0; col < 7; col++) {
        top.col = col;
        Piece attop = get_piece(state, top);
        if (attop == WHITE_STONE)
            set_piece(state, top, WHITE_DAME);

        bottom.col = col;
        Piece atbottom = get_piece(state, bottom);
        if (atbottom == BLACK_STONE)
            set_piece(state, bottom, BLACK_DAME);
    }
}


void perform_movement(Game_state *state, Position src, Position dest)
{
    // Move the piece
    set_piece(state, dest, get_piece(state, src));
    set_piece(state, src, EMPTY);

    // Perform the captures along the way... 
    int distance = abs(dest.row - src.row);
    int vstep = (dest.row > src.row) ? 1 : -1;  // Vertical step
    int hstep = (dest.col > src.col) ? 1 : -1;  // Horizontal step

    // ... by making the squares between src and dest empty
    Position captured = { src.row + vstep, src.col + hstep };
    for (int i = 1; i < distance; i++)
    {
        set_piece(state, captured, EMPTY);
        captured.row += vstep;
        captured.col += hstep;
    }
}


