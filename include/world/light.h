#ifndef LIGHT_H__
#define LIGHT_H__

#include "math/vec.h"
#include "render/color.h"

typedef struct Light {

   float intensity;
   Color color;
   vec direction;

} Light;

#endif