#pragma once


#include "../../ECS/ECS.h"

#include "../../Engine/MathLibrary/MathLibrary.h"

struct DirectionalLightComponent : public Component<DirectionalLightComponent>
{
        FQuaternion       m_LightRotation;
        DirectX::XMFLOAT4 m_LightColor;
};