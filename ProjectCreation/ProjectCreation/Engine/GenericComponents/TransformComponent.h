#pragma once
#include "../../ECS/Component.h"
#include "../MathLibrary/Transform.h"

class TransformComponent : public Component<TransformComponent>
{
    public:
        FTransform transform;
};