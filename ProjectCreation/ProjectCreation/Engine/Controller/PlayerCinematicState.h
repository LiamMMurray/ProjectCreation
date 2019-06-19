#pragma once

#include "../MathLibrary/MathLibrary.h"
#include "../../ECS/HandleManager.h"
#include "PlayerControllerState.h"

enum class E_TRANSITION_MODE
{
        Simple,
        LookAt,
};

class PlayerCinematicState : public IPlayerControllerState
{
    private:
        FQuaternion _playerInitialLookAtRot;

        float m_currAlpha;
        float m_lookAtAlpha;

        std::vector<FTransform>      m_InitTransforms;
        std::vector<FTransform>      m_EndTransforms;
        std::vector<ComponentHandle> m_TransformComponents;

        ComponentHandle m_lookAtTarget;

        float m_Delay                    = 0.0f;
        float m_LookAtTransitionDuration = 1.0f;
        float m_Duration                 = 1.0f;
        int   m_TargetState              = 0;

        E_TRANSITION_MODE m_transitionMode;

    public:
        // Inherited via IPlayerControllerState
        virtual void Enter() override;
        virtual void Update(float deltaTime) override;

        void UpdateSimple(float deltaTime);
        void UpdateLookAt(float deltaTime);

        virtual void Exit() override;

        void        AddTransformTransitions(int transformCounts, const ComponentHandle* handles, const FTransform* ends);
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

		inline void SetLookAtTransitionDuration(float val)
        {
                m_LookAtTransitionDuration = val;
        }
};