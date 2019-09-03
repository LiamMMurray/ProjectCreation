#pragma once
#include <Component.h>
#include <Transform.h>

class TransformComponent : public Component<TransformComponent>
{
    public:
        FTransform transform;

        bool wrapping        = true;
        bool wrappingPartial = false;
        bool alignToTerrain  = true;
};