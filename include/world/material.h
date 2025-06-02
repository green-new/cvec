#ifndef MATERIAL_H__
#define MATERIAL_H__

#include "render/color.h"

typedef struct material {

    Color base_color;

    float reflect;

} material;

#endif