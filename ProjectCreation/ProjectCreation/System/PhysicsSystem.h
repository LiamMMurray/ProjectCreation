#pragma once
#include "../ECS/ISystem.h"

class PhysicsSystem : public ISystem
{
    private:
		//m_ZeroG is 0.0 times earth's gravity
		//m_HalfG is 0.5 times earth's gravity
		//m_OneG is 1.0 times earth's gravity
		//m_TwoG is 2.0 times earth's gravity
		//All Gravity is measured in Meters per Second squared (m/s^2)

        float m_ZeroG = 0.0f, m_HalfG = -4.9f, m_OneG = -9.8f, m_TwoG = -19.6f;

        // m_Gravity is the active gravity
        float m_Gravity = m_ZeroG;

    public:
        void OnPreUpdate(float deltaTime) override;
        void OnUpdate(float deltaTime) override;
        void OnPostUpdate(float deltaTime) override;
        void OnInitialize() override;
        void OnShutdown() override;
        void OnResume() override;
        void OnSuspend() override;
};
