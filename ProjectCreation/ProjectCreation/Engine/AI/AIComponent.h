#pragma once

#include <DirectXMath.h>
#include "../../ECS/Component.h"

using namespace DirectX;
class AIComponent : public IComponent
{
    public:
        XMVECTOR boidPosition           = XMVectorZero();
        XMVECTOR boidVelocity           = XMVectorZero();
        XMVECTOR boidMaxVelocity        = XMVectorZero();
        float    boidSafeRadius         = 0.0f;
};