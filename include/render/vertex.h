#ifndef VERTEX_H__
#define VERTEX_H__

#include <SDL3/SDL_vulkan.h>

typedef struct Vertex {
    float position[3];
    float color[3];
    float normal[3];
    float texture[2];
} Vertex;

#endif