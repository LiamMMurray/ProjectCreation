#include "CollisionLibary.h"
using namespace DirectX;
CollisionComponent::FCollideResult CollisionLibary::SphereToSphere(CollisionComponent::FSphere& a,
                                                                   CollisionComponent::FSphere& b)
{
        CollisionComponent::FCollideResult output;
        float                              distance    = XMVectorGetX(XMVector3Dot(a.center, b.center));
        float                              totalRadius = a.radius + b.radius;
        if (distance < totalRadius)
        {
                output.collisionType = CollisionComponent::ECollisionType::EOveralap;
        }
        else if (distance > totalRadius)
        {
                output.collisionType = CollisionComponent::ECollisionType::ENoCollision;
        }
		else if (distance == totalRadius)
		{
                output.collisionType = CollisionComponent::ECollisionType::ECollide;
		}


        return output;
}
