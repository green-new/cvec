#ifndef WORLD_VERTEX_H__
#define WORLD_VERTEX_H__

#include "math/vec.h"

#include "render/color.h"

#include "world/material.h"

typedef struct WorldVertex {

   vec position;
   vec normal;
   vec texture;
   
   Color color;

   Material material;


} WorldVertex;

#endif