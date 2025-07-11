#ifndef VERTEX_H_
#define VERTEX_H

#define VEC_IMPL_H_
#include "c_math.h"

typedef struct {
    float x;
    float y;
} Vec2;

/* Game vertex. */
typedef struct {

    float position[2];
    float color[3];

} Vertex;

#endif // VERTEX_H_