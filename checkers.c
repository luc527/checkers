#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include "checkers.h"

// Language used in printing the messages
Language language = EN;


/* get_movement(state, *src, *dest) stores a valid movement (source and
 * destination positions) from the player in src and dest, returns the type of
 * the movement (REGULAR or CAPTURE -- we ensure it's never INVALID).  It also
 * ensures that, if captures are available, the player must perform one of them.
 */
Movtype get_movement(Game_state *state, Position *src, Position *dest)
{
    Mov_options options;
    generate_mov_options(state, &options);

    // Tell whether a capture must be performed
    if (options.type == CAPTURE)
        msgwin_print(getmsg(MUST_CAPTURE, language));

    bool valid_move = false;
    while (!valid_move)
    {
        get_movement_interactively(state, &options, src, dest);

        // check whether given source is an option;
        // srcindex the index of the given source position in the
        // player movement options array. -1 used to indicate not found
        int srcindex = -1;
        for (int i = 0; i < options.length; i++)
        {
            Position pos = options.array[i].src;
            if (src->row == pos.row && src->col == pos.col)
                srcindex = i;
        }

        if (srcindex == -1)
        {
            msgwin_print(getmsg(NOT_AN_OPTION, language));
        }
        else
        {
            Dest_options *destopts = &options.array[srcindex];

            // check wheter given destination is an option;
            // set valid_move to true if so,
            // keep valid_move false and warn the player otherwise.
            for (int i = 0; i < destopts->length; i++)
            {
                Position pos = destopts->array[i];
                if (dest->row == pos.row && dest->col == pos.col)
                    valid_move = true;
            }

            if (!valid_move)  msgwin_print(getmsg(NOT_AN_OPTION, language));
        }
    }

    return options.type;
}


Movtype get_sequential_capture(Game_state *state, Position src, Position *dest)
{
    Dest_options destopts;
    generate_dest_options(state, src, &destopts, true);

    if (destopts.type == CAPTURE) {
        // We can't just pass Dest_options to get_movement interactively.
        // To force the player to perform the sequential capture we must
        // make up a Mov_options with only one source option, then let
        // the player choose between the possible captures that can be
        // performed from there.
        Mov_options opts;
        opts.type = CAPTURE;
        opts.array[0] = destopts;
        opts.length = 1;

        msgwin_print(getmsg(MUST_PERFORM_SEQUENTIAL_CAPTURE, language));
        get_movement_interactively(state, &opts, &src, dest);
    }

    return destopts.type;
}


void game_loop(Game_state *state)
{
    while (state->situation == ONGOING)
    {
        Position movsrc, movdest;

        Movtype type = get_movement(state, &movsrc, &movdest);

        if (type == REGULAR) {
            perform_movement(state, movsrc, movdest);
        } else {
            // keep performing captures if available
            while (type == CAPTURE) {
                perform_movement(state, movsrc, movdest);
                movsrc = movdest;
                type = get_sequential_capture(state, movsrc, &movdest);
            }
        }

        upgrade_stones_to_dames(state);
        switch_player(state);
        update_situation(state);

    }

    if (state->situation == WHITE_WINS)
        msgwin_print(getmsg(WHITE_WINS, language));
    else if (state->situation == BLACK_WINS)
        msgwin_print(getmsg(BLACK_WINS, language));
    else  // situation == TIE
        msgwin_print(getmsg(TIE, language));

    // TODO final game situation is not very visible when the game ends,
    // this below behaves kinda weird
    refresh_interface();
    getch();
}



int main(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        if (strcmp("--pt", argv[i]) == 0)  language = PT;
    }

    init_interface();

    Game_state state;
    game_setup(&state);
    game_loop(&state);

    close_interface();

    return 0;
}

