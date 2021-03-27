#include <stdio.h>
#include <ctype.h>

#include "checkers.h"


int abs(int x)
{
    return x < 0 ? -x : x;
}

//
// Position reading and writing
//

// red_position(pos): read a position from the user and store it into pos.
// Returns whether the user wrote a position in a valid format,
// but not whether the position actually exists (will return true for
// R9, for instance). TODO change this...
bool read_position(Position *pos)
{
    // Columns go from A to H
    // Rows go from 1 to 8
    // We use the fact that characters are encoded as integers
    // and sequential characters ('a', 'b', 'c', '1', '2', '3' etc.)
    // are also ordered sequentially in the caracter set. 
    
    bool valid_position = true;

    int col = getchar();
    if (!isalpha(col))
        valid_position = false;
    col = toupper(col);
    col -= 'A';

    int row = getchar();
    if (!isdigit(row))
        valid_position = false;
    row -= '1';

    pos->row = row;
    pos->col = col;

    while (getchar() != '\n')
        ;

    return valid_position;
}


void print_position(Position p)
{
    printf("%c%c ", p.col+'A', p.row+'1');
}

//
// Predicates
//

bool is_valid_position(Position p)
{ return p.row >= 0 && p.row < 8
      && p.col >= 0 && p.col < 8; }

bool is_diagonal(Position src, Position dest)
{ return abs(src.row - dest.row) == abs(src.col - dest.col); };

bool is_white(Piece piece)
{ return piece == WHITE_STONE || piece == WHITE_DAME; }

bool is_black(Piece piece)
{ return piece == BLACK_STONE || piece == BLACK_DAME; }

bool is_stone(Piece piece)
{ return piece == WHITE_STONE || piece == BLACK_STONE; }

bool is_dame(Piece piece) 
{ return piece == WHITE_DAME || piece == BLACK_DAME; }

bool is_empty(Piece piece)
{ return piece == EMPTY; }

bool same_color(Piece piece1, Piece piece2)
{ return (is_white(piece1) && is_white(piece2))
      || (is_black(piece1) && is_black(piece2)); }

bool piece_matches_player(Piece piece, Color player)
{ return (is_white(piece) && player == WHITE)
      || (is_black(piece) && player == BLACK); }

