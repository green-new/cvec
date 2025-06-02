#ifndef GAME_H_
#define GAME_H_

#include "g_window.h"

typedef struct game {

    window window;

} game;

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