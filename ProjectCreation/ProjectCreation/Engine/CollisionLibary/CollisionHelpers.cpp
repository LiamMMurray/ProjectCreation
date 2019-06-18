#include "CollisionHelpers.h"
#include "../GEngine.h"
#include "CollisionComponents.h"
#include "CollisionLibary.h"
#include "CollisionGrid.h"

using namespace DirectX;
using namespace std;
void Collision::CreateSphere(CollisionGrid*         grid,
                             const Shapes::FSphere& fSphere,
                             EntityHandle           entityH,
                             ComponentHandle*       sphereHandle,
                             ComponentHandle*       aabbHAndle)
{
        // create handle for shapes, the grid container will have aabb box as the main object to detect collision

        ComponentHandle sHandle = COMPONENT_MANAGER->AddComponent<SphereComponent>(entityH);
        ComponentHandle aHandle = COMPONENT_MANAGER->AddComponent<AABBComponent>(entityH);

        SphereComponent* sComponent = COMPONENT_MANAGER->GetComponent<SphereComponent>(sHandle);
        sComponent->sphere          = fSphere;

        // adding component handle and shap types in the grid container
        CollisionGrid::Cell cellpos;
        cellpos.x = (int)(XMVectorGetX(fSphere.center) / CollisionGrid::CellSize);
        cellpos.y = (int)(XMVectorGetY(fSphere.center) / CollisionGrid::CellSize);
        cellpos.z = (int)(XMVectorGetZ(fSphere.center) / CollisionGrid::CellSize);

        int index = grid->ComputeHashBucketIndex(cellpos);

		auto it = grid->m_Container.try_emplace(index, CollisionGrid::CellContainer());
        it.first->second.m_Spheres.push_back(sHandle);

        if (aabbHAndle)
                *aabbHAndle = aHandle;
        if (sphereHandle)
                *sphereHandle = sHandle;
}

void Collision::CreateAABB(CollisionGrid* grid, const Shapes::FAabb& fAABB, EntityHandle entityH, ComponentHandle* aabbHandle)
{
        // create handle for shapes, the grid container will have aabb box as the main object to detect collision
        ComponentHandle aHandle    = COMPONENT_MANAGER->AddComponent<AABBComponent>(entityH);
        AABBComponent*  aComponent = COMPONENT_MANAGER->GetComponent<AABBComponent>(aHandle);
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
        ComponentHandle cHandle = COMPONENT_MANAGER->AddComponent<CapsuleComponent>(entityH);
        ComponentHandle aHandle = COMPONENT_MANAGER->AddComponent<AABBComponent>(entityH);

        CapsuleComponent* sComponent = COMPONENT_MANAGER->GetComponent<CapsuleComponent>(cHandle);
        sComponent->capsule          = fCapsule;
        AABBComponent* aComponent    = COMPONENT_MANAGER->GetComponent<AABBComponent>(aHandle);
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
