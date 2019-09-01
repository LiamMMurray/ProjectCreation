#pragma once

#include <DirectXMath.h>
#include <Component.h>

class SpeedboostSplineComponent : public Component<SpeedboostSplineComponent>
{
    public:
        int  index;
        int  clusterID;
        int  color;
        bool bTail = false;
        bool bHead = false;
};