#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "checkers.h"


// The logic here is probably wrong in several places. I need to test all of it.
// But the general idea is +/- ok I think.


/* returns board state value; puts selected movement in src and dest */
double minimax(Game_state* state, int depth, Color maximizing_player, Position* src, Position *dest)
{
    bool maximize = state->current_player == maximizing_player;
    // otherwise minimize

    Mov_options mov_options;
    generate_mov_options(state, &mov_options);

    if (mov_options.length == 0) {
        printf("No movement options available.\n");
        exit(EXIT_FAILURE);
    } else {
        for (int i = 0; i < mov_options.length; i++)
            if (mov_options.array[i].length == 0) {
                printf("No destinations available.\n");
                exit(EXIT_FAILURE);
            }
    }

    double sub_values[NUMPIECES][MAXOPTIONS];
    Position sub_srcs[NUMPIECES][MAXOPTIONS];
    Position sub_dests[NUMPIECES][MAXOPTIONS];

    Game_state sub_state;
    Position sub_src, sub_dest;
    for (int i = 0; i < mov_options.length; i++) {
        for (int j = 0; j < mov_options.array[i].length; j++) {
            game_copy(&sub_state, state);
            game_update(&sub_state, mov_options.array[i].src, mov_options.array[i].array[j]);
            if (depth > 0) {
                sub_values[i][j] = minimax(&sub_state, depth - 1, maximizing_player, &sub_src, &sub_dest);
                sub_srcs[i][j]   = sub_src;
                sub_dests[i][j]  = sub_dest;
            } else {
                sub_values[i][j] = evaluate(&sub_state, maximizing_player);
                sub_srcs[i][j]   = mov_options.array[i].src;
                sub_dests[i][j]  = mov_options.array[i].array[j];
            }
        }
    }

    double value = sub_values[0][0];
    *src = sub_srcs[0][0];
    *dest = sub_dests[0][0];
    for (int i = 0; i < mov_options.length; i++) {
        for (int j = 0; j < mov_options.array[i].length; j++) {
            bool replace = ( maximize && sub_values[i][j] > value)
                        || (!maximize && sub_values[i][j] < value);
            if (replace) {
                value = sub_values[i][j];
                *src = sub_srcs[i][j];
                *dest = sub_dests[i][j];
            }
        }
    }

    print_indentation(depth * 4);
    char src_str[3], dest_str[3];
    printf("%s -> %s (%g %s)\n",
            write_position(src, src_str), write_position(dest, dest_str),
            value, maximize ? "MAX-ing" : "MIN-ing");
    // game_print(state, depth * 4);
    // putchar('\n');

    return value;
}


double evaluate(Game_state* state, Color player)
{
    static double stone_value = 10;
    static double dame_value = 25;
    static double distance_bonus = 1;
    static double white_origin = 0;
    static double black_origin = 7;

    double value = 0;

    Position pos;
    for (pos.row = 0; pos.row < 8; pos.row++) {
        for (pos.col = 0; pos.col < 8; pos.col++) {
            Piece piece = get_piece(state, pos);

            if (!is_empty(piece))
            {
                double piece_value;
                if (is_stone(piece))  piece_value = stone_value;
                else                  piece_value = dame_value;

                int dist;  // distance from origin
                if (is_white(piece))  dist = pos.row - white_origin;
                else                  dist = black_origin - pos.row;

                piece_value += dist * distance_bonus;

                if (!piece_matches_player(piece, player))
                    piece_value *= -1;

                value += piece_value;
            }
        }
    }

    return value;
}

