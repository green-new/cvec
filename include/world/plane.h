#ifndef PLANE_H__
#define PLANE_H__

#include "math/vec.h"

typedef struct Plane {
   float d;
   vec normal;
} Plane;


#endif