#pragma once
#include "CollisionComponent.h"
class CollisionLibary
{
        CollisionComponent::FPlane              CalculatePlane(DirectX::XMVECTOR a, DirectX::XMVECTOR b, DirectX::XMVECTOR c);
        static CollisionComponent::FCotactPoint CollidePoint(DirectX::XMVECTOR pointA, DirectX::XMVECTOR pointB, float radius);

    public:
        static CollisionComponent::FCollideResult  OverlapSphereToSphere(CollisionComponent::FSphere& a,
                                                                         CollisionComponent::FSphere& b,
                                                                         float                        offset);
        static CollisionComponent::FSweepCollision SweepSphereToSphere(CollisionComponent::FSphere& startA,
                                                                       CollisionComponent::FSphere& endA,
                                                                       CollisionComponent::FSphere& checkB,
                                                                       float                        offset);
        static CollisionComponent::FSweepCollision MovingSphereToMovingSphere(CollisionComponent::FSphere& startA,
                                                                              CollisionComponent::FSphere& endA,
                                                                              CollisionComponent::FSphere& startB,
                                                                              CollisionComponent::FSphere  endB,
                                                                              float                        offset);
        static CollisionComponent::FCollideResult  SphereToAabb(CollisionComponent::FSphere& sphere,
                                                                CollisionComponent::FAabb&   aabb,
                                                                float                        offset);
        static CollisionComponent::FCollideResult  AabbToAabb(CollisionComponent::FAabb& a,
                                                              CollisionComponent::FAabb& b,
                                                              float                      offset);
};