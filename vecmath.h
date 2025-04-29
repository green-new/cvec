#ifndef VECMATH_H__
#define VECMATH_H__

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
  float x;
  float y;
  float z;
  float w;
} vec4;

typedef struct mat4 {
  float m[4][4];
} mat4;

/**
 * @brief Adds two 3-dimension vectors together.
 * @param a The first vector
 * @param b The second vector
 * @return The summed vector
 */
vec
vec_add(const vec* a, const vec* b);

/**
 * @brief Adds two 4-dimension vectors together.
 * @param a The first vector
 * @param b The second vector
 * @return The summed vector
 */
vec4
vec4_add(const vec4* a, const vec4* b);

/**
 * @brief Subtracts two 3-dimension vectors together.
 * @param a The first vector
 * @param b The second vector
 * @return The subtracted vector
 */
vec
vec_sub(const vec* a, const vec* b);

/**
 * @brief Subtract two 4-dimension vectors together.
 * @param a The first vector
 * @param b The second vector
 * @return The subtracted vector
 */
vec4
vec4_sub(const vec4* a, const vec4* b);

/**
 * @brief Multiply two 3-dimension vectors together.
 * @param a The first vector
 * @param b The second vector
 * @return The multiplied vector
 */
vec
vec_mul(const vec* a, const vec* b);

/**
 * @brief Multiply two 4-dimension vectors together.
 * @param a The first vector
 * @param b The second vector
 * @return The multiplied vector
 */
vec4
vec4_mul(const vec4* a, const vec4* b);

/**
 * @brief Multiply a vector by the scalar amount
 * @param a The vector to multiply
 * @param s The scalar amount
 * @return The multipled vector
 */
vec
vec_mul(const vec* a, float s);

/**
 * @brief Divide a given vector by the scalar amount
 * @param a The vector to divide
 * @param s The scalar amount
 * @return The divided vector
 */
vec
vec_div(const vec* a, float s);

/**
 * @brief Normalize the vector in place
 * @param a The vector to normalize, in place
 * @return void
 */
void
vec_normalize(const vec* a);

/**
 * @brief Get the dot product of two vectors
 * @param a The first vector
 * @param b The second vector
 * @return The dot product
 */
float
dot(const vec* a, const vec* b);

/**
 * @brief Get the cross product of two vectors
 * @param a The first vector
 * @param b The second vector
 * @return The cross product
 */
vec
cross(const vec* a, const vec* b);

/**
 * @brief Get the magnitude of the vector
 * @param a The first vector
 * @return The magnitude or distance of the vector
 */
float
vec_distance(const vec* a);

/**
 * @brief Multiply two mat4 matrices together
 * @param a The first vector
 * @return The multipled mat4
 */
mat4
mat4_mul(const mat4* a, const mat4* b);

/**
 * @brief Return the identity vec4
 * @return The identity vec4
 */
vec4
vec4_identity(void);

/**
 * @brief Return the identity mat4
 * @return The identity mat4
 */
mat4
mat4_identity(void);

#endif