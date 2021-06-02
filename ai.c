#include "checkers.h"


void printpos(Position *p)
{
    printf("%c%c ", p->col+'A', p->row+'1');
}


int evaluate(Game_state* state, Color player)
{
    static int stone_value = 10;
    static int dame_value = 25;
    static int distance_bonus = 1;
    static int white_origin = 0;
    static int black_origin = 7;

    int state_value = 0;

    Position pos;
    for (int pos.row = 0; pos.row < 8; pos.row++)
    {
        for (int pos.col = 0; pos.col < 8; pos.col++)
        {
            Piece piece = get_piece(state, pos);

            if (!is_empty(piece))
            {
                int piece_value;
                if (is_stone(piece))  piece_value = stone_value;
                else                  piece_value = dame_value;

                int dist;  // distance from origin
                if (is_white(piece))  dist = p.row - white_origin;
                else                  dist = black_origin - p.row;

                piece_value += dist * distance_bonus;

                if (!piece_matches_player(piece, player))  piece_value *= -1;

                state_value += piece_value;
            }
        }
    }

    return state_value;
}
