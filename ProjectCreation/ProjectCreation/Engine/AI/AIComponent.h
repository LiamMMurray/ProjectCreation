#pragma once

#include <DirectXMath.h>
#include "../../ECS/Component.h"

using namespace DirectX;
class AIComponent : public Component<AIComponent>
{
    public:
        XMVECTOR m_Velocity   = XMVectorZero();
        float    m_MaxSpeed   = 1.0f;
        float    m_SafeRadius = 0.5f;
};