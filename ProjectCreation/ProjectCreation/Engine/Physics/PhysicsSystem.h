#pragma once
#include "../../ECS/ISystem.h"
#include <DirectXMath.h>

class PhysicsSystem : public ISystem
{
    private:
		//m_ZeroG is 0.0 times earth's gravity
		//m_HalfG is 0.5 times earth's gravity
		//m_OneG is 1.0 times earth's gravity
		//m_TwoG is 2.0 times earth's gravity
		//All Gravity is measured in Meters per Second squared (m/s^2)

        DirectX::XMVECTOR m_ZeroG = DirectX::XMVectorSet(0, 0, 0, 0); 
		DirectX::XMVECTOR m_HalfG = DirectX::XMVectorSet(0, -4.9f, 0, 0);
        DirectX::XMVECTOR m_OneG  = DirectX::XMVectorSet(0, -9.8f, 0, 0); 
		DirectX::XMVECTOR m_TwoG = DirectX::XMVectorSet(0, -19.6f, 0, 0);

        // m_Gravity is the active gravity
        DirectX::XMVECTOR m_Gravity = m_ZeroG;

    public:
        void OnPreUpdate(float deltaTime) override;
        void OnUpdate(float deltaTime) override;
        void OnPostUpdate(float deltaTime) override;
        void OnInitialize() override;
        void OnShutdown() override;
        void OnResume() override;
        void OnSuspend() override;
};
