#include <stdio.h>
#include <string.h>
#include "checkers.h"

// Language used in printing the messages
Language language = EN;

// Shortcut for printing the messages in the preferred language
void print(Message m)
{
    printmsg(m, language);
}


/* prompt_source: given the player's movement options,
 * will prompt the user for a movement source until he provides a valid one,
 * and will return the destination options for that given position.
 */
Movoptions_piece *prompt_source(Movoptions_player *options)
{
    // Tell player when captures are available and must be performed
    if (options->type == CAPTURE) {
        print(MUST_CAPTURE_WITH);
        for (int i = 0; i < options->length; i++) {
            print_position(options->array[i].src);
        }
        putchar('\n');
    }

    Position src;
    int src_index = -1;

    // Prompt player for source position until he provides a valid one
    bool valid_source = false;
    do {
        print(SOURCE_PROMPT);
        read_position(&src);
        for (int i = 0; i < options->length; i++) {
            Position option_src = options->array[i].src;
            if (src.row == option_src.row && src.col == option_src.col) {
                src_index = i;
                break;
            }
        }

        if (src_index == -1) {
            print(NOT_AN_OPTION);
            // Keep looping
        } else {
            valid_source = true;
        }
    } while (!valid_source);

    return &options->array[src_index];
}


/* prompt_destionation: given the player's destination options,
 * will prompt the player for a movement destination until he provides a valid
 * one, and will load it in the given destination pointer by dereferencing.
 */
void prompt_destination(Movoptions_piece *options, Position *dest)
{
    bool valid_dest = false;
    do {
        print(DESTINATION_PROMPT);
        read_position(dest);

        bool dest_is_option = false;
        for (int i = 0; i < options->length && !dest_is_option; i++) {
            Position option = options->array[i];
            dest_is_option = (dest->row == option.row
                           && dest->col == option.col);
        }

        if (!dest_is_option) {
            print(NOT_AN_OPTION);
        } else {
            putchar('\n');
            valid_dest = true;
        }
    } while (!valid_dest);
}

/* get_movement(state, *src, *dest)
 * stores a valid movement (source and destination positions) from the player in src and dest,
 * returns the type of the movement (REGULAR or CAPTURE -- we ensure it's never INVALID).
 * It also ensures that, if captures are available, the player must perform one of them.
 */
Movtype get_movement(Game_state *state, Position *src, Position *dest)
{

    // TODO cleaning up get_movement like this actually changed the 
    // control flow.  Before it, if the player gave an invalid destination
    // the game would prompt again from the start -- would ask for
    // another source again, then another destination, and so on.
    // Now when the player gives an invalid destination
    // the game just asks for another destination.  That is,
    // the player is stuck with the source he gave before.
    //
    // This doesn't call for going back to the previous get_movement
    // implementation, but it calls for a better way for the user to
    // interact with the game.  In fact, the previous implementation
    // was alredy bad -- it required the player to input an invalid position
    // if he wanted to change the source.  Furthermore, there are other
    // problems to be solved related to the same issue: there's no way for the
    // player to restart the game, or to declare a tie, etc. (and now to
    // change what movement he wants to do).
    //
    // So the next step is to completely reestructure the user interaction
    // (pretty much everything in the checkers.c file), which includes using
    // ncurses (so unfortunately it won't run on windows anymore) so the player has a
    // cursor to move around the board to choose pieces and perform movements
    // and does not need to type in positions.

    Movoptions_player options;
    generate_movoptions_player(state, &options);

    Movoptions_piece *dest_options = prompt_source(&options);

    src->row = dest_options->src.row;
    src->col = dest_options->src.col;

    prompt_destination(dest_options, dest);

    return options.type;
}


void get_sequential_capture(Game_state *state, Position *src, Position *dest,
                            Movoptions_piece *options)
{
    print(MUST_PERFORM_SEQUENTIAL_CAPTURE);
    print_position(*src);
    putchar('\n');

    prompt_destination(options, dest);
}


void game_loop(Game_state *state)
{
    do {
        game_print(state);

        Position movsrc, movdest;

        // First movement
        Movtype type = get_movement(state, &movsrc, &movdest);
        perform_movement(state, movsrc, movdest);

        // Whenever the player captures some piece, they may continue capturing sequentially
        while (type == CAPTURE)
        {
            Movoptions_piece options;
            generate_movoptions_piece(state, movdest, &options, true);

            if (options.type == CAPTURE)  // Can perform a sequential capture
            {
                // Destination from previous move is source for sequential move
                movsrc = movdest;
                // Ask the player for the sequential capture and perform it
                game_print(state);
                get_sequential_capture(state, &movsrc, &movdest, &options);
                perform_movement(state, movsrc, movdest);
                type = CAPTURE;
            }
            else
            {
                // No more captures available, only regular moves (stop looping)
                type = REGULAR;
            }
        }

        upgrade_stones_to_dames(state);
        switch_player(state);
        update_situation(state);

    } while (state->situation == ONGOING);

    if (state->situation == WHITE_WINS)
        print(WHITE_WINS);
    else if (state->situation == BLACK_WINS)
        print(BLACK_WINS);
    else  // situation == TIE -- FIXME this is unreachable, either delete it (also in the checkers.h enums) or make ties possible in the game
        print(TIE);
}



int main(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        if (strcmp("--pt", argv[i]) == 0) {
            language = PT;
        }
    }

    init_messages_array();

    Game_state state;
    game_setup(&state);
    game_loop(&state);
    return 0;
}

