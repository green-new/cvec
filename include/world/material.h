#ifndef MATERIAL_H__
#define MATERIAL_H__

#include "render/color.h"

typedef struct Material {

   Color baseColor;

   float reflectivity;

} Material;

#endif