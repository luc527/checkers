#include "checkers.h"

int main()
{
    Game_state state;
    game_setup(&state);

    // printf("%g\n", evaluate(&state, WHITE));
    // printf("%g\n", evaluate(&state, BLACK));

    Position src, dest;
    minimax(&state, 4, WHITE, &src, &dest);

    return 0;
}
