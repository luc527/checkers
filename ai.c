#include "checkers.h"

void printpos(Position *p) {
    printf("%c%c ", p->col+'A', p->row+'1');
}

void game_tree(Game_state* state0, int depth, int maxdepth) {

    game_print(state0);
    printf("Depth: %d\n", depth);
    putchar('\n');

    if (depth >= maxdepth) {
        return;
    }

    Mov_options mops;
    generate_mov_options(state0, &mops);

    for (int i = 0; i < mops.length; i++) {
        Dest_options* dops = &mops.array[i];
        for (int j = 0; j < dops->length; j++) {


            printf("From ");
            printpos(&dops->src);
            printf(" to ");
            printpos(&dops->array[j]);
            printf(".\n");

            Game_state* state = allocate_copy(state0);
            game_update(state, dops->src, dops->array[j]);
            game_tree(state, depth+1, maxdepth);
            free_copy(state);
        }
    }
}
