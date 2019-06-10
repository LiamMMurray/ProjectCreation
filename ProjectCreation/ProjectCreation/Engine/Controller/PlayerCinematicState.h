#pragma once

#include "../MathLibrary/MathLibrary.h"
#include "..//..//ECS/ECSTypes.h"
#include "PlayerControllerState.h"

enum class E_TRANSITION_MODE
{
        Simple,
        LookAt,
};

class PlayerCinematicState : public IPlayerControllerState
{
    private:
        float m_currAlpha;

        float m_targetAlpha;

        FTransform m_initTransform;
        FTransform m_endTransform;

        float m_Delay       = 0.0f;
        float m_Duration    = 1.0f;
        int   m_TargetState = 0;
        ComponentHandle m_lookAtTarget;

        E_TRANSITION_MODE m_transitionMode;

    public:
        // Inherited via IPlayerControllerState
        virtual void Enter() override;
        virtual void Update(float deltaTime) override;

		void UpdateSimple(float deltaTime);
        void UpdateLookAt(float deltaTime);

        virtual void Exit() override;

        void        SetInitTransform(FTransform _transform);
        void        SetEndTransform(FTransform _transform);
        void        SetTansitionDuration(float _duration);
        inline void SetTansitionTargetState(int state)
        {
                m_TargetState = state;
        }

        inline void SetTransitionDelay(float val)
        {
                m_Delay = val;
        }

        inline void SetTransitionMode(E_TRANSITION_MODE val)
        {
                m_transitionMode = val;
        }

		inline void SetLookAtTarget(ComponentHandle val)
        {
                m_lookAtTarget = val;
		}
};