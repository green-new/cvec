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
} Vec;

typedef struct Vec4 {
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
} Vec4;

typedef struct Mat4 {
    /**
     * @brief Matrix array.
     */
    float m[4][4];
} Mat4;

typedef struct Mat3 {
    /**
     * @brief Matrix array.
     */
    float m[3][3];
} Mat3;

/**
 * @brief Adds two 3-dimension vectors together.
 * @param a The first vector
 * @param b The second vector
 * @return The summed vector
 */
Vec
M_AddVec(const Vec* a, const Vec* b) {
    return (Vec) {
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
Vec4
M_AddVec4(const Vec4* a, const Vec4* b) {
    return (Vec4) {
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
Vec
M_SubtractVec(const Vec* a, const Vec* b) {
    return (Vec) {
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
Vec4
M_SubtractVec4(const Vec4* a, const Vec4* b) {
    return (Vec4) {
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
Vec
M_MultiplyVec(const Vec* a, const Vec* b) {
    return (Vec) {
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
Vec4
M_MultiplyVec4(const Vec4* a, const Vec4* b) {
    return (Vec4) {
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
Vec
M_MultiplyVecByScalar(const Vec* a, float s) {
    return (Vec) {
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
Vec
M_DivideVec(const Vec* a, float s) {
    return (Vec) {
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
M_NormalizeVec(Vec* a) {
    float len = M_Distance(a);

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
M_Dot(const Vec* a, const Vec* b) {
    return (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
}

/**
 * @brief Get the cross product of two vectors
 * @param a The first vector
 * @param b The second vector
 * @return The cross product
 */
Vec
M_Cross(const Vec* a, const Vec* b) {
    return (Vec) {
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
M_SumOfSquares(const Vec* a) {
    return (a->x * a->x) + (a->y * a->y) + (a->z * a->z);
}


/**
 * @brief Get the magnitude of the vector
 * @param a The first vector
 * @return The magnitude or distance of the vector
 */
float
M_Distance(const Vec* a) {
    return sqrtf((a->x * a->x) + (a->y * a->y) + (a->z * a->z));
}

/**
 * @brief Multiply two mat4 matrices together
 * @param a The first mat
 * @param b The second mat
 * @return The multipled mat4
 */
Mat4
M_MultiplyMat4(const Mat4* a, const Mat4* b) {
    Mat4 res = (Mat4) {
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
 * @brief Multiply two mat3 matrices together
 * @param a The first mat
 * @param b The second mat
 * @return The multipled mat3
 */
Mat3
M_MultiplyMat3(const Mat3* a, const Mat3* b) {
    Mat3 res = (Mat3) {
        .m = {0}
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

/**
 * @brief Return the identity vec4
 * @return The identity vec4
 */
Vec4
M_Vec4Identity(void) {
  return (Vec4) {
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
Mat4
M_Mat4Identity(void) {
    return (Mat4) {
        .m = {
            {1.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f}
        }
    };
}

/**
 * @brief Return the identity mat4
 * @return The identity mat4
 */
Mat3
M_Mat3Identity(void) {
    return (Mat3) {
        .m = {
            {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f}
        }
    };
}

#endif