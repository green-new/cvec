#ifndef SHADER_H__
#define SHADER_H__

#include "enum/status.h"

typedef struct Shader {
   const char* name;
   unsigned int id;
} Shader;

/**
 * 
 */
StatusCode
shader_create(Shader* shader);

#endif