#include <stdio.h>
#include <ctype.h>

#include "checkers.h"


int abs(int x)
{
    return x < 0 ? -x : x;
}


int clamp(int x, int min, int max)
{
	if      (x < min)  return min;
	else if (x > max)  return max;
	else               return x;
}

void print_indentation(int i)
{
    while (i-- > 0) putchar(' ');
}

char* write_position(Position *p, char* buf)
{
    sprintf(buf, "%c%c", p->col+'A', p->row+'1');
    return buf;
}

//
// Predicates
//

bool is_valid_position(Position p)
{ return p.row >= 0 && p.row < BOARD_SIZE
      && p.col >= 0 && p.col < BOARD_SIZE; }

bool is_diagonal(Position src, Position dest)
{ return abs(src.row - dest.row) == abs(src.col - dest.col); }

// Consider storing these pieces of information in bits of the piece integer?

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
