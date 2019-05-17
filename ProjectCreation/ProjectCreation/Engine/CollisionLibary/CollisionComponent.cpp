#include "CollisionComponent.h"

using namespace DirectX;

float CollisionComponent::FCapsule::GetCapsuleDistance()
{		
        return XMVectorGetX(XMVector3Dot(FCapsule::startPoint, FCapsule::endPoint));
}
