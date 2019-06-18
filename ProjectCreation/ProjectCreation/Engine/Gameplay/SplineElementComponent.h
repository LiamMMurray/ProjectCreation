#pragma once

#include <DirectXMath.h>
#include "../../ECS/Component.h"

class SpeedboostSplineComponent : public Component<SpeedboostSplineComponent>
{
    public:
        DirectX::XMVECTOR m_NextPos;
        DirectX::XMVECTOR m_CurrPos;
        DirectX::XMVECTOR m_PrevPos;
        bool              bTail = false;
        bool              bHead = false;
};