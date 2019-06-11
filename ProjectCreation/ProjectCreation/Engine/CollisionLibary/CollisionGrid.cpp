#include "CollisionGrid.h"
#include <DirectXMath.h>
#include "../MathLibrary/MathLibrary.h"
using namespace DirectX;

CollisionGrid::CollisionGrid()
{}

CollisionGrid::Cell CollisionGrid::GetCellFromShape(const Shapes::FCollisionShape* shape)
{
        Shapes::ECollisionObjectTypes typeID = shape->GetID();
        XMVECTOR                      center = XMVectorZero();
        switch (typeID)
        {
                case Shapes::Sphere:
                {
                        Shapes::FSphere* sphere = (Shapes::FSphere*)shape;
                        center                  = sphere->center;
                }
                break;
                case Shapes::Aabb:
                {
                        Shapes::FAabb* aabb = (Shapes::FAabb*)shape;
                        center              = aabb->center;
                }
                break;

                case Shapes::Capsule:
                {
                        Shapes::FCapsule* capsule = (Shapes::FCapsule*)shape;
                        center = MathLibrary::GetMidPointFromTwoVector(capsule->startPoint, capsule->endPoint);
                }
                break;
                default:
                        break;
        }

        Cell cellpos;
        cellpos.x = (int)(XMVectorGetX(center) / CollisionGrid::CellSize);
        cellpos.y = 0; // XMVectorGetY(center) / CollisionGrid::CellSize;
        cellpos.z = (int)(XMVectorGetZ(center) / CollisionGrid::CellSize);

        return cellpos;
}

int CollisionGrid::ComputeHashBucketIndex(Cell cellPos)
{
        const int h1 = 0x8da6b343; // Arbitrary, large primes.
        const int h2 = 0xd8163841; // Primes are popular for hash functions
        const int h3 = 0xcb1ab31f; // for reducing the chance of hash collision.
        int       n  = h1 * cellPos.x + h2 * cellPos.y + h3 * cellPos.z;
        n            = n % NUM_BUCKETS; // Wrap indices to stay in bucket range
        if (n < 0)
                n += NUM_BUCKETS; // Keep indices in positive range
        return n;
}

using namespace Collision;

Collision::FCollisionQueryResult CollisionGrid::GetPossibleCollisions(Shapes::FCollisionShape* shape)
{
        Shapes::ECollisionObjectTypes typeID = shape->GetID();
        FCollisionQueryResult         output;

        Cell             checkCell = GetCellFromShape(shape);
        int              index     = ComputeHashBucketIndex(checkCell);
        auto             it        = m_Container.find(index);

        if (it != m_Container.end())
        {
                output.spheres  = it->second.m_Spheres;
                output.AABBs    = it->second.m_AABBs;
                output.capsules = it->second.m_Capsules;
        }

        return output;
}
