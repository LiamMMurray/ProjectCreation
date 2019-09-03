#include <CollisionHelpers.h>
#include <GEngine.h>
#include <CollisionComponents.h>
#include <CollisionGrid.h>
#include <CollisionLibary.h>

using namespace DirectX;
using namespace std;
void Collision::CreateSphere(CollisionGrid*         grid,
                             const Shapes::FSphere& fSphere,
                             EntityHandle           entityH,
                             ComponentHandle*       sphereHandle,
                             ComponentHandle*       aabbHandle)
{
        // create handle for shapes, the grid container will have aabb box as the main object to detect collision

        ComponentHandle  sHandle    = entityH.AddComponent<SphereComponent>();
        ComponentHandle  aHandle    = entityH.AddComponent<AABBComponent>();
        SphereComponent* sComponent = sHandle.Get<SphereComponent>();
        sComponent->sphere          = fSphere;

        // adding component handle and shap types in the grid container
        CollisionGrid::Cell cellpos;
        cellpos.x = (int)(XMVectorGetX(fSphere.center) / CollisionGrid::CellSize);
        cellpos.y = (int)(XMVectorGetY(fSphere.center) / CollisionGrid::CellSize);
        cellpos.z = (int)(XMVectorGetZ(fSphere.center) / CollisionGrid::CellSize);

        int index = grid->ComputeHashBucketIndex(cellpos);

        auto it = grid->m_Container.try_emplace(index, CollisionGrid::CellContainer());
        it.first->second.m_Spheres.push_back(sHandle);

        if (aabbHandle)
                *aabbHandle = aHandle;
        if (sphereHandle)
                *sphereHandle = sHandle;
}

void Collision::CreateAABB(CollisionGrid* grid, const Shapes::FAabb& fAABB, EntityHandle entityH, ComponentHandle* aabbHandle)
{
        // create handle for shapes, the grid container will have aabb box as the main object to detect collision
        ComponentHandle aHandle    = entityH.AddComponent<AABBComponent>();
        AABBComponent*  aComponent = aHandle.Get<AABBComponent>();
        aComponent->aabb           = fAABB;

        // adding component handle and shap types in the grid container
        CollisionGrid::Cell cellpos;
        cellpos.x = (int)(XMVectorGetX(aComponent->aabb.center) / CollisionGrid::CellSize);
        cellpos.y = (int)(XMVectorGetY(aComponent->aabb.center) / CollisionGrid::CellSize);
        cellpos.z = (int)(XMVectorGetZ(aComponent->aabb.center) / CollisionGrid::CellSize);

        int index = grid->ComputeHashBucketIndex(cellpos);

        auto it = grid->m_Container.try_emplace(index, CollisionGrid::CellContainer());
        it.first->second.m_AABBs.push_back(aHandle);

        if (aabbHandle)
                *aabbHandle = aHandle;
}

void Collision::CreateCapsule(CollisionGrid*          grid,
                              const Shapes::FCapsule& fCapsule,
                              EntityHandle            entityH,
                              ComponentHandle*        capsuleHandle,
                              ComponentHandle*        aabbHAndle)
{

        // create handle for shapes, the grid container will have aabb box as the main object to detect collision
        ComponentHandle cHandle = entityH.AddComponent<CapsuleComponent>();
        ComponentHandle aHandle = entityH.AddComponent<AABBComponent>();

        CapsuleComponent* sComponent = cHandle.Get<CapsuleComponent>();
        sComponent->capsule          = fCapsule;
        AABBComponent* aComponent    = aHandle.Get<AABBComponent>();
        aComponent->aabb             = CollisionLibary::CreateBoundingBoxFromCapsule(fCapsule);


        // adding component handle and shap types in the grid container
        CollisionGrid::Cell cellpos;
        cellpos.x = (int)(XMVectorGetX(aComponent->aabb.center) / CollisionGrid::CellSize);
        cellpos.y = (int)(XMVectorGetY(aComponent->aabb.center) / CollisionGrid::CellSize);
        cellpos.z = (int)(XMVectorGetZ(aComponent->aabb.center) / CollisionGrid::CellSize);

        int index = grid->ComputeHashBucketIndex(cellpos);

        auto it = grid->m_Container.try_emplace(index, CollisionGrid::CellContainer());
        it.first->second.m_Capsules.push_back(cHandle);

        if (capsuleHandle)
                *capsuleHandle = cHandle;

        if (aabbHAndle)
                *aabbHAndle = aHandle;
}
