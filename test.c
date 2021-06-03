#include "checkers.h"

int main()
{
    Game_state state;
    game_setup(&state);

    // printf("%g\n", evaluate(&state, WHITE));
    // printf("%g\n", evaluate(&state, BLACK));

    Position src, dest;
    minimax(&state, 2, WHITE, &src, &dest);

    printf("(%d,%d) -> (%d,%d)\n", src.col, src.row, dest.col, dest.row);

    return 0;
}
