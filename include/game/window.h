#ifndef WINDOW_H__
#define WINDOW_H__

#include "sdl3/SDL.h"

typedef enum WindowResult {
    SUCCESS = 0,
    FAILURE = 1
} WindowResult;

typedef struct Window {


    SDL_Window* handle;

    const char* title;
    int width;
    int height;
    uint32_t flags;


} Window;

/**
 * @brief Create a SDL window.
 * @param title The title of the window.
 * @param width The width of the window.
 * @param height The height of the window.
 * @param SDL_flags The SDL flags to add to the window.
 * @returns WindowResult
 */
WindowResult
window_create(
    const char* title,
    int width,
    int height,
    uint32_t SDL_flags
);

/**
 * Destroy the given SDL window.
 * @param window The window.
 * @returns WindowResult
 */
WindowResult
window_destroy(Window* window);


#endif