#include <stdio.h>
#include "checkers.h"

Piece get_piece(Game_state *state, Position pos)
{
    if (is_valid_position(pos))
        return state->board[pos.row][pos.col];
    return -1;
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
    state->current_player = WHITE;
    state->situation = ONGOING;
}


void update_situation(Game_state *state)
{
    int white_piece_count = 0;
    int black_piece_count = 0;

    Position p;
    for (p.row = 0; p.row < BOARD_SIZE; p.row++)
    {
        for (p.col = 0; p.col < BOARD_SIZE; p.col++)
        {
            Piece piece = get_piece(state, p);
            /**/ if (is_white(piece)) white_piece_count++;
            else if (is_black(piece)) black_piece_count++;
        }
    }

    /**/ if (white_piece_count == 0) state->situation = BLACK_WINS;
    else if (black_piece_count == 0) state->situation = WHITE_WINS;
    else                             state->situation = ONGOING;
}


void switch_player(Game_state *state)
{
    if (state->current_player == WHITE)
        state->current_player = BLACK;
    else
        state->current_player = WHITE;
}


// FIXME this is apparently not working
// when a stone gets to the other side
// it DOES NOT get upgraded into a dame
// as it should...

void upgrade_stones_to_dames(Game_state *state)
{
    // When a stone gets all the way to the opposite side of the board,
    // it gets upgraded to a dame.
    // This function implements that. It iterates simultaneously
    // through the top row to upgrade white stones, and
    // through the bottom row to upgrade black stones

    Position top    = { 7, 0 };
    Position bottom = { 0, 0 };

    for (int col = 0; col < 7; col++) {
        top.col = col;
        Piece top_piece = get_piece(state, top);
        if (top_piece == WHITE_STONE)
            set_piece(state, top, WHITE_DAME);

        bottom.col = col;
        Piece bottom_piece = get_piece(state, bottom);
        if (bottom_piece == BLACK_STONE)
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
    int vdir = (dest.row > src.row) ? 1 : -1;
    int hdir = (dest.col > src.col) ? 1 : -1;

    // ... by making the squares between src and dest empty
    Position captured = { src.row + vdir, src.col + hdir };
    for (int i=1; i<distance; i++)
    {
        set_piece(state, captured, EMPTY);
        captured.row += vdir;
        captured.col += hdir;
    }
}


