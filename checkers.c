#include <stdio.h>
#include <string.h>
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

    // Tell whether a capture must be performed, and using which pieces.
    if (options.type == CAPTURE)
    {
        msgwin_print(getmsg(MUST_CAPTURE_WITH, language));
        for (int i = 0; i < options.length; i++)
        {
            char srcstr[4];  // e.g. {'A', '3', ' ', '\0'}
            print_position(srcstr, options.array[i].src);
            msgwin_append(srcstr);
        }
    }

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


void game_loop(Game_state *state)
{
    do {
        Position movsrc, movdest;

        Movtype type = get_movement(state, &movsrc, &movdest);
        perform_movement(state, movsrc, movdest);

        upgrade_stones_to_dames(state);
        switch_player(state);
        update_situation(state);

    } while (state->situation == ONGOING);

    if (state->situation == WHITE_WINS)
        msgwin_print(getmsg(WHITE_WINS, language));
    else if (state->situation == BLACK_WINS)
        msgwin_print(getmsg(BLACK_WINS, language));
    else  // situation == TIE
        msgwin_print(getmsg(TIE, language));
}



int main(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        if (strcmp("--pt", argv[i]) == 0) {
            language = PT;
        }
    }

    init_messages_array();
    init_interface();

    Game_state state;
    game_setup(&state);
    game_loop(&state);

    close_interface();

    return 0;
}

