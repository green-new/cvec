#ifndef OCTREE_H
#define OCTREE_H

#include "math/vec.h"
#include "entity/entity.h"

typedef struct aabb {

   vec min;
   vec max;
   vec center;

} AABB;

typedef struct OctreeNode {
   Entity objects;

   OctreeNode* children[8];
   OctreeNode* parent;

   int isLeaf;
} OctreeNode;

typedef struct Octree {
   /**  */
   
} Octree;

#endif // OCTREE_H