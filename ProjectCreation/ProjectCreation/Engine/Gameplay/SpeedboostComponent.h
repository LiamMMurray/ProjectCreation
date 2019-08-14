#pragma once
#include "../../ECS/Component.h"

#include <DirectXMath.h>
#include "LightOrbColors.h"

class SpeedboostComponent : public Component<SpeedboostComponent>
{
    public:
        int               color;
        float             collisionRadius;
        float             lifetime;
        float             decay;

		bool hasParticle = true;
};