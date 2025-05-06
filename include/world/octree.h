#ifndef OCTREE_H
#define OCTREE_H

#include "math/vec.h"
#include "entity/entity.h"
#include "world/worldPlane.h"

typedef struct aabb {

   vec min;
   vec max;
   vec center;

} AABB;

typedef struct OctreeNode {
   WorldPlane objects;

   OctreeNode* children[8];
   OctreeNode* parent;

   int isLeaf;
} OctreeNode;

typedef struct Octree {
   /** Root of the octree. */
   OctreeNode* root;
} Octree;

#endif // OCTREE_H