#include "CollisionHelpers.h"
#include "../GEngine.h"
#include "CollisionComponents.h"

#include "CollisionSystem.h"

using namespace DirectX;
using namespace std;
void Collision::CreateSphere(const Shapes::FSphere& fSphere,
                             EntityHandle           entityH,
                             ComponentHandle*       sphereHandle,
                             ComponentHandle*       aabbHandle)
{
        // create handle for shapes, the grid container will have aabb box as the main object to detect collision
        CollisionSystem* gCollisionSystem = SYSTEM_MANAGER->GetSystem<CollisionSystem>();

        ComponentHandle sHandle = entityH.GetComponentHandle<SphereComponent>();
        ComponentHandle aHandle = entityH.GetComponentHandle<AABBComponent>();

        SphereComponent* sComponent = sHandle.Get<SphereComponent>();
        sComponent->sphere          = fSphere;
        AABBComponent* aComponent   = aHandle.Get<AABBComponent>();
        aComponent->aabb            = CollisionLibary::CreateBoundingBoxFromShpere(fSphere);

        // adding component handle and shap types in the grid container
        CollisionGrid::Cell cellpos;
        cellpos.x = (int)(XMVectorGetX(aComponent->aabb.center) / CollisionGrid::CellSize);
        cellpos.y = (int)(XMVectorGetY(aComponent->aabb.center) / CollisionGrid::CellSize);
        cellpos.z = (int)(XMVectorGetZ(aComponent->aabb.center) / CollisionGrid::CellSize);

        int index = gCollisionSystem->GetCollisionGrid().ComputeHashBucketIndex(cellpos);

        std::vector<CollisionID> IDs;

        CollisionID id;
        id.type   = Shapes::ECollisionObjectTypes::Sphere;
        id.handle = *sphereHandle;
        IDs.push_back(id);

        std::pair<int, std::vector<CollisionID>> item(index, IDs);
        gCollisionSystem->GetCollisionGrid().GridContainers.insert(item);

        if (aabbHandle)
                *aabbHandle = aHandle;
        if (sphereHandle)
                *sphereHandle = sHandle;
}

void Collision::CreateAABB(const Shapes::FAabb& fAABB, EntityHandle entityH, ComponentHandle* aabbHandle)
{
        CollisionSystem* gCollisionSystem = SYSTEM_MANAGER->GetSystem<CollisionSystem>();

        // create handle for shapes, the grid container will have aabb box as the main object to detect collision
        ComponentHandle aHandle = entityH.AddComponent<AABBComponent>();
        AABBComponent*  aComponent = aHandle.Get<AABBComponent>();
        aComponent->aabb           = fAABB;

        // adding component handle and shap types in the grid container
        CollisionGrid::Cell cellpos;
        cellpos.x = (int)(XMVectorGetX(aComponent->aabb.center) / CollisionGrid::CellSize);
        cellpos.y = (int)(XMVectorGetY(aComponent->aabb.center) / CollisionGrid::CellSize);
        cellpos.z = (int)(XMVectorGetZ(aComponent->aabb.center) / CollisionGrid::CellSize);

        int index = gCollisionSystem->GetCollisionGrid().ComputeHashBucketIndex(cellpos);

        std::vector<CollisionID> IDs;

        CollisionID id;
        id.type   = Shapes::ECollisionObjectTypes::Sphere;
        id.handle = *aabbHandle;
        IDs.push_back(id);

        std::pair<int, std::vector<CollisionID>> item(index, IDs);
        gCollisionSystem->GetCollisionGrid().GridContainers.insert(item);

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

        int index = gCollisionSystem->GetCollisionGrid().ComputeHashBucketIndex(cellpos);

        std::vector<CollisionID> IDs;

        CollisionID id;
        id.type   = Shapes::ECollisionObjectTypes::Sphere;
        id.handle = *capsuleHandle;
        IDs.push_back(id);

        std::pair<int, std::vector<CollisionID>> item(index, IDs);
        gCollisionSystem->GetCollisionGrid().GridContainers.insert(item);

        if (capsuleHandle)
                *capsuleHandle = cHandle;

        if (aabbHAndle)
                *aabbHAndle = aHandle;
}
