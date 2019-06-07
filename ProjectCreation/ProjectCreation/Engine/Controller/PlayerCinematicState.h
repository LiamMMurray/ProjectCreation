#pragma once

#include "PlayerControllerState.h"
#include "../MathLibrary/MathLibrary.h"

class PlayerCinematicState : public IPlayerControllerState
{
    private:
        float m_currAlpha;
        float m_cinematicAlpha;

        float m_targetAlpha;

		FTransform m_initTransform;
        FTransform m_endTransform;

		float m_Delay    = 2.0f;
		float m_Duration = 5.0f;

    public:
        // Inherited via IPlayerControllerState
        virtual void Enter() override;
        virtual void Update(float deltaTime) override;
        virtual void Exit() override;

		void SetInitTransform(FTransform _transform);
        void SetEndTransform(FTransform _transform);
        void SetTansitionDuration(float _duration);
};