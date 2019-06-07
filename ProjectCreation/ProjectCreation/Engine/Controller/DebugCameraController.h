#pragma once
#include "../../ECS/ECSTypes.h"
#include "IController.h"

#include "../GenericComponents/TransformComponent.h"
#include "../Physics/PhysicsComponent.h"

#include "../CollisionLibary/Shapes.h"

class DebugCameraController : public IController
{
    private:
        void GatherInput() override;
        void ProcessInput() override;
        void ApplyInput() override;

        DirectX::XMFLOAT3 m_EulerAngles;  
    public:

		DebugCameraController();

        int32_t m_MouseXDelta;
        int32_t m_MouseYDelta;

        DirectX::XMVECTOR m_CurrentInput;

        DirectX::XMVECTOR m_CurrentVelocity;
};