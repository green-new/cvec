#ifndef WORLD_H__
#define WORLD_H__

#include "world/light.h"
#include "world/worldVertex.h"
#include "world/worldEdge.h"
#include "world/worldPlane.h"

#include "entity/entity.h"

#define WORLD_MAX_LIGHTS 1024
#define WORLD_MAX_VERTICES 65536
#define WORLD_MAX_EDGES 256000
#define WORLD_MAX_PLANES 256000

typedef struct World {

    Light lights[WORLD_MAX_LIGHTS];

    WorldVertex vertices[WORLD_MAX_VERTICES];
    WorldEdge edges[WORLD_MAX_EDGES];
    WorldPlane planes[WORLD_MAX_PLANES];

    Entity entities[ENTITY_MAX_ENTITIES];

} World;

#endif