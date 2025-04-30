#include "math/vec.h"

typedef struct Body {

   float mass;
   float temperature;

   vec velocity;
   vec force;

} Body;