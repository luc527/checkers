#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include "checkers.h"

#include <unistd.h>
// ^ for sleep() when testing how the game ends, TODO remove when that's all sorted out

// Language used in printing the messages
Language language = EN;


/* get_movement sets up the interactive board for the player to perform a
 * movement (with get_movement_interactively), stores the given movement in
 * 'src' and 'dest', and returns the movement type (CAPTURE or REGULAR -- never
 * INVALID because get_movement_interactively only allows the player to select
 * movements from the given options).
 */
Movtype get_movement(Game_state *state, Position *src, Position *dest)
{
    Mov_options options;
    generate_mov_options(state, &options);

    // Tell whether a capture must be performed
    if (options.type == CAPTURE)
        msgwin_print(getmsg(MUST_CAPTURE, language));

    // TODO this movement validation might be unnecessary because now
    // get_movement_interactively locks the player into performing only
    // movements from the options -- it's impossible (except in the case of a
    // bug...) for an invalid movement to be selected and retrieved from that
    // function.
    //
    // However, it's probably nice to keep it and use it to verify
    // that the program is working correctly -- then instead of warning the user
    // that the options is invalid it should show an error message.
    //
    // So if the decision is taken to keep the movement validation, leave a
    // comment explaining the situaton (it used to be that in the board
    // interaction the player could select any movement and we had to validate
    // it, but since we lock the player into selecting only the options this is
    // technically unnecessary, but we kept it since it could detect and treat
    // errors in the board interaction)
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


/* get_sequential_capture: if the player can perform a capture from 'src', the
 * game will set up the interactive board for the player to peform the capture
 * (or to choose between different possible captures if there's more than one),
 * and then store it in 'dest'.
 * If no captures are available, nothing is done. 
 * It returns CAPTURE if there were captures available, REGULAR otherwise.
 *
 * The return value is used to decide whether or not to call
 * get_sequential_capture again: if CAPTURE was returned it means the player
 * can perform even more sequential captures, so call it again; otherwise stop
 * calling since no more sequential captures are available.
 */
Movtype get_sequential_capture(Game_state *state, Position src, Position *dest)
{
    Dest_options destopts;
    generate_dest_options(state, src, &destopts, true);

    if (destopts.type == CAPTURE) {
        // get_movement_interactively takes a Mov_options, not a Dest_options,
        // so we must create a Mov_options with only the capture(s) as option(s).  
        Mov_options opts;
        opts.type = CAPTURE;
        opts.array[0] = destopts;
        opts.length = 1;

        msgwin_print(getmsg(MUST_PERFORM_SEQUENTIAL_CAPTURE, language));
        get_movement_interactively(state, &opts, &src, dest);
    }

    return destopts.type;
}


/* game_loop will play the game with the given Game_state until the end. */
void game_loop(Game_state *state)
{
    while (state->situation == ONGOING)
    {
        Position movsrc, movdest;
        Movtype type = get_movement(state, &movsrc, &movdest);
        if (type == REGULAR) {
            perform_movement(state, movsrc, movdest);
        } else {
            // keep performing captures with the same piece if available
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

    /* FIXME segmentation fault somewhere here. maybe already fixed by adding the _MSG though
    if (state->situation == WHITE_WINS)
        msgwin_print(getmsg(WHITE_WINS_MSG, language));
    else if (state->situation == BLACK_WINS)
        msgwin_print(getmsg(BLACK_WINS_MSG, language));
    else  // situation == TIE
        msgwin_print(getmsg(TIE_MSG, language));
    */
    
    endwin();
    printf("Test");

    sleep(10);
}



int main(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        if (strcmp("--pt", argv[i]) == 0)  language = PT;
    }

    initscr();

    if (LINES < 24 || COLS < 80) {
        printf("Your terminal window is too small (%dx%d)!"
               " At least 80x24.\n", COLS, LINES);
        endwin();
        return 1;
    }

    setup_interface();

    Game_state state;
    game_setup(&state);
    game_loop(&state);

    close_interface();

    return 0;
}

