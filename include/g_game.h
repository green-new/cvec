#ifndef GAME_H_
#define GAME_H_

#include "g_window.h"
#include "g_clock.h"
#include <stdbool.h>

/**
 * Structure containing high-level game information
 */
typedef struct game {

    window window;

    clock clock;

    bool running;

} game;

/**
 * Structure containing game configuration
 */
typedef struct game_config {

} game_config;

/**
 * Initializes the game.
 * @param game The game object.
 */
void
game_init(game* game);

/**
 * Starts the game.
 * @param game The game object.
 */
void
game_start(game* game);

/**
 * Stops the game.
 * @param game The game object.
 */
void
game_stop(game* game);

#endif