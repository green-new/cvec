#ifndef VEC_H_
#define VEC_H_

#include <math.h>

#define M_2_PI 6.28318530717958647692
#define M_1_PI 0.31830988618379067153803535746773
#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679489661923
#define M_PI_4 0.785398163397448309615
#define M_ROOT_2 1.4142135623730950488016887242097
#define M_ROOT_1_2 0.70710678118654752440084436210485

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

typedef struct Orientation {

  vec_t eye;
  vec_t up;
  vec_t right;

} Orientation;

typedef enum eProjectionType {
    PROJECTION_TYPE_PERSPECTIVE
} ProjectionType;

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

void
Vec4_MultiplyMatrix(vec4_t* vec, const mat4_t* mat);

mat4_t
Mat4_TranslationMatrix(float x, float y, float z);

mat4_t
Mat4_RotationMatrix(vec_t axis, float theta);

void
Mat4_Rotate(mat4_t* mat, vec_t axis, float theta);

void
Vec4_Translate(vec4_t* vec, float x, float y, float z, float w);

void
Vec_Translate(vec_t* vec, float x, float y, float z);

void
Vec4_Scale(vec4_t* self, float scale);

mat4_t
Mat4_PerspectiveProjection(
    float fov, 
    float aspect, 
    float zNear, 
    float zFar);

mat4_t
Mat4_LookAt(Orientation orient);

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

void
Vec4_MultiplyMatrix(vec4_t* vec, const mat4_t* mat) {
    vec->x = (mat->m[0][0] * vec->x) 
        + (mat->m[0][1] * vec->x) 
        + (mat->m[0][2] * vec->x)
        + (mat->m[0][3] * vec->x);
    vec->y = (mat->m[1][0] * vec->y) 
        + (mat->m[1][1] * vec->y) 
        + (mat->m[1][2] * vec->y)
        + (mat->m[1][3] * vec->y);
    vec->z = (mat->m[2][0] * vec->z) 
        + (mat->m[2][1] * vec->z) 
        + (mat->m[2][2] * vec->z)
        + (mat->m[2][3] * vec->z);
    vec->w = (mat->m[3][0] * vec->w) 
        + (mat->m[3][1] * vec->w) 
        + (mat->m[3][2] * vec->w)
        + (mat->m[3][3] * vec->w);
}

mat4_t
Mat4_TranslationMatrix(float x, float y, float z) {
    mat4_t i = M_Mat4Identity();
    i.m[0][3] = x;
    i.m[1][3] = y;
    i.m[2][3] = z;
    return i;
}

void
Mat4_Rotate(mat4_t* mat, vec_t axis, float theta) {
    mat4_t rotation = Mat4_RotationMatrix(axis, theta);
    mat4_t res = M_MultiplyMat4(&rotation, mat);
    *mat = res;
}

mat4_t
Mat4_RotationMatrix(vec_t axis, float theta) {
    mat4_t res = { 0 };
    float costheta = cosf(theta);
    float sintheta = sinf(theta); 
    // Row 0
    res.m[0][0] = (axis.x * axis.x) * (1.0f - costheta) + costheta;
    res.m[0][1] = (axis.x * axis.y) * (1.0f - costheta) - (axis.z * sintheta);
    res.m[0][2] = (axis.x * axis.z) * (1.0f - costheta) + (axis.y * sintheta);
    res.m[0][3] = 0.0f;

    // Row 1
    res.m[1][0] = (axis.x * axis.y) * (1.0f - costheta) + (axis.z * sintheta);
    res.m[1][1] = (axis.y * axis.y) * (1.0f - costheta) + costheta;
    res.m[1][2] = (axis.y * axis.z) * (1.0f - costheta) - (axis.x * sintheta);
    res.m[1][3] = 0.0f;

    // Row 2
    res.m[2][0] = (axis.x * axis.z) * (1.0f - costheta) - (axis.y * sintheta);
    res.m[2][1] = (axis.y * axis.z) * (1.0f - costheta) + (axis.x * sintheta);
    res.m[2][2] = (axis.z * axis.z) * (1.0f - costheta) + costheta;
    res.m[2][3] = 0.0f;

    // Row 3
    res.m[3][0] = 0.0f;
    res.m[3][1] = 0.0f;
    res.m[3][2] = 0.0f;
    res.m[3][3] = 1.0f;
    return res;
}

void
Vec4_Translate(vec4_t* vec, float x, float y, float z, float w) {
    vec->x += x;
    vec->y += y;
    vec->z += z;
    vec->w += w;
}

void
Vec_Translate(vec_t* vec, float x, float y, float z) {
    vec->x += x;
    vec->y += y;
    vec->z += z;
}

void
Vec4_Scale(vec4_t* self, float scale) {
    self->x = self->x * scale;
    self->y = self->y * scale;
    self->z = self->z * scale;
    self->w = self->w;
}

mat4_t
Mat4_PerspectiveProjection(
    float fov, 
    float aspect, 
    float zNear, 
    float zFar) {

    mat4_t res = { 0 };
    res.m[0][0] = 1.0f / (aspect * tanf(fov / 2.0f));
    res.m[0][1] = 0.0f;
    res.m[0][2] = 0.0f;
    res.m[0][3] = 0.0f;

    res.m[1][0] = 0.0f;
    res.m[1][1] = 1.0f / tanf(fov / 2.0f);
    res.m[1][2] = 0.0f;
    res.m[1][3] = 0.0f;

    res.m[2][0] = 0.0f;
    res.m[2][1] = 0.0f;
    res.m[2][2] = -((zFar + zNear) / (zFar - zNear));
    res.m[2][3] = -((2.0f * zFar * zNear) / (zFar - zNear));

    res.m[3][0] = 0.0f;
    res.m[3][1] = 0.0f;
    res.m[3][2] = -1.0f;
    res.m[3][3] = 0.0f;
    return res;
}

mat4_t
Mat4_LookAt(Orientation orient) {
    mat4_t res = { 0 };
    res.m[0][0] = orient.right.x;
    res.m[0][1] = orient.right.y;
    res.m[0][2] = orient.right.z;
    res.m[0][3] = 0.0f;

    res.m[1][0] = orient.up.x;
    res.m[1][1] = orient.up.y;
    res.m[1][2] = orient.up.z;
    res.m[1][3] = 0.0f;

    res.m[2][0] = orient.eye.x;
    res.m[2][1] = orient.eye.y;
    res.m[2][2] = orient.eye.z;
    res.m[2][3] = 0.0f;

    res.m[3][0] = 0.0f;
    res.m[3][1] = 0.0f;
    res.m[3][2] = 0.0f;
    res.m[3][3] = 1.0f;
    return res; 
}

#endif // VEC_IMPL_H_
#endif // VEC_H_