#include "checkers.h"

int main()
{
    Game_state state;
    game_setup(&state);

    game_tree(&state, 0, 2);

    return 0;
}
