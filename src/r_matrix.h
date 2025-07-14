#ifndef MATRIX_H_
#define MATRIX_H_

#include "c_math.h"

typedef struct R_UniformBufferObject {

  mat4_t model;
  mat4_t view;
  mat4_t proj;

} R_UniformBufferObject;

#endif // MATRIX_H_