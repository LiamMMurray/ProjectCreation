#pragma once

#define NUM_BUCKETS 1024
#include <unordered_map>
#include <vector>
#include "../../ECS/ECSTypes.h"
#include "CollisionHelpers.h"
#include "Shapes.h"
struct CollisionID
{
        Shapes::ECollisionObjectTypes type;
        ComponentHandle               handle;
};
class CollisionGrid
{


        int HashTableBuckets[NUM_BUCKETS];

    public:
        struct Cell
        {
                int x;
                int y;
                int z;
        };
        static constexpr int                                     CellSize = 5;
        std::unordered_map<int, std::vector<CollisionID>> GridContainers;

        CollisionGrid();
        Cell                     GetCellFromShape(const Shapes::FCollisionShape* shape);
        int                      ComputeHashBucketIndex(Cell cellPos);
        CollisionID              GetShapeId(Shapes::FCollisionShape& shape);
        const std::vector<CollisionID> GetPossibleCollisions(Shapes::FCollisionShape* shape);
};
