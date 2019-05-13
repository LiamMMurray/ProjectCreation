#pragma once
#include"CollisionComponent.h"
class CollisionLibary
{
        CollisionComponent::FPlane CalculatePlane(DirectX::XMVECTOR a, DirectX::XMVECTOR b, DirectX::XMVECTOR c);
        CollisionComponent::FCotactPoint   CollidePoint(DirectX::XMVECTOR pointA, DirectX::XMVECTOR pointB, float radius);
    public:
        CollisionComponent::FCollideResult OverlapSphereToSphere(CollisionComponent::FSphere& a, CollisionComponent::FSphere& b); 
        CollisionComponent::FCollideResult SweepSphereToSphere(CollisionComponent::FSphere& startA,
                                                               CollisionComponent::FSphere& endA, CollisionComponent::FSphere& checkB); 
		CollisionComponent::FCollideResult SphereToAabb(CollisionComponent::FSphere& sphere, CollisionComponent::FAabb& aabb);
        CollisionComponent::FCollideResult AabbToAabb(CollisionComponent::FAabb& a, CollisionComponent::FAabb& b);
       
};