#ifndef VEC_H_
#define VEC_H_

#include "math.h"

/**
 * @struct vec
 * @brief Mathematical vector of 3 floating-point values.
 */
typedef struct vec {
    /**
     * @brief X value of the vector.
     */
    float x;

    /**
     * @brief Y value of the vector.
     */
    float y;
    
    /**
     * @brief Z value of the vector.
     */
    float z;
} vec;

typedef struct vec4 {
    /**
     * @brief X value of the vector.
     */
    float x;
    
    /**
     * @brief Y value of the vector.
     */
    float y;
    
    /**
     * @brief Z value of the vector.
     */
    float z;
    
    /**
     * @brief W value of the vector.
     */
    float w;
} vec4;

typedef struct mat4 {
    /**
     * @brief Matrix array.
     */
    float m[4][4];
} mat4;

/**
 * @brief Adds two 3-dimension vectors together.
 * @param a The first vector
 * @param b The second vector
 * @return The summed vector
 */
vec
vec_add(const vec* a, const vec* b) {
    return (vec) {
        .x = a->x + b->x,
        .y = a->y + b->y,
        .z = a->z + b->z
    };
}

/**
 * @brief Adds two 4-dimension vectors together.
 * @param a The first vector
 * @param b The second vector
 * @return The summed vector
 */
vec4
vec4_add(const vec4* a, const vec4* b) {
    return (vec4) {
        .x = a->x + b->x,
        .y = a->y + b->y,
        .z = a->z + b->z,
        .w = a->w + b->w
    };
}

/**
 * @brief Subtracts two 3-dimension vectors together.
 * @param a The first vector
 * @param b The second vector
 * @return The subtracted vector
 */
vec
vec_sub(const vec* a, const vec* b) {
    return (vec) {
        .x = a->x - b->x,
        .y = a->y - b->y,
        .z = a->z - b->z
    };
}

/**
 * @brief Subtract two 4-dimension vectors together.
 * @param a The first vector
 * @param b The second vector
 * @return The subtracted vector
 */
vec4
vec4_sub(const vec4* a, const vec4* b) {
    return (vec4) {
        .x = a->x - b->x,
        .y = a->y - b->y,
        .z = a->z - b->z,
        .w = a->w - b->w
    };
}

/**
 * @brief Multiply two 3-dimension vectors together.
 * @param a The first vector
 * @param b The second vector
 * @return The multiplied vector
 */
vec
vec_mul(const vec* a, const vec* b) {
    return (vec) {
        .x = a->x * b->x,
        .y = a->y * b->y,
        .z = a->z * b->z
    };
}

/**
 * @brief Multiply two 4-dimension vectors together.
 * @param a The first vector
 * @param b The second vector
 * @return The multiplied vector
 */
vec4
vec4_mul(const vec4* a, const vec4* b) {
    return (vec4) {
        .x = a->x * b->x,
        .y = a->y * b->y,
        .z = a->z * b->z,
        .w = a->w * b->w
    };
}

/**
 * @brief Multiply a vector by the scalar amount
 * @param a The vector to multiply
 * @param s The scalar amount
 * @return The multipled vector
 */
vec
vec_muls(const vec* a, float s) {
    return (vec) {
        .x = a->x * s,
        .y = a->y * s,
        .z = a->z * s
    };
}

/**
 * @brief Divide a given vector by the scalar amount
 * @param a The vector to divide
 * @param s The scalar amount
 * @return The divided vector
 */
vec
vec_div(const vec* a, float s) {
    return (vec) {
        .x = a->x / s,
        .y = a->y / s,
        .z = a->z / s
    };
}

/**
 * @brief Normalize the vector in place
 * @param a The vector to normalize, in place
 * @return void
 */
void
vec_normalize(vec* a) {
    float len = vec_distance(a);

    a->x /= len;
    a->y /= len;
    a->z /= len;
}

/**
 * @brief Get the dot product of two vectors
 * @param a The first vector
 * @param b The second vector
 * @return The dot product
 */
float
dot(const vec* a, const vec* b) {
    return (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
}

/**
 * @brief Get the cross product of two vectors
 * @param a The first vector
 * @param b The second vector
 * @return The cross product
 */
vec
cross(const vec* a, const vec* b) {
    return (vec) {
        .x = (a->y * b->z) - (b->y * a->z),
        .y = (a->z * b->x) - (b->z * a->x),
        .z = (a->x * b->y) - (b->x * a->y)
    };
}

/**
 * @brief Get the magnitude of the vector
 * @param a The first vector
 * @return The magnitude or distance of the vector
 */
float
vec_distance(const vec* a) {
    return sqrtf((a->x * a->x) + (a->y * a->y) + (a->z * a->z));
}

/**
 * @brief Multiply two mat4 matrices together
 * @param a The first vector
 * @return The multipled mat4
 */
mat4
mat4_mul(const mat4* a, const mat4* b) {
    mat4 res = (mat4) {
        .m = {0}
    };

    // naiive implementation
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

/**
 * @brief Return the identity vec4
 * @return The identity vec4
 */
vec4
vec4_identity(void) {
  return (vec4) {
    .x = 0.0f,
    .y = 0.0f,
    .z = 0.0f,
    .w = 1.0f,
  };
}

/**
 * @brief Return the identity mat4
 * @return The identity mat4
 */
mat4
mat4_identity(void) {
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