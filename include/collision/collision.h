#ifndef COLLISION_H__
#define COLLISION_H__

#include "entity/entity.h"
#include "math/vec.h"

typedef struct CollideEvent {
   
    Entity p;
    Entity q;

    vec position;
    vec impulse_point;

} CollideEvent;

#endif