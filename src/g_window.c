#include "g_window.h"

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

void
window_destroy(Window* window) {
    SDL_DestroyWindow(window->handle);
}