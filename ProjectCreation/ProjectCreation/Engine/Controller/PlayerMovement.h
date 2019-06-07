#pragma once
#include "../../ECS/ECSTypes.h"
#include "IController.h"

#include "PlayerControllerStateMachine.h"
#include "../MathLibrary/MathLibrary.h"
#include "../Physics/PhysicsComponent.h"

class TransformComponent;
class PlayerCinematicState;
class PlayerController : public IController
{
        friend class PlayerControllerStateMachine;

    private:
        ComponentHandle m_GoalComponent;
        void            GatherInput() override;
        void            ProcessInput() override;
        void            ApplyInput() override;

        TransformComponent* _cachedControlledTransformComponent;

        DirectX::XMFLOAT3 m_EulerAngles;

        float minMaxSpeed = 1.0f;
        float maxMaxSpeed = 3.0f;


        float acceleration   = 1.0;
        float deacceleration = 1.5f;

        DirectX::XMVECTOR m_CurrentInput;
        DirectX::XMVECTOR m_CurrentVelocity;

		PlayerControllerStateMachine m_StateMachine;

		PlayerCinematicState* m_CinematicState;
    public:
        PlayerController();

        virtual void Init(EntityHandle h) override;
        void         SpeedBoost(DirectX::XMVECTOR preBoostVelocity);

        inline void SetMinMaxSpeed(float val)
        {
                minMaxSpeed = val;
        };

        inline float GetMinMaxSpeed() const
        {
                return minMaxSpeed;
        }

        inline void SetMaxMaxSpeed(float val)
        {
                maxMaxSpeed = val;
        };

        inline float GetMaxMaxSpeed() const
        {
                return maxMaxSpeed;
        }

        inline void SetAcceleration(float val)
        {
                acceleration = val;
        };

        inline float GetAcceleration() const
        {
                return acceleration;
        }

        inline void SetDeacceleration(float val)
        {
                deacceleration = val;
        };

        inline float GetDeacceleration() const
        {
                return deacceleration;
        }

        inline DirectX::XMVECTOR GetCurrentInput() const
        {
                return m_CurrentInput;
        }

        inline void SetCurrentInput(DirectX::XMVECTOR val)
        {
                m_CurrentInput = val;
        }

        inline void SetCurrentVelocity(DirectX::XMVECTOR val)
        {
                m_CurrentVelocity = val;
        }

        inline DirectX::XMVECTOR GetCurrentVelocity() const
        {
                return m_CurrentVelocity;
        }

        inline void SetGoalComponent(ComponentHandle val)
        {
                m_GoalComponent = val;
        }

        inline ComponentHandle GetGoalComponent() const
        {
                return m_GoalComponent;
        }

        inline DirectX::XMFLOAT3 GetEulerAngles() const
        {
                return m_EulerAngles;
        }

        inline void SetEulerAngles(DirectX::XMFLOAT3 val)
        {
                m_EulerAngles = val;
        }

		void RequestTransition(const FTransform& target);
};