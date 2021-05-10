#include <stdio.h>
#include "checkers.h"


/* get_movtype: takes in a game state and a movement (source and destination),
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
    if (!valid_positions)  return INVALID;

    Piece atsrc  = get_piece(state, src);
    Piece atdest = get_piece(state, dest);
    if (is_empty(atsrc) || !is_empty(atdest))  return INVALID;

    Color player = state->current_player;
    if (!piece_matches_player(atsrc, player))  return INVALID;

    int distance = abs(dest.row - src.row);
    if (distance == 0)  return INVALID;

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
            int atmid = get_piece(state, mid);
            bool can_capture = (atmid != EMPTY)
                            && (!same_color(atsrc, atmid));
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
                int atmid = get_piece(state, mid);
                if (atmid != EMPTY)
                {
                    // Can't move over pieces of the same color;
                    // moving over pieces of different colors means it's a capture.
                    if (same_color(atsrc, atmid)) {
                        type = INVALID;
                        break;
                        // We break right away because otherwise a piece that comes later
                        // that has the opposite color would set 'type' to CAPTURE
                    } else {
                        type = CAPTURE;
                        // But we don't break here because there might be a piece of
                        // opposite color in the way later
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


/* generate_dest_options: generates all possible movement destinations taking
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
    // TODO int diagonals[] = {{-1,1},{-1,-1},etc.} even though /not all/ of the code below can use it?

    // We iterate through all possible diagonals a piece can go doing
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
        // and set a bool telling whether one was found
        for (int i = 0; i < 2; i++)
        {
            int vdir = directions[i];
            for (int j = 0; j < 2; j++)
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
        // (captures are mandatory) or if only_captures doesn't restrict it
        if (!only_captures && options->type == REGULAR)
        {
            int vdir = is_white(atsrc) ? 1 : -1;
            for (int i = 0; i < 2; i++)
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
        for (int i = 0; i < 2; i++)
        {
            int vdir = directions[i];
            for (int j = 0; j < 2; j++)
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
        // (captures are mandatory) or only_captures doesn't restrict it
        if (!only_captures && options->type == REGULAR)
        {
            for (int i = 0; i < 2; i++)
            {
                int vdir = directions[i];
                for (int j = 0; j < 2; j++)
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
void generate_mov_options(Game_state *state, Mov_options *mov_options)
{   //{{{
    mov_options->length = 0;
    mov_options->type = REGULAR;
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
    Dest_options *dest_options;
    Position pos;  // Holds current position when iterating

    // Outer for iterates through columns because then the options are ordered
    // column by column, which is more convenient for when the player cycles
    // through them.
    for (pos.col = 0; pos.col < BOARD_SIZE; pos.col++)
    {
        for (pos.row = 0; pos.row < BOARD_SIZE; pos.row++)
        {
            Piece piece = get_piece(state, pos);
            // Player /might/ be able to move this piece
            if (piece_matches_player(piece, player))
            {  
                // dest_options points to next available space in mov_options
                // (the Mov_options structure already has all the space we need
                // for storing Dest_options structures, so we just point to
                // each array location in it)
                dest_options = &mov_options->array[mov_options->length];
                generate_dest_options(state, pos, dest_options, false);
                // This piece /can/ be moved
                if (dest_options->length > 0)
                {  
                    // Effectuate dest_options insertion by increasing the 'length' component
                    // (When dest_options->length == 0, which means the piece can't really be moved,
                    // not increasting the 'length' component will cause the next iteration to overwrite
                    // the mov_options array location where the current dest_options was generated.)
                    mov_options->length++;
                    // But if it's a capture, then we want /only/ the captures...
                    if (dest_options->type == CAPTURE)
                    {
                        can_capture = true;
                        mov_options->type = CAPTURE;
                        goto end_regular_search;
                    }
                }
            }  // end if

        }  // end for
    }
end_regular_search:

    // At this point, if no capture was available, we don't need to do anything
    // else, the code above will generate regular movements as options just
    // fine.  But if there /were/ captures available, we need to re-generate
    // the options to only include captures. [The code below follows
    // the same logic as above].
    if (can_capture)
    {
        mov_options->length = 0;
        for (pos.col = 0; pos.col < BOARD_SIZE; pos.col++)
        {
            for (pos.row = 0; pos.row < BOARD_SIZE; pos.row++)
            {
                dest_options = &mov_options->array[mov_options->length];
                Piece piece = get_piece(state, pos);
                if (piece_matches_player(piece, player))
                {
                    generate_dest_options(state, pos, dest_options, true);
                    if (dest_options->type == CAPTURE)
                        mov_options->length++;
                }
            }

        }  // end for
    }
    // TODO consider making this function take an 'only_captures' argument for RECURRING:
    // instead of doing goto end_capture_search and stuff,
    // do if (dest_options->capture) { generate_mov_options(state, mov_options, true); return; }
    // basically recur to do a different thing (generate options but only including captures)
    // that has the same structure (see the similarity between the code above and below end_capture_search)

}   //}}}

