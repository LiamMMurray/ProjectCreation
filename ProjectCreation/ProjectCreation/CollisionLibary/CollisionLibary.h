#pragma once
#include"CollisionComponent.h"
class CollisionLibary
{
        CollisionComponent::FPlane CalculatePlane(DirectX::XMVECTOR a, DirectX::XMVECTOR b, DirectX::XMVECTOR c);
    public:
        void CalculateFrustum(CollisionComponent::Frustum& frustum);
        CollisionComponent::FCollideResult SphereToSphere(CollisionComponent::FSphere& a, CollisionComponent::FSphere& b); 
		CollisionComponent::FCollideResult SphereToAabb(CollisionComponent::FSphere& sphere, CollisionComponent::FAabb& aabb);
       
};