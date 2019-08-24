#pragma once
#include "PlayerControllerState.h"

class PlayerGroundState : public IPlayerControllerState
{

    public:
        // Inherited via IPlayerControllerState
        virtual void Enter() override;
        virtual void Update(float deltaTime) override;
        virtual void Exit() override;
        virtual ~PlayerGroundState() = default;

        inline void AddSpeedBoost()
        {
                m_SpeedboostTimer = m_SpeedboostDuration;
        }

        bool  bUseGravity                 = false;
        float m_SpeedboostTimer           = 0.0f;
        float m_SpeedboostDuration        = 3.0f;
        float m_SpeedboostTransitionSpeed = 1.0f;
        float m_ExtraYSpeed               = 0.0f;
        float m_YRestorationSpeed         = 0.1f;
};
