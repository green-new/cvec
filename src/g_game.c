#include "g_game.h"

void
game_init(game* game) {
    game->window = window_create(
        "C game",
        1920,
        1080,
        SDL_WINDOW_FULLSCREEN
    );
    game->clock = (clock) { 0 };

    /* Initialize vulkan */
    

    /* Running is now true*/
    game->running = true;
}

void
game_start(game* game) {
    while (game->running) {
        /* Update the game clock */
        clock_update(&game->clock);
    }
}

void game_stop(game* game) {
    window_destroy(&game->window);
    game->running = false;

}