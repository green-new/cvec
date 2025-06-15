
#include "g_game.h"

#include <stdio.h>

int main(int argc, char** argv) {
    printf("Hello world");

    Game game;

    G_Init(&game);
    G_Start(&game);
    G_Stop(&game);

    return 0;
}