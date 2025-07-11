#ifndef VEC_H_
#define VEC_H_

#include <math.h>

/**
 * @struct vec
 * @brief Mathematical vector of 3 floating-point values.
 */
typedef struct {
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
} vec_t;

typedef struct vec4_t {
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
} vec4_t;

typedef struct mat4_t {
    /**
     * @brief Matrix array.
     */
    float m[4][4];
} mat4_t;

typedef struct mat3_t {
    /**
     * @brief Matrix array.
     */
    float m[3][3];
} mat3_t;

/**
 * @brief Adds two 3-dimension vectors together.
 * @param a The first vector
 * @param b The second vector
 * @return The summed vector
 */
vec_t
M_AddVec(const vec_t* a, const vec_t* b);

/**
 * @brief Adds two 4-dimension vectors together.
 * @param a The first vector
 * @param b The second vector
 * @return The summed vector
 */
vec4_t
M_AddVec4(const vec4_t* a, const vec4_t* b);

/**
 * @brief Subtracts two 3-dimension vectors together.
 * @param a The first vector
 * @param b The second vector
 * @return The subtracted vector
 */
vec_t
M_SubtractVec(const vec_t* a, const vec_t* b);

/**
 * @brief Subtract two 4-dimension vectors together.
 * @param a The first vector
 * @param b The second vector
 * @return The subtracted vector
 */
vec4_t
M_SubtractVec4(const vec4_t* a, const vec4_t* b);

/**
 * @brief Multiply two 3-dimension vectors together.
 * @param a The first vector
 * @param b The second vector
 * @return The multiplied vector
 */
vec_t
M_MultiplyVec(const vec_t* a, const vec_t* b);

/**
 * @brief Multiply two 4-dimension vectors together.
 * @param a The first vector
 * @param b The second vector
 * @return The multiplied vector
 */
vec4_t
M_MultiplyVec4(const vec4_t* a, const vec4_t* b);

/**
 * @brief Multiply a vector by the scalar amount
 * @param a The vector to multiply
 * @param s The scalar amount
 * @return The multipled vector
 */
vec_t
M_MultiplyVecByScalar(const vec_t* a, float s);

/**
 * @brief Divide a given vector by the scalar amount
 * @param a The vector to divide
 * @param s The scalar amount
 * @return The divided vector
 */
vec_t
M_DivideVec(const vec_t* a, float s);

/**
 * @brief Get the magnitude of the vector
 * @param a The first vector
 * @return The magnitude or distance of the vector
 */
float
M_Distance(const vec_t* a);

/**
 * @brief Normalize the vector in place
 * @param a The vector to normalize, in place
 * @return void
 */
void
M_NormalizeVec(vec_t* a);

/**
 * @brief Get the dot product of two vectors
 * @param a The first vector
 * @param b The second vector
 * @return The dot product
 */
float
M_Dot(const vec_t* a, const vec_t* b);

/**
 * @brief Get the cross product of two vectors
 * @param a The first vector
 * @param b The second vector
 * @return The cross product
 */
vec_t
M_Cross(const vec_t* a, const vec_t* b);

/**
 * @brief Get the magnitude of the vector
 * @param a The first vector
 * @return The magnitude or distance of the vector
 */
float
M_SumOfSquares(const vec_t* a);

/**
 * @brief Multiply two mat4 matrices together
 * @param a The first mat
 * @param b The second mat
 * @return The multipled mat4
 */
mat4_t
M_MultiplyMat4(const mat4_t* a, const mat4_t* b);

/**
 * @brief Multiply two mat3 matrices together
 * @param a The first mat
 * @param b The second mat
 * @return The multipled mat3
 */
mat3_t
M_MultiplyMat3(const mat3_t* a, const mat3_t* b);

/**
 * @brief Return the identity vec4
 * @return The identity vec4
 */
vec4_t
M_Vec4Identity(void);

/**
 * @brief Return the identity mat4
 * @return The identity mat4
 */
mat4_t
M_Mat4Identity(void);

/**
 * @brief Return the identity mat4
 * @return The identity mat4
 */
mat3_t
M_Mat3Identity(void);

#ifndef VEC_IMPL_H_

vec_t
M_AddVec(const vec_t* a, const vec_t* b) {
    return (vec_t) {
        .x = a->x + b->x,
        .y = a->y + b->y,
        .z = a->z + b->z
    };
}

vec4_t
M_AddVec4(const vec4_t* a, const vec4_t* b) {
    return (vec4_t) {
        .x = a->x + b->x,
        .y = a->y + b->y,
        .z = a->z + b->z,
        .w = a->w + b->w
    };
}

vec_t
M_SubtractVec(const vec_t* a, const vec_t* b) {
    return (vec_t) {
        .x = a->x - b->x,
        .y = a->y - b->y,
        .z = a->z - b->z
    };
}

vec4_t
M_SubtractVec4(const vec4_t* a, const vec4_t* b) {
    return (vec4_t) {
        .x = a->x - b->x,
        .y = a->y - b->y,
        .z = a->z - b->z,
        .w = a->w - b->w
    };
}

vec_t
M_MultiplyVec(const vec_t* a, const vec_t* b) {
    return (vec_t) {
        .x = a->x * b->x,
        .y = a->y * b->y,
        .z = a->z * b->z
    };
}

vec4_t
M_MultiplyVec4(const vec4_t* a, const vec4_t* b) {
    return (vec4_t) {
        .x = a->x * b->x,
        .y = a->y * b->y,
        .z = a->z * b->z,
        .w = a->w * b->w
    };
}

vec_t
M_MultiplyVecByScalar(const vec_t* a, float s) {
    return (vec_t) {
        .x = a->x * s,
        .y = a->y * s,
        .z = a->z * s
    };
}

vec_t
M_DivideVec(const vec_t* a, float s) {
    return (vec_t) {
        .x = a->x / s,
        .y = a->y / s,
        .z = a->z / s
    };
}

float
M_Distance(const vec_t* a) {
    return sqrtf((a->x * a->x) + (a->y * a->y) + (a->z * a->z));
}

void
M_NormalizeVec(vec_t* a) {
    float len = M_Distance(a);

    a->x /= len;
    a->y /= len;
    a->z /= len;
}

float
M_Dot(const vec_t* a, const vec_t* b) {
    return (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
}

vec_t
M_Cross(const vec_t* a, const vec_t* b) {
    return (vec_t) {
        .x = (a->y * b->z) - (b->y * a->z),
        .y = (a->z * b->x) - (b->z * a->x),
        .z = (a->x * b->y) - (b->x * a->y)
    };
}

float
M_SumOfSquares(const vec_t* a) {
    return (a->x * a->x) + (a->y * a->y) + (a->z * a->z);
}

mat4_t
M_MultiplyMat4(const mat4_t* a, const mat4_t* b) {
    mat4_t res = (mat4_t) {
        .m = { { 0 } }
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

mat3_t
M_MultiplyMat3(const mat3_t* a, const mat3_t* b) {
    mat3_t res = (mat3_t) {
        .m = { { 0 } }
    };

    // naiive implementation
    for (unsigned char i = 0; i < 3; i++) {
        for (unsigned char j = 0; j < 3; j++) {
            res.m[i][j] = 0.0f;
            for (unsigned char k = 0; k < 3; k++) {
                res.m[i][j] += a->m[i][k] * b->m[k][j];
            }
        }
    }

    return res;
}

vec4_t
M_Vec4Identity(void) {
  return (vec4_t) {
    .x = 0.0f,
    .y = 0.0f,
    .z = 0.0f,
    .w = 1.0f,
  };
}

mat4_t
M_Mat4Identity(void) {
    return (mat4_t) {
        .m = {
            {1.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f}
        }
    };
}

mat3_t
M_Mat3Identity(void) {
    return (mat3_t) {
        .m = {
            {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f}
        }
    };
}

#endif
#endif // VEC_H_