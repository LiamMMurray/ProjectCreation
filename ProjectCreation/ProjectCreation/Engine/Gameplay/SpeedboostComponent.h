#pragma once
#include "../../ECS/Component.h"

#include <DirectXMath.h>
#include "LightOrbColors.h"

class SpeedboostComponent : public Component<SpeedboostComponent>
{
    public:
        int               color;
        DirectX::XMVECTOR center;
        float             collisionRadius;
        float             lifetime;
        float             decay;
};