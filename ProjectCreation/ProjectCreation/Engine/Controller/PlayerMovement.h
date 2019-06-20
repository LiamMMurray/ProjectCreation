#pragma once
#include "../../ECS/ECSTypes.h"
#include "IController.h"

#include "../ConsoleWindow/ConsoleWindow.h"
#include "../MathLibrary/MathLibrary.h"
#include "../Physics/PhysicsComponent.h"
#include "PlayerControllerStateMachine.h"

// Audio Includes
#include <Interface/G_Audio/GMusic.h>
#include <Interface/G_Audio/GSound.h>
#include "../Audio/AudioManager.h"

class TransformComponent;
class PlayerCinematicState;
class PlayerGroundState;
class PlayerController : public IController
{
        friend class PlayerControllerStateMachine;

    private:
        ComponentHandle     m_GoalComponent;
        void                GatherInput() override;
        void                ProcessInput() override;
        void                ApplyInput() override;
        TransformComponent* _cachedControlledTransformComponent;

        DirectX::XMFLOAT3 m_EulerAngles;

        float minMaxSpeed     = 1.0f;
        float maxMaxSpeed     = 3.0f;
        float currentMaxSpeed = minMaxSpeed;

        float acceleration   = 1.0;
        float deacceleration = 8.0f;

        DirectX::XMVECTOR m_CurrentInput;
        DirectX::XMVECTOR m_CurrentVelocity;
        DirectX::XMVECTOR m_CurrentForward;

        PlayerControllerStateMachine m_StateMachine;

        PlayerCinematicState* m_CinematicState;
        PlayerGroundState*    m_GroundState;

        // Boolean values for the light collection keys
        // Names will most likely change later on

		static constexpr unsigned int MAX_SPEEDBOOST_SOUNDS = 10;

		void DebugPrintSpeedBoostColor(int color);
		float rhythmThreshold = 0.5f;
        double spaceTimeStamp = 0.0f;
    public:
        GW::AUDIO::GSound* mSpeedBoostSoundPool[MAX_SPEEDBOOST_SOUNDS];
        unsigned int       currSpeedBoostIteration = 0;
        virtual void       Shutdown() override;

        virtual void Init(EntityHandle h) override;
        void         SpeedBoost(DirectX::XMVECTOR boostPos, int color, double collisionTimeStamp);

        inline void SetCurrentMaxSpeed(float val)
        {
                currentMaxSpeed = val;
        };

        inline float GetCurrentMaxSpeed() const
        {
                return currentMaxSpeed;
        }

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

        void AddCurrentVelocity(DirectX::XMVECTOR val);

        inline void SetCurrentVelocity(DirectX::XMVECTOR val)
        {
                m_CurrentVelocity = val;
        }

        inline DirectX::XMVECTOR GetCurrentVelocity() const
        {
                return m_CurrentVelocity;
        }

        inline void SetCurrentForward(DirectX::XMVECTOR val)
        {
                m_CurrentVelocity = val;
        }

        inline DirectX::XMVECTOR GetCurrentForward() const
        {
                return m_CurrentForward;
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

        void SetUseGravity(bool val);
        bool GetUseGravity();
        void SetYExtraSpeed(float val);

        void RequestCinematicTransition(int                    count,
                                        const ComponentHandle* handles,
                                        const FTransform*      targets,
                                        int                    targetState,
                                        float                  duration,
                                        float                  delay = 0.0f);
        void RequestCinematicTransitionLookAt(ComponentHandle        lookAtTarget,
                                              int                    count,
                                              const ComponentHandle* handles,
                                              const FTransform*      targets,
                                              int                    targetState,
                                              float                  duration,
                                              float                  lookAtTransitionDuration = 1.0f,
                                              float                  delay                    = 0.0f);
        void RequestPuzzleMode(ComponentHandle          goalHandle,
                               const DirectX::XMVECTOR& puzzleCenter,
                               bool                     alignToGoal          = false,
                               float                    transitionDuration   = 1.0f,
                               float                    lookAtDuration       = 1.0f,
                               int                      otherTransformsCount = 0,
                               const ComponentHandle*   handles              = nullptr,
                               const FTransform*        transforms           = nullptr);
};