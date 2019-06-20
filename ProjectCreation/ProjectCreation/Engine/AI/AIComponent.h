#pragma once

#include <DirectXMath.h>
#include "../../ECS/Component.h"

using namespace DirectX;
class AIComponent : public IComponent
{
    public:
        XMVECTOR m_Position    = XMVectorZero();
        XMVECTOR m_Velocity    = XMVectorZero();
        XMVECTOR m_MaxVelocity = XMVectorZero();
        float    m_SafeRadius  = 0.0f;

        virtual void Update(float deltaTime)
        {
                if (XMVector3Greater(XMVector3LengthSq(m_Velocity), (m_MaxVelocity * m_MaxVelocity)))
                {
                        m_Velocity = XMVector3Normalize(m_Velocity);
                        m_Velocity *= m_MaxVelocity;
                }
                m_Position += m_Velocity * deltaTime;
        };
};