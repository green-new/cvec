#ifndef WINDOW_H__
#define WINDOW_H__

#include "SDL3/SDL.h"

typedef struct Window {

    SDL_Window* handle;

    const char* title;
    int width;
    int height;
    uint32_t flags;

} Window;


/**
 * @brief Create an SDL window.
 * @param title The title of the window.
 * @param width The width of the window.
 * @param height The height of the window.
 * @param SDL_flags The SDL flags to add to the window.
 * @returns Window
 */
Window
window_create(
    const char* title,
    int width,
    int height,
    uint32_t flags
) {
    return (Window) {
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
window_destroy(Window* window) {
    SDL_DestroyWindow(window->handle);
}


#endif