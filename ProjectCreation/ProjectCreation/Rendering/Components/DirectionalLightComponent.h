#pragma once


#include <HandleManager.h>

#include "../../Engine/MathLibrary/MathLibrary.h"

struct DirectionalLightComponent : public Component<DirectionalLightComponent>
{
        FQuaternion       m_LightRotation;
        DirectX::XMFLOAT4 m_LightColor = {1.0f, 1.0f, 1.0f, 1.0f};
        DirectX::XMFLOAT4 m_AmbientColor = {1.0f, 1.0f, 1.0f, 1.0f};
};