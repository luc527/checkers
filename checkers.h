#ifndef CHECKERS_H
#define CHECKERS_H

#include <stdbool.h>

// Languages {{{
typedef enum { PT, EN } Language;
#define NLANGS 2

typedef enum {
    SOURCE_PROMPT,
    DESTINATION_PROMPT,
    NOT_AN_OPTION,
    MUST_CAPTURE,
    MUST_PERFORM_SEQUENTIAL_CAPTURE,
    WHITE_WINS_MSG, 
    // ^-- needs _MSG because the versions without it are alredy defined (situations)
    BLACK_WINS_MSG,
    TIE_MSG,
    CURRENT_PLAYER,
    WHITE_PLAYER,
    BLACK_PLAYER,
	MUST_SELECT_MOVEMENT,
	ALREADY_SELECTED_MOVEMENT,
    INSTRUCTIONS
} Message;
#define NMESSAGES 14

char *getmsg(Message, Language);
// }}}

// Simple typedefs {{{
typedef enum { WHITE, BLACK } Color;

typedef enum { WHITE_STONE, BLACK_STONE,
               WHITE_DAME , BLACK_DAME ,
               EMPTY } Piece;

typedef enum { ONGOING, WHITE_WINS, BLACK_WINS, TIE } Situation;

typedef enum { INVALID, REGULAR, CAPTURE } Movtype;

typedef struct {
    int row;
    int col;
} Position;
// }}}

// util.c {{{
bool read_position  (Position *);
void print_position (char *, Position);

bool is_valid_position    (Position);
bool is_diagonal          (Position, Position);
bool is_white             (Piece);
bool is_black             (Piece);
bool is_stone             (Piece);
bool is_dame              (Piece);
bool is_empty             (Piece);
bool same_color           (Piece, Piece);
bool piece_matches_player (Piece, Color);

int abs(int);
int clamp(int, int min, int max);
// }}}

// game_state.c {{{
typedef struct {
    Piece board[8][8];
    Color current_player;
    Situation situation;
} Game_state;

Piece get_piece (Game_state *, Position);
void  set_piece (Game_state *, Position, Piece);

void game_setup              (Game_state *);
void switch_player           (Game_state *);
void upgrade_stones_to_dames (Game_state *);
void perform_movement        (Game_state *, Position src, Position dest);
void game_print              (Game_state *);
void update_situation        (Game_state *);
/// }}}

// movement.c {{{

// Returns the type of the given movement -- INVALID, REGULAR or CAPTURE.
Movtype get_movtype (Game_state *, Position from, Position to);

#define MAXOPTIONS 13
/* A /stone/ will have at most four options of movement (captures in all four
 * directions), or two if it can't capture, but a /dame/ can potentially move to
 * every square in its four diagonals, which are 13 in total (count the X's below).
 * So MAXOPTIONS in the upper bound to how many options any piece can perform.
 * | 12345678|
 * |1       X|
 * |2X     X |
 * |3 X   X  |
 * |4  X X   |
 * |5   @    |
 * |6  X X   |
 * |7 X   X  |
 * |8X     X |
 */

/* Dest_options groups the data that informs what are the
 * destination options for a piece at position 'src'.
 * It can have an arbitrary (within [0..MAXOPTIONS]) number of
 * options, so we implement a variable-length array (with a limit of
 * MAXOPTIONS elements) with a regular array and a 'length' component.
 * The Movtype component type tells whether the options are all regular
 * movements (so type = REGULAR) or all captures (type = CAPTURE).
 * It cannot have regular and capture movements at the same time,
 * since it's required that if captures are available they must be made.
 */
typedef struct {
    Position src;
    Position array[MAXOPTIONS];
    int length;
    Movtype type;
} Dest_options;

void generate_dest_options(Game_state *, Position, Dest_options *, bool only_captures);

#define NUMPIECES 12

// Mov_options groups the data that informs all movement options
// a player has. That is, it lists all the Dest_options for each piece.
// Again a variable-length array with an upper-bound (NUMPIECES) is
// implemented with a regular array and a 'length' component; and again
// the Movtype component type tells whether the movements are all captures
// (since when there are captures no regular moves can be made) or all
// regular moves.
typedef struct {
    Dest_options array[NUMPIECES];
    int length;
    Movtype type;
} Mov_options;

void generate_mov_options(Game_state *, Mov_options *);
// }}}

// {{{ interface.c
void msgwin_print(char *);
void msgwin_append(char *);

void init_interface();
void refresh_interface();
void close_interface();

void get_movement_interactively(
        Game_state *,
        Mov_options *,
        Position *src,
        Position *dest
);
// }}}

#endif

