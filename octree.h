#ifndef OCTREE_H
#define OCTREE_H

#include "vecmath.h"


typedef struct aabb {
   vec min;
   vec max;
   vec center;
} aabb;

typedef struct world_object {
   aabb box;
   vec position;
} world_object;

typedef struct octree_node {
   world_object m_objects;

   octree_node* m_children[8];

   int m_isLeaf;
} octree_node;

struct octree_t {
   /**  */

};

#endif // OCTREE_H