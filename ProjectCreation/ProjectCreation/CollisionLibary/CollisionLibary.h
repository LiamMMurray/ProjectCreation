#pragma once
#include"CollisionComponent.h"
class CollisionLibary
{
        CollisionComponent::FPlane CalculatePlane(DirectX::XMVECTOR a, DirectX::XMVECTOR b, DirectX::XMVECTOR c);
    public:
        void CalculateFrustum(CollisionComponent::Frustum& frustum);
        CollisionComponent::FCollideResult OverlapSphereToSphere(CollisionComponent::FSphere& a, CollisionComponent::FSphere& b); 
        CollisionComponent::FCollideResult SweepSphereToSphere(CollisionComponent::FSphere& startA,
                                                               CollisionComponent::FSphere& endA, CollisionComponent::FSphere& checkB); 
		CollisionComponent::FCollideResult SphereToAabb(CollisionComponent::FSphere& sphere, CollisionComponent::FAabb& aabb);
       
};