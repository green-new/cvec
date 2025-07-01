
#define VEC_IMPL_H_
#include "g_game.h"

#include <stdio.h>

int main(int argc, char** argv) {
    printf("Hello world\n");

    Game game = { 0 };

    G_Init(&game);
    G_Start(&game);
    G_Stop(&game);

    return 0;
}