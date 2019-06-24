#pragma once

#include <DirectXMath.h>
#include "../../ECS/Component.h"

using namespace DirectX;
class AIComponent : public Component<AIComponent>
{
    public:
        XMVECTOR m_Velocity    = XMVectorZero();
        XMVECTOR m_MaxVelocity = XMVectorZero();
        float    m_SafeRadius  = 0.0f;
};