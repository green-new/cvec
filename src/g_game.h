/**
 * Author: tristen
 * Defines the main game things
 */

#ifndef GAME_H_
#define GAME_H_

#define SDL_VULKAN_CORE_H_
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include "g_window.h"
#include "g_clock.h"

#define VEC_IMPL_H_
#include "r_render.h"

/**
 * Structure containing high-level game information
 */
typedef struct game_t {

    Window window;

    Clock clock;

    int running;

    R_RenderState render_state;

    VkDebugUtilsMessengerEXT debug_messenger;
    
} game_t;

/**
 * Initializes the game.
 * @param game The game object.
 */
int
G_Init(game_t* game);

/**
 * Starts the game.
 * @param game The game object.
 */
void
G_Start(game_t* game);

/**
 * Stops the game.
 * @param game The game object.
 */
void
G_Stop(game_t* game);

#endif // GAME_H_