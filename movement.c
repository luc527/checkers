#include <stdio.h>
#include "checkers.h"


/**
 * get_movtype: takes in a game state and a movement (source and destination),
 * and returns whether the movement is INVALID (cannot be performed),
 * CAPTURE or REGULAR (no captures made).
 */
Movtype get_movtype(Game_state *state, Position src, Position dest)
{   //{{{
    //
    // General movement validation
    //
    int valid_positions = is_valid_position(src) && is_valid_position(dest)
                       && is_diagonal(src, dest);
    if (!valid_positions)
        return INVALID;

    Piece atsrc  = get_piece(state, src);
    Piece atdest = get_piece(state, dest);
    if (is_empty(atsrc) || !is_empty(atdest))
        return INVALID;

    Color player = state->current_player;
    if (!piece_matches_player(atsrc, player))
        return INVALID;

    int distance = abs(dest.row - src.row);
    if (distance == 0)
        return INVALID;

    // hdir and vdir describe the direction of the movement
    // hdir (horizontal) tells if it goes right (1) or left (-1)
    // vdir (vertical) tells if it goes up (1) or down (-1)
    int hdir = (dest.col > src.col) ? 1 : -1;
    int vdir = (dest.row > src.row) ? 1 : -1;

    //
    // Stone-specific movement validation
    //
    if (is_stone(atsrc))
    {   // {{{
        if (distance == 1)
        {
            // White stones can only move up, blacks only down
            bool can_move = (atsrc == WHITE_STONE && vdir ==  1)
                         || (atsrc == BLACK_STONE && vdir == -1);
            return can_move ? REGULAR : INVALID;
        }
        else if (distance == 2)
        {
            // Possibly a capture
            Position mid = { src.row + vdir, src.col + hdir };
            int mid_piece = get_piece(state, mid);
            bool can_capture = (mid_piece != EMPTY)
                            && (!same_color(atsrc, mid_piece));
            return can_capture ? CAPTURE : INVALID;
        }
        else
        {
            // distance > 2
            return INVALID;
        }
    }  // }}}
    //
    // Dame-specific movement validation
    //
    else 
    {   // {{{
        if (distance == 1)
        {
            return REGULAR;
        }
        else
        {
            // Iterate over the pieces in the middle of src and dest to determine the movement type
            Movtype type = REGULAR;
            Position mid = { src.row + vdir, src.col + hdir };
            for (int i = 0; i < distance; i++)
            {
                int mid_piece = get_piece(state, mid);
                if (mid_piece != EMPTY)
                {
                    // Can't move over pieces of the same color (type is INVALID);
                    // capture when moving over pieces of different color (type is CAPTURE).
                    if (same_color(atsrc, mid_piece)) {
                        type = INVALID;
                        break;
                    } else {
                        type = CAPTURE;
                    }
                }
                mid.row += vdir;
                mid.col += hdir;
            }
            return type;
        }

    }  //}}}
}  //}}}


static void push_dest_option(Dest_options *opts, Position p)
{
    if (opts->length < MAXOPTIONS)  opts->array[opts->length++] = p;
}


/**
 * generate_dest_options: generates all possible movement destinations taking
 * the piece at the given position as the source, storing all of it at the
 * given Dest_options struct. 
 *
 * [Note about only_captures: If there are captures available, this function
 * will already generate only them as options.  So why only_captures, if the
 * function already only generates captures if required?
 * only_captures being true causes no options to be stored when there are
 * only regular movements available. But why would we want that?  Because when a
 * capture can be performed, it must be performed.  So when we are generating
 * all movement options and we know there's a capture to be made but there are still
 * pieces left to be processed, we don't want to consider their possible
 * regular movements as options.  So we call generate_dest_options for them with
 * only_captures set to true.]
 */
void generate_dest_options(Game_state *state, Position src,
                           Dest_options *options, bool only_captures)
{   //{{{
    options->length = 0;
    options->src    = src;
    options->type   = REGULAR;

    Piece atsrc  = get_piece(state, src);
    int directions[] = { -1, 1 };

    // We iterate through all possible directions a piece can go doing
    // for (i=0..2) vdir = directions[i]
    //   for (j=0..2) hdir = directions[j]
    //     ...

    if (is_empty(atsrc))
    {
        return;
    }
    //
    // Stone movement generation
    //
    else if (is_stone(atsrc))
    {   // {{{
        // First check for captures (in all squares with distance 2),
        // and set a bool telling whether a capture is an option
        for (int i=0; i<2; i++)
        {
            int vdir = directions[i];
            for (int j=0; j<2; j++)
            {
                int hdir = directions[j];
                Position dest = { src.row + 2*vdir, src.col + 2*hdir };
                Movtype type = get_movtype(state, src, dest);
                if (type == CAPTURE)
                {
                    options->type = CAPTURE;
                    push_dest_option(options, dest);
                }
            }
        }
        // Now check for regular moves only if no captures were available
        // (because captures are mandatory -- we can only push regular movements
        // as options under this condition)
        if (!only_captures && options->type == REGULAR)
        {
            int vdir = is_white(atsrc) ? 1 : -1;
            for (int i=0; i<2; i++)
            {
                int hdir = directions[i];
                Position dest = { src.row+vdir, src.col+hdir };
                Movtype type = get_movtype(state, src, dest);
                if (type == REGULAR)
                    push_dest_option(options, dest);
            }
        }
    }  //}}}
    //
    // Dame movement generation
    //
    else
    {   // {{{
        // First check for captures, and set a bool telling whether a capture is an option
        for (int i=0; i<2; i++)
        {
            int vdir = directions[i];
            for (int j=0; j<2; j++)
            {
                int hdir = directions[j];
                Position dest;
                for (dest.row = src.row+vdir, dest.col = src.col+hdir;
                     is_valid_position(dest);
                     dest.row += vdir, dest.col += hdir)
                {
                    Movtype type = get_movtype(state, src, dest);
                    if (type == CAPTURE)
                    {
                        options->type = CAPTURE;
                        push_dest_option(options, dest);
                    }
                }
            }
        }
        // Now check for regular moves only if no captures were available
        // (because captures are mandatory, so we can only push regular movements
        // as options under this condition)
        if (!only_captures && options->type == REGULAR)
        {
            for (int i=0; i<2; i++)
            {
                int vdir = directions[i];
                for (int j=0; j<2; j++)
                {
                    int hdir = directions[j];
                    Position dest;
                    for (dest.row = src.row+vdir, dest.col = src.col+hdir;
                         is_valid_position(dest);
                         dest.row += vdir, dest.col += hdir)
                    {
                        Movtype type = get_movtype(state, src, dest);
                        if (type == REGULAR)
                            push_dest_option(options, dest);
                    }

                }
            }
        } // end if
    } // }}} 

}   //}}}


/**
 * generate_mov_options: Generate all movement options for a player.
 * Rule: a capture must be made when it is possible.  Note that
 * generate_dest_options will only consider a single piece, so it may give a
 * regular move as an option even though some other piece in the board can
 * perform a capture. On the othar hand, generate_mov_options will ensure
 * that this rule is fulfilled.
 */
void generate_mov_options(Game_state *state, Mov_options *player_options)
{   //{{{
    player_options->length = 0;
    player_options->type = REGULAR;
    Color player = state->current_player;

    // Generate all movement options for the player.
    // If an available capture is found, then quit this loop and 
    // restart the process of generating options, this time just including captures.
    // (Why restart? Because until reaching that capture, some regular movements might have been counted as options).
    //
    // Another improvement is maybe first generate an array of the positions of the pieces the player has.
    // Then we don't need to iterate through the whole board like
    // for row=0..8
    //   for col=0..8
    //     ...
    // twice.

    bool can_capture = false;
    Dest_options *piece_options;
    Position pos;  // Holds current position when iterating

    // Outer for iterates through columns because then the options are ordered
    // column by column, which is more convenient for when the player cycles
    // through them.
    for (pos.col = 0; pos.col < BOARD_SIZE; pos.col++)
    {
        for (pos.row = 0; pos.row < BOARD_SIZE; pos.row++)
        {
            Piece piece = get_piece(state, pos);
            // v Found a piece that the player /may/ be able to move
            if (piece_matches_player(piece, player))
            {  
                // v at each iteration piece_options points to the current Dest_options inside the Mov_options array
                // TODO maybe encapsulate this stuff (into get_current_somethingidk) and also the pushing stuff 10 lines below
                piece_options = &player_options->array[player_options->length];

                generate_dest_options(state, pos, piece_options, false);
                // v This piece /can/, in fact, be moved (there are movement options for it)
                if (piece_options->length > 0)
                {  
                    // Push the movement options for this piece onto the Mov_options array
                    player_options->length++;
                    if (piece_options->type == CAPTURE)
                    {
                        can_capture = true;
                        player_options->type = CAPTURE;
                        goto end_capture_search;
                    }
                }
            }  // end if

        }  // end for
    }
end_capture_search:

    // If no capture was available, we don't need to do anything else,
    // the code above will generate regular movements as options just fine.
    // Otherwise, we need to re-generate the options to only include captures.
    if (can_capture)
    {
        player_options->length = 0;
        for (pos.col = 0; pos.col < BOARD_SIZE; pos.col++)
        {
            for (pos.row = 0; pos.row < BOARD_SIZE; pos.row++)
            {
                piece_options = &player_options->array[player_options->length];
                Piece piece = get_piece(state, pos);
                if (piece_matches_player(piece, player))
                {
                    generate_dest_options(state, pos, piece_options, true);
                    if (piece_options->type == CAPTURE)
                        player_options->length++;
                }
            }

        }  // end for
    }
    // TODO consider making this function take an 'only_captures' argument for RECURRING:
    // instead of doing goto end_capture_search and stuff,
    // do if (piece_options->capture) { generate_mov_options(state, player_options, true); return; }
    // basically recur to do a different thing (generate options but only including captures)
    // that has the same structure (see the similarity between the code above and below end_capture_search)

}   //}}}

