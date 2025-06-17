#ifndef WINDOW_H_
#define WINDOW_H_

#include "SDL3/SDL.h"

typedef struct {

    /** SDL pointer to the window */
    SDL_Window* handle;

    /** Title of the window */
    const char* title;

    /** Width of the window */
    uint32_t width;
    /** Height of the window */
    uint32_t height;
    /** SDL flags for the window */
    uint32_t flags;

} Window;

/**
 * @brief Create an SDL window.
 * @param title The title of the window.
 * @param width The width of the window.
 * @param height The height of the window.
 * @param SDL_flags The SDL flags to add to the window.
 * @returns `Window`
 */
Window
G_CreateWindow(
    const char* title,
    int width,
    int height,
    uint32_t flags
);

/**
 * Destroy the given SDL window.
 * @param window The window.
 * @returns void
 */
void
G_DestroyWindow(Window* window);


#endif