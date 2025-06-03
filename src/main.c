
#include "g_game.h"

int main(int argc, char** argv) {
    printf("Hello world");

    game game;

    game_init(&game);
    game_run(&game);
    game_stop(&game);

    return 0;
}