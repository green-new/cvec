#include "math/vec.h"

typedef struct Body {

   int anchored;

   float mass;
   float temperature;
   float restituion;

   vec velocity;
   vec force;

} Body;