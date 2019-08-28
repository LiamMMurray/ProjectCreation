#include "PlayerGroundState.h"
#include <cmath>
#include <iostream>
#include "../../UI/UIManager.h"
#include "..//..//Rendering/Components/CameraComponent.h"
#include "..//..//Rendering/Components/DirectionalLightComponent.h"
#include "..//GEngine.h"
#include "..//GenericComponents/TransformComponent.h"
#include "..//MathLibrary/MathLibrary.h"
#include "../CoreInput/CoreInput.h"
#include "PlayerControllerStateMachine.h"
#include "PlayerMovement.h"

#include "..//Gameplay/OrbitSystem.h"

#define _USE_MATH_DEFINES

using namespace DirectX;

void PlayerGroundState::Enter()
{
        TransformComponent* playerTransformComponent =
            _playerController->GetControlledEntity().GetComponent<TransformComponent>();

        _playerController->SetEulerAngles(playerTransformComponent->transform.rotation.ToEulerAngles());

        _playerController->RequestCurrentLevel();

		volcanoPos = XMVectorSet(24.51f, 0.0f, -139.36f, 1.0f);
        
        // Sets the gravity vector for the player
        //_playerController->SetPlayerGravity(XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));
}

void PlayerGroundState::Update(float deltaTime)
{
        OrbitSystem* orbitSystem = GET_SYSTEM(OrbitSystem);
        // Dev Cheat to test Reveal
        if (GCoreInput::GetKeyState(KeyCode::N) == KeyState::DownFirst)
        {
                orbitSystem->goalsCollected++;
        }
        // Check if the player has collected the three planets : Changed for play testing
        if (orbitSystem->goalsCollected == 1)
        {

                // Changed for play testing
                // _playerController->RequestNextLevel();

                // Reveal Ocean once first planet placed
                GEngine::Get()->GetLevelStateManager()->RequestState(E_LevelStateEvents::LEVEL_01_TO_LEVEL_02);
        }

        if (orbitSystem->goalsCollected == 2)
        {

                // Changed for play testing
                // _playerController->RequestNextLevel();

                // Reveal Ocean once first planet placed
                GEngine::Get()->GetLevelStateManager()->RequestState(E_LevelStateEvents::LEVEL_02_TO_LEVEL_03);
        }

        static bool doOnce = false;
        if (!doOnce && orbitSystem->goalsCollected == 3)
        {
                GEngine::Get()->GetLevelStateManager()->RequestState(E_LevelStateEvents::LEVEL_03_TO_LEVEL_04);
                // UIManager::instance->DemoEnd();
                // GEngine::Get()->m_TargetInstanceReveal = 1.0f;
                // doOnce                                 = true;
        }

        XMVECTOR currentVelocity = _playerController->GetCurrentVelocity();

        currentVelocity += _playerController->GetJumpForce();
        // currentVelocity += _playerController->GetPlayerGravity();


        // Get Delta Time
        float totalTime = (float)GEngine::Get()->GetTotalTime();

        XMFLOAT3 eulerAngles = _playerController->GetEulerAngles();

        float Sensitivity = _playerController->GetSensitivity();

        float           angularSpeed = XMConvertToRadians(Sensitivity) * deltaTime;
        constexpr float pitchLimit   = XMConvertToRadians(90.0f);
        constexpr float rollLimit    = 20.0f;


        float pitchDelta = eulerAngles.x;
        float yawDelta   = eulerAngles.y;
        // Controller Is Connected
        if (GamePad::Get()->CheckConnection() == true)
        {
                eulerAngles.x += -GamePad::Get()->leftStickY * angularSpeed;
                pitchDelta = eulerAngles.x - pitchDelta;

                eulerAngles.y += GamePad::Get()->leftStickX * angularSpeed;
                yawDelta = eulerAngles.y - yawDelta;

                eulerAngles.z += GamePad::Get()->leftStickX * angularSpeed;

                eulerAngles.x = MathLibrary::clamp(eulerAngles.x, -pitchLimit, pitchLimit);

                // std::cout << eulerAngles.x << std::endl;
        }

        // Controller Isn't Connected
        else
        {
                // float pitchDelta = eulerAngles.x;
                eulerAngles.x += GCoreInput::GetMouseY() * (angularSpeed * 0.75f);
                pitchDelta = eulerAngles.x - pitchDelta;

                // float yawDelta = eulerAngles.y;
                eulerAngles.y += GCoreInput::GetMouseX() * angularSpeed;
                yawDelta = eulerAngles.y - yawDelta;

                eulerAngles.z += GCoreInput::GetMouseX() * angularSpeed;

                eulerAngles.x = MathLibrary::clamp(eulerAngles.x, -pitchLimit, pitchLimit);
        }
        // Convert to degrees due to precision errors using small radian values
        float rollDegrees = XMConvertToDegrees(eulerAngles.z);
        rollDegrees       = MathLibrary::clamp(rollDegrees, -rollLimit, rollLimit);
        rollDegrees       = MathLibrary::lerp(rollDegrees, 0.0f, MathLibrary::clamp(deltaTime * rollLimit, 0.0f, 1.0f));

        eulerAngles.z          = XMConvertToRadians(rollDegrees);
        static float shakeTime = 5.0f;
		
        // Player Approached Volcano

        float distance = MathLibrary::CalulateDistanceIgnoreY(_cachedTransform.translation, volcanoPos);

        if (distance <= 50 && startedShaking == false)
        {
                ShouldShake = true;
        }

        else if (distance > 50.0f && startedShaking == true)
        {
                ShouldShake = false;
        }

        ScreenShake(shakeTime, 0.7f, eulerAngles.x, eulerAngles.z);

        _cachedTransform.rotation = FQuaternion::FromEulerAngles(eulerAngles);

        currentVelocity = XMVector3Rotate(currentVelocity, XMQuaternionRotationAxis(VectorConstants::Up, yawDelta));

        currentVelocity = XMVector3Rotate(currentVelocity, XMQuaternionRotationAxis(VectorConstants::Right, pitchDelta));

        // Get the Speed from the gathered input
        XMVECTOR currentInput = _playerController->GetCurrentInput();
        currentInput          = XMVector3Rotate(currentInput, _cachedTransform.rotation.data);
        {
                XMVECTOR dir = VectorConstants::Up;

                if (bUseGravity == false)
                {
                        float delta =
                            MathLibrary::CalulateDistance(_playerController->GetNextForward(), _cachedTransform.GetRight());


                        if (delta > 0.001f)
                        {
                                dir = XMVector3Normalize(
                                    XMVector3Cross(_playerController->GetNextForward(), _cachedTransform.GetRight()));
                        }
                        else
                        {
                                dir = XMVector3Normalize(
                                    XMVector3Cross(_playerController->GetNextForward(), _cachedTransform.GetForward()));
                        }
                }

                float dot = MathLibrary::VectorDotProduct(currentInput, dir);
                currentInput -= dir * dot;
                currentInput = XMVector3Normalize(currentInput);
        }
        // Normalize the gathered input to determine the desired direction
        XMVECTOR desiredDir = XMVector3Normalize(currentInput);

        // Determine the max speed the object can move
        float maxSpeed = _playerController->GetCurrentMaxSpeed();

        XMVECTOR desiredVelocity = maxSpeed * desiredDir;
        // Calculate desiredVelocity by multiplying the currSpeed by the direction we want to go
        // Determine if we should speed up or slow down
        float accel = (XMVectorGetX(XMVector3Length(desiredVelocity)) >= XMVectorGetX(XMVector3Length(currentVelocity))) ?
                          _playerController->GetAcceleration() :
                          _playerController->GetDeacceleration();

        // Calculate distance from our current velocity to our desired velocity
        float dist = MathLibrary::CalulateDistance(currentVelocity, desiredVelocity);

        // Calculate change based on the type of acceleration, the change in time, and the calculated distance
        float delta = std::min(accel * deltaTime, dist);

        // Normalize the difference of the desired velocity and the current velocity
        XMVECTOR deltaVec = XMVector3Normalize(desiredVelocity - currentVelocity);
        currentVelocity   = currentVelocity + deltaVec * delta + m_ExtraYSpeed * VectorConstants::Up;

        XMVECTOR actualVelocity = currentVelocity;
        if (bUseGravity)
        {
                float currY   = XMVectorGetY(_cachedTransform.translation);
                float sign    = MathLibrary::GetSign(currY);
                float gravity = -sign * 200.0f * deltaTime;
                // Calculate current velocity based on itself, the deltaVector, and delta
                actualVelocity += gravity * VectorConstants::Up;
        }

        if (m_SpeedboostTimer > 0.0f)
        {
                m_SpeedboostTimer -= deltaTime;
        }
        else
        {
                _playerController->SetCurrentMaxSpeed(MathLibrary::MoveTowards(
                    maxSpeed, _playerController->GetMinMaxSpeed(), m_SpeedboostTransitionSpeed * deltaTime));

                m_ExtraYSpeed = 0.0f;
        }


        // Process offset
        {
                XMVECTOR offset = actualVelocity * deltaTime;
                _cachedTransform.translation += offset;
                float posY                   = XMVectorGetY(_cachedTransform.translation);
                posY                         = std::max(posY, 0.0f);
                _cachedTransform.translation = XMVectorSetY(_cachedTransform.translation, posY);
        }

        _playerController->GetControlledEntity().GetComponent<TransformComponent>()->transform = _cachedTransform;

        auto sunComp                        = GEngine::Get()->m_SunHandle.GetComponent<DirectionalLightComponent>();
        auto sunTransComp                   = GEngine::Get()->m_SunHandle.GetComponent<TransformComponent>();
        sunTransComp->transform.translation = _cachedTransform.translation;
        sunComp->m_LightRotation =
            sunComp->m_LightRotation *
            XMQuaternionRotationAxis(VectorConstants::Up,
                                     deltaTime * 0.04f * MathLibrary::CalulateVectorLength(XMVectorSetY(actualVelocity, 0.0f)));
        // sunEuler.y    = MathLibrary::MoveTowardsAngle(sunEuler.y, MathLibrary::NormalizeAngle(eulerAngles.y + XM_PI), 1.0f *
        // deltaTime);

        _playerController->SetCurrentVelocity(currentVelocity);
        _playerController->SetEulerAngles(MathLibrary::NormalizeEulerAngles(eulerAngles));

}

void PlayerGroundState::Exit()
{
        _playerController->SetCurrentVelocity(XMVectorZero());
}

void PlayerGroundState::ScreenShake(float& shakeTime, float magnitude, float& pitchDelta, float& rollDelta)
{
        if (ShouldShake == true)
        {
                if (shakeTime <= 0.0f)
                {
                        startedShaking = true;
                        ShouldShake = false;
                        shakeTime   = 5.0f;
                        return;
                }

                float rollShake  = (MathLibrary::RandomFloatInRange(-2.0f, 2.0f) * std::min(shakeTime, magnitude));
                float pitchShake = (MathLibrary::RandomFloatInRange(-1.0f, 1.0f) * std::min(shakeTime, magnitude));
                shakeTime -= (GEngine::Get()->GetDeltaTime() * 1.5f);

                // rollDelta += XMConvertToRadians(rollShake);

                rollDelta  = MathLibrary::LerpAngle(rollDelta, rollShake, GEngine::Get()->GetDeltaTime());
                pitchDelta = MathLibrary::LerpAngle(pitchDelta, pitchShake, GEngine::Get()->GetDeltaTime());
        }
        return;
}
