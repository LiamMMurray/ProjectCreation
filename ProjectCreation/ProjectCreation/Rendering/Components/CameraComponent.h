#pragma once
#include <DirectXMath.h>
#include "../../ECS/Component.h"
#include "../../Engine/ResourceManager/IResource.h"

struct FCameraSettings
{
        float m_HorizontalFOV = 90.0f;
        float m_NearClip      = 0.05f;
        float m_FarClip       = 1500.0f;
        float m_AspectRatio   = 1.77777777778f;
};

struct CameraComponent : public Component<CameraComponent>
{
	FCameraSettings m_Settings;
	DirectX::XMMATRIX _cachedViewProjection;
	DirectX::XMMATRIX _cachedProjection;
	DirectX::XMMATRIX _cachedView;
};