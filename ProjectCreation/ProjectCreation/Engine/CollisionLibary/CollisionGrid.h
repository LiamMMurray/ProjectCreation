#pragma once

#define NUM_BUCKETS 1024
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "../../ECS/ECSTypes.h"
#include "../../Utility/Hashing/PairHash.h"
#include "CollisionHelpers.h"
#include "Shapes.h"

#include "CollisionResult.h"


class CollisionGrid
{


    public:
        struct Cell
        {
                int x;
                int y;
                int z;
        };

        struct CellContainer
        {
                std::vector<ComponentHandle> m_Spheres;
                std::vector<ComponentHandle> m_AABBs;
                std::vector<ComponentHandle> m_Capsules;
        };


        static constexpr int CellSize = 5;
        CollisionGrid();
        std::unordered_map<int, CellContainer> m_Container;
        Cell                                   GetCellFromShape(const Shapes::FCollisionShape* shape);
        int                                    ComputeHashBucketIndex(Cell cellPos);
        std::vector<const CellContainer*>      GetPossibleCollisions(Shapes::FCollisionShape* shape);
};
