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

/* get_movement(state, *src, *dest)
 * stores a valid movement (source and destination positions) from the player in src and dest,
 * returns the type of the movement (REGULAR or CAPTURE -- we ensure it's never INVALID).
 * It also ensures that, if captures are available, the player must perform one of them.
 */

Movtype get_movement(Game_state *state, Position *src, Position *dest)
{

    // The paradigm is: generate all the options and, to validate, verify if the given movement is one of the options.
    //
    // First generate all the options the player has with generate_movoptions_player. 
    // This involves considering all the pieces the playe can move from (the sources),
    // and for each source, all of the positions the piece can be moved to (the destinations).
    // Then ask the player the position of the piece he wants to move (his source).
    // If this position is not one of the pre-computed options, tell him so and ask him again.
    // After a valid source is chosen, ask the player for the position to move the piece to (the destination).
    // If this position is not one of the destination options, tell him so and ask him again.

    Movoptions_player options;
    generate_movoptions_player(state, &options);

    // Tell the player when he has some capture available to do and must perform it
    if (options.captures)
    {
        print(MUST_CAPTURE_WITH);
        for (int i = 0; i < options.length; i++)
            print_position(options.array[i].src);
        putchar('\n');
    }

    // Ask player for movement until he provides a valid one
    bool valid_move = false;
    do {
        //
        // Get movement source (position of the piece to be moved) and validate it
        //
        print(SOURCE_PROMPT);
        read_position(src);
        int src_index = -1;  // -1 indicates the given position was not found in the pre-computed options
        for (int i=0; i<options.length; i++)
        {
            Position option_src = options.array[i].src;
            if (src->row == option_src.row && src->col == option_src.col)
            {
                src_index = i;
                break;
            }
        }

        if (src_index == -1)
        {
            print(NOT_AN_OPTION);
            // don't change valid_move to repeat the loop and ask the player again
        }
        else
        {
            //
            // Get movement destination
            //
            Movoptions_piece *piece_options = &options.array[src_index];
            print(DESTINATION_PROMPT);
            read_position(dest);

            bool dest_is_an_option = false;
            for (int i=0; i<piece_options->length && !dest_is_an_option; i++)
            {
                Position opt = piece_options->array[i];
                if (opt.row == dest->row && opt.col == dest->col)
                    dest_is_an_option = true;
            }

            if (!dest_is_an_option) {
                print(NOT_AN_OPTION);
            } else {
                putchar('\n');
                valid_move = true;
            }
        }

    } while (!valid_move);

    return options.captures ? CAPTURE : REGULAR;
}


// TODO a lot of this code is duplicated from get_movement.
// See if you /can/ extract it to a function and call that function
// in both functions.

void get_sequential_capture(Game_state *state, Position *src, Position *dest,
                            Movoptions_piece *options)
{
    print(MUST_PERFORM_SEQUENTIAL_CAPTURE);
    print_position(*src);
    putchar('\n');

    bool valid_move = false;
    do {
        //
        // Get movement destination
        //
        print(DESTINATION_PROMPT);
        read_position(dest);

        bool dest_is_an_option = false;
        for (int i=0; i<options->length && !dest_is_an_option; i++)
        {
            Position opt = options->array[i];
            if (dest->row == opt.row && dest->col == opt.col)
                dest_is_an_option = true;
        }

        if (!dest_is_an_option) {
            print(NOT_AN_OPTION);
        } else {
            putchar('\n');
            valid_move = true;
        }

    } while (!valid_move);
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
        // TODO should this code be in a function call to make game_loop cleaner?
        while (type == CAPTURE)
        {
            Movoptions_piece options;
            generate_movoptions_piece(state, movdest, &options, true);

            if (options.captures)  // Can perform a sequential capture
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

