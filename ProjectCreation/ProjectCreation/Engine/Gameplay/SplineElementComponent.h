#pragma once

#include <DirectXMath.h>
#include "../../ECS/Component.h"

class SpeedboostSplineComponent : public Component<SpeedboostSplineComponent>
{
    public:
        int  index;
        int  clusterID;
        bool bTail = false;
        bool bHead = false;
};