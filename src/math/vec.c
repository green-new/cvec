#include "math/vec.h"
#include <math.h>

#ifndef VECMATH_IMPL__
#define VECMATH_IMPL__

vec
vec_add(const vec* a, const vec* b) {
  return (vec) {
    .x = a->x + b->x,
    .y = a->y + b->y,
    .z = a->z + b->z
  };
}

vec4
vec4_add(const vec4* a, const vec4* b) {
  return (vec4) {
    .x = a->x + b->x,
    .y = a->y + b->y,
    .z = a->z + b->z,
    .w = a->w + b->w
  };
}

vec
vec_sub(const vec* a, const vec* b) {
  return (vec) {
    .x = a->x - b->x,
    .y = a->y - b->y,
    .z = a->z - b->z
  };
}

vec4
vec4_sub(const vec4* a, const vec4* b) {
  return (vec4) {
    .x = a->x - b->x,
    .y = a->y - b->y,
    .z = a->z - b->z,
    .w = a->w - b->w
  };
}

vec
vec_mul(const vec* a, const vec* b) {
  return (vec) {
    .x = a->x * b->x,
    .y = a->y * b->y,
    .z = a->z * b->z
  };
}

vec4
vec4_mul(const vec4* a, const vec4* b) {
  return (vec4) {
    .x = a->x * b->x,
    .y = a->y * b->y,
    .z = a->z * b->z,
    .w = a->w * b->w
  };
}

vec
vec_mul(const vec* a, float s) {
  return (vec) {
    .x = a->x * s,
    .y = a->y * s,
    .z = a->z * s
  };
}

vec
vec_div(const vec* a, float s) {
  return (vec) {
    .x = a->x / s,
    .y = a->y / s,
    .z = a->z / s
  };
}

void
vec_normalize(vec* a) {
  float len = vec_distance(a);

  a->x /= len;
  a->y /= len;
  a->z /= len;
}

float
dot(const vec* a, const vec* b) {
  return (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
}

vec
vec_cross(const vec* a, const vec* b) {
  return (vec) {
    .x = (a->y * b->z) - (b->y * a->z),
    .y = (a->z * b->x) - (b->z * a->x),
    .z = (a->x * b->y) - (b->x * a->y)
  };
}

float
vec_distance(const vec* a) {
  return sqrtf((a->x * a->x) + (a->y * a->y) + (a->z * a->z));
}

mat4
mat4_mul(const mat4* a, const mat4* b) {
  mat4 res = (mat4) {
    .m = {0}
  };

  for (unsigned char i = 0; i < 4; i++) {
    for (unsigned char j = 0; j < 4; j++) {
      res.m[i][j] = 0.0f;
      for (unsigned char k = 0; k < 4; k++) {
        res.m[i][j] += a->m[i][k] * b->m[k][j];
      }
    }
  }

  return res;
}

vec4
vec4_identity() {
  return (vec4) {
    .x = 0.0f,
    .y = 0.0f,
    .z = 0.0f,
    .w = 0.0f,
  };
}

mat4
mat4_identity() {
  return (mat4) {
    .m = {
      {1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f}
    }
  };
}



#endif