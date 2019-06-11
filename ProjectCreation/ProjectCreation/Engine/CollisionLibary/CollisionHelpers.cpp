#include "CollisionHelpers.h"
#include "../GEngine.h"
#include "CollisionComponents.h"
#include "CollisionLibary.h"
#include "CollisionSystem.h"

using namespace DirectX;
using namespace std;
void Collision::CreateSphere(const Shapes::FSphere& fSphere,
                             EntityHandle           entityH,
                             ComponentHandle*       sphereHandle,
                             ComponentHandle*       aabbHAndle)
{
        // create handle for shapes, the grid container will have aabb box as the main object to detect collision
        CollisionSystem* gCollisionSystem = SYSTEM_MANAGER->GetSystem<CollisionSystem>();

        ComponentHandle sHandle = COMPONENT_MANAGER->AddComponent<SphereComponent>(entityH);
        ComponentHandle aHandle = COMPONENT_MANAGER->AddComponent<AABBComponent>(entityH);

        SphereComponent* sComponent = COMPONENT_MANAGER->GetComponent<SphereComponent>(sHandle);
        sComponent->sphere          = fSphere;
        AABBComponent* aComponent   = COMPONENT_MANAGER->GetComponent<AABBComponent>(aHandle);
        aComponent->aabb            = CollisionLibary::CreateBoundingBoxFromShpere(fSphere);

        // adding component handle and shap types in the grid container
        CollisionGrid::Cell cellpos;
        cellpos.x = (int)(XMVectorGetX(aComponent->aabb.center) / CollisionGrid::CellSize);
        cellpos.y = (int)(XMVectorGetY(aComponent->aabb.center) / CollisionGrid::CellSize);
        cellpos.z = (int)(XMVectorGetZ(aComponent->aabb.center) / CollisionGrid::CellSize);

        int index = gCollisionSystem->GetCollisionGrid().ComputeHashBucketIndex(cellpos);

		auto it = gCollisionSystem->GetCollisionGrid().m_Container.try_emplace(index, CollisionGrid::CellContainer());
        it.first->second.m_Spheres.push_back(sHandle);

        if (aabbHAndle)
                *aabbHAndle = aHandle;
        if (sphereHandle)
                *sphereHandle = sHandle;
}

void Collision::CreateAABB(const Shapes::FAabb& fAABB, EntityHandle entityH, ComponentHandle* aabbHandle)
{
        CollisionSystem* gCollisionSystem = SYSTEM_MANAGER->GetSystem<CollisionSystem>();

        // create handle for shapes, the grid container will have aabb box as the main object to detect collision
        ComponentHandle aHandle    = COMPONENT_MANAGER->AddComponent<AABBComponent>(entityH);
        AABBComponent*  aComponent = COMPONENT_MANAGER->GetComponent<AABBComponent>(aHandle);
        aComponent->aabb           = fAABB;

        // adding component handle and shap types in the grid container
        CollisionGrid::Cell cellpos;
        cellpos.x = (int)(XMVectorGetX(aComponent->aabb.center) / CollisionGrid::CellSize);
        cellpos.y = (int)(XMVectorGetY(aComponent->aabb.center) / CollisionGrid::CellSize);
        cellpos.z = (int)(XMVectorGetZ(aComponent->aabb.center) / CollisionGrid::CellSize);

        int index = gCollisionSystem->GetCollisionGrid().ComputeHashBucketIndex(cellpos);

        auto it = gCollisionSystem->GetCollisionGrid().m_Container.try_emplace(index, CollisionGrid::CellContainer());
        it.first->second.m_AABBs.push_back(aHandle);

        if (aabbHandle)
                *aabbHandle = aHandle;
}

void Collision::CreateCapsule(const Shapes::FCapsule& fCapsule,
                              EntityHandle            entityH,
                              ComponentHandle*        capsuleHandle,
                              ComponentHandle*        aabbHAndle)
{
        CollisionSystem* gCollisionSystem = SYSTEM_MANAGER->GetSystem<CollisionSystem>();

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

        int index = gCollisionSystem->GetCollisionGrid().ComputeHashBucketIndex(cellpos);

        auto it = gCollisionSystem->GetCollisionGrid().m_Container.try_emplace(index, CollisionGrid::CellContainer());
        it.first->second.m_Capsules.push_back(cHandle);

        if (capsuleHandle)
                *capsuleHandle = cHandle;

        if (aabbHAndle)
                *aabbHAndle = aHandle;
}
