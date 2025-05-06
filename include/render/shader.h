#ifndef SHADER_H__
#define SHADER_H__

#include "math/vec.h"

typedef enum ShaderResult {
   SUCCESS = 0,
   ABORTED = 1,
   INVALID_FILENAME = 2,
   FAILED_LINK = 3,
   UNIFORM_NOT_FOUND = 4
};

typedef struct Shader {
   const char* name;
   unsigned int id;
} Shader;

ShaderResult
shader_create(Shader* shader, const char* name);

ShaderResult
shader_destroy(Shader* shader);

ShaderResult
shader_uniform_set_vec(const Shader* shader, const char* uniform, const vec* a);

ShaderResult
shader_uniform_set_vec2(const Shader* shader, const char* uniform, float a, float b);

ShaderResult
shader_uniform_set_vec4(const Shader* shader, const char* uniform, const vec4* a);

ShaderResult
shader_uniform_set_mat4(const Shader* shader, const char* uniform, const mat4* a);

ShaderResult
shader_uniform_set_int(const Shader* shader, const char* uniform, int a);

ShaderResult
shader_uniform_set_float(const Shader* shader, const char* uniform, float a);

ShaderResult
shader_uniform_set_string(const Shader* shader, const char* uniform, const char* a);

ShaderResult
shader_uniform_set_bool(const Shader* shader, const char* uniform, bool a);

ShaderResult
shader_uniform_set_double(const Shader* shader, const char* uniform, double a);

ShaderResult
shader_uniform_set_void(const Shader* shader, const char* uniform, const void* a);

#endif