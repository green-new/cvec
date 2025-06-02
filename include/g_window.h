#ifndef WINDOW_H_
#define WINDOW_H_

#include "SDL3/SDL.h"

typedef struct window {

    SDL_Window* handle;

    const char* title;
    int width;
    int height;
    uint32_t flags;

} window;


/**
 * @brief Create an SDL window.
 * @param title The title of the window.
 * @param width The width of the window.
 * @param height The height of the window.
 * @param SDL_flags The SDL flags to add to the window.
 * @returns WindowCreateResult
 */
window
window_create(
    const char* title,
    int width,
    int height,
    uint32_t flags
) {
    return (window) {
        .handle = SDL_CreateWindow(title, width, height, flags),
        .title = title,
        .width = width,
        .height = height,
        .flags = flags
    };
}

/**
 * Destroy the given SDL window.
 * @param window The window.
 * @returns void
 */
void
window_destroy(window* window) {
    SDL_DestroyWindow(window->handle);
}


#endif