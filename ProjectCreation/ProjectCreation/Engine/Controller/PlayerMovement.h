#pragma once
#include "../../ECS/ECSTypes.h"
#include "IController.h"

#include "../Physics/PhysicsComponent.h"

enum class EPlayerState
{
        ON_GROUND = 0,
        ON_AIR,
        ON_WATER,
        ON_PUZZLE,
        COUNT,
};

enum class EPuzzleState
{
        EASE_IN = 0,
        SOLVE,
        EASE_OUT,
        COUNT,
};

class TransformComponent;
class PlayerController : public IController
{
    private:
        ComponentHandle m_GoalComponent;
        void            GatherInput() override;
        void            ProcessInput() override;
        void            ApplyInput() override;

        void UpdateOnGround();
        void UpdateOnPuzzle();
        void UpdateOnPuzzleEaseIn();
        void UpdateOnPuzzleEaseOut();
        void UpdateOnPuzzleSolve();

        DirectX::XMVECTOR m_CurrentPosition;

        struct FDebugLine
        {
                DirectX::XMVECTOR start;
                DirectX::XMVECTOR end;
        };

        EPlayerState m_CurrentPlayerState = EPlayerState::ON_GROUND;
        EPuzzleState m_CurrentPuzzleState = EPuzzleState::SOLVE;

		TransformComponent* _cachedControlledTransformComponent;
		DirectX::XMFLOAT3 m_EulerAngles;
    public:
        PlayerController();

		virtual void Init(EntityHandle h) override;
        void SpeedBoost(DirectX::XMVECTOR preBoostVelocity);
		

        float minMaxSpeed = 1.0f;
        float maxMaxSpeed = 3.0f;

        float acceleration   = 1.0;
        float deacceleration = 1.5f;

        float m_TotalTime = 0.0f;


        int32_t m_MouseXDelta;
        int32_t m_MouseYDelta;

        DirectX::XMVECTOR m_CurrentInput;

        DirectX::XMVECTOR m_CurrentVelocity;

        inline void SetGoalComponent(ComponentHandle val)
        {
                m_GoalComponent = val;
        }

        inline ComponentHandle GetGoalComponent() const
        {
                return m_GoalComponent;
        }

        inline EPlayerState GetPlayerState() const
        {
                return m_CurrentPlayerState;
        };

        inline void SetPlayerState(EPlayerState val)
        {
                m_CurrentPlayerState = val;
        };
};