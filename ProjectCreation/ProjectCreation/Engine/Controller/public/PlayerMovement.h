#pragma once
#include <ECSTypes.h>
#include "IController.h"

#include "../../ConsoleWindow/ConsoleWindow.h"
#include <MathLibrary.h>
#include <PhysicsComponent.h>
#include "PlayerControllerStateMachine.h"

#include "../..//CoreInput/CoreInput.h"
#include "../..//Gameplay/LightOrbColors.h"
// Audio Includes
#include <Interface/G_Audio/GMusic.h>
#include <Interface/G_Audio/GSound.h>
#include "../../Audio/AudioManager.h"
#include <JGamePad.h>

class TransformComponent;
class PlayerCinematicState;
class PlayerGroundState;
class PlayerController : public IController
{
        friend class PlayerControllerStateMachine;

    private:
        ComponentHandle m_GoalComponent;
        void            GatherInput() override;
        void            ProcessInput() override;
        void            ApplyInput() override;
        FTransform      _cachedControlledTransform;

        DirectX::XMFLOAT3 m_EulerAngles;

        float minMaxSpeed     = 2.5f;
        float maxMaxSpeed     = 4.0f;
        float currentMaxSpeed = minMaxSpeed;

        float acceleration   = 1.0;
        float deacceleration = 8.0f;

        DirectX::XMVECTOR m_CurrentInput;
        DirectX::XMVECTOR m_CurrentVelocity;
        DirectX::XMVECTOR m_CurrentForward;

        PlayerControllerStateMachine m_StateMachine;

        PlayerCinematicState* m_CinematicState;
        PlayerGroundState*    m_GroundState;

        DirectX::XMVECTOR m_JumpForce;
        DirectX::XMVECTOR m_PlayerGravity;

        // Boolean values for the light collection keys
        // Names will most likely change later on

        static constexpr unsigned int MAX_SPEEDBOOST_SOUNDS = 10;

        void DebugPrintSpeedBoostColor(int color);

        DirectX::XMVECTOR mNextForward;

        int m_CollectedSplineOrbCount;

    public:
        int m_TotalSplineOrbCount;

        float m_MouseSensitivity[3]      = {1.0f, 5.0f, 10.0f};
        float m_ControllerSensitivity[3] = {25.0f, 75.0f, 125.0f};

        void IncreaseCollectedSplineOrbCount(int color);

        inline void ResetCollectedSplineOrbCount()
        {
                m_CollectedSplineOrbCount = 0;
        }

        float m_Sensitivity = 0;

        inline void SetSensitivity(int var)
        {
                if (JGamePad::Get()->CheckConnection() == true)
                {
                        m_Sensitivity = m_ControllerSensitivity[var];
                }
                else
                {
                        m_Sensitivity = m_MouseSensitivity[var];
                }
        }

        inline float GetSensitivity()
        {
                return m_Sensitivity;
        }

        inline void SetNextForward(const DirectX::XMVECTOR& _val)
        {
                mNextForward = _val;
        }

        inline const DirectX::XMVECTOR& GetNextForward() const
        {
                return mNextForward;
        }

        virtual void Shutdown() override;

        KeyCode m_ColorInputKeyCodes[E_LIGHT_ORBS::COUNT]            = {KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::Any};
        DWORD   m_ColorInputGameControllerCodes[E_LIGHT_ORBS::COUNT] = {XINPUT_GAMEPAD_B, XINPUT_GAMEPAD_A, XINPUT_GAMEPAD_X};

        const std::string m_SpeedboostSoundNames[3] = {"WHITE_ORB_COLLECT_0_0", "WHITE_ORB_COLLECT_0_1", "WHITE_ORB_COLLECT_0_2"};

        GW::AUDIO::GSound* m_SpeedBoostSoundPool[E_LIGHT_ORBS::COUNT][MAX_SPEEDBOOST_SOUNDS];
        unsigned int       m_SpeedBoostPoolCounter[E_LIGHT_ORBS::COUNT] = {};

        virtual void Init(EntityHandle h) override;
        virtual void Reset() override;
        bool         SpeedBoost(DirectX::XMVECTOR boostPos, int color);


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

        inline void SetJumpForce(DirectX::XMVECTOR val)
        {
                m_JumpForce = val;
        }

        inline DirectX::XMVECTOR GetJumpForce() const
        {
                return m_JumpForce;
        }

        inline void SetPlayerGravity(DirectX::XMVECTOR val)
        {
                m_PlayerGravity = val;
        }

        inline DirectX::XMVECTOR GetPlayerGravity() const
        {
                return m_PlayerGravity;
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

        void RequestCurrentLevel();
        void RequestNextLevel();

        void RequestPuzzleMode(ComponentHandle          goalHandle,
                               const DirectX::XMVECTOR& puzzleCenter,
                               bool                     alignToGoal          = false,
                               float                    transitionDuration   = 1.0f,
                               float                    lookAtDuration       = 1.0f,
                               int                      otherTransformsCount = 0,
                               const ComponentHandle*   handles              = nullptr,
                               const FTransform*        transforms           = nullptr);
};