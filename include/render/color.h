#ifndef COLOR_H__
#define COLOR_H__

#include "math/vec.h"

typedef union Color {
    char rgba[4];
    unsigned int value;
} Color;

vec4
color_color_to_vec4(const Color* a);

Color
color_vec4_to_color(const vec4* a);


#endif