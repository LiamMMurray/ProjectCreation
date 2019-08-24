#pragma once
#include "ControllerSystem.h"

#include <iostream>
#include "../..//Rendering/Terrain/TerrainManager.h"
#include "..//Gameplay/LightOrbColors.h"
#include "../Controller/IController.h"
#include "../CoreInput/CoreInput.h"
#include "DebugCameraController.h"
#include "PlayerMovement.h"

#include "..//Gameplay/OrbitSystem.h"
#include "..//Gameplay/SpeedboostSystem.h"

#include "../../Rendering/DebugRender/debug_renderer.h"
#include "../../Rendering/RenderingSystem.h"

#include "../CollisionLibary/Shapes.h"

#include "../../UI/UIManager.h"

#include <WinUser.h>
#include "../CoreInput/InputActions.h"

using namespace std;

DirectX::XMFLOAT3 ControllerSystem::GetCurrentColorSelection() const
{
        DirectX::XMFLOAT3 output;
        DirectX::XMStoreFloat3(&output, currentColor);
        return output;
}

float ControllerSystem::GetCurrentColorAlpha() const
{
        return currentColorAlpha;
}


void ControllerSystem::DisplayConsoleMenu()
{
        cout << "Switching Active Controller To : ";

        switch (m_CurrentController)
        {
                case E_CONTROLLERS::DEBUG:
                {
                        cout << "DEBUG";
                        break;
                }
                case E_CONTROLLERS::PLAYER:
                {
                        cout << "DEBUG";
                        break;
                }

                default:
                {
                        assert(false && "INVALID PLAYER SELECTED");
                }
        }

        cout << endl;
}

int ControllerSystem::GetOrbCount()
{
        return m_OrbCount;
}

void ControllerSystem::IncreaseOrbCount(int color)
{
        int levelType = GEngine::Get()->GetLevelStateManager()->GetCurrentLevelState()->GetLevelType();

        if (color == E_LIGHT_ORBS::WHITE_LIGHTS && levelType != E_Level_States::TUTORIAL_LEVEL)
                return;

        if (m_PrevOrbColor != color)
        {
                ResetOrbCount();
        }
        m_OrbCount++;

        if (m_OrbCount >= 3)
        {
                ResetOrbCount();
                GET_SYSTEM(SpeedBoostSystem)->RequestPath(color);
        }

        m_PrevOrbColor = color;
}

void ControllerSystem::ResetOrbCount()
{
        m_OrbCount = 0;
}

void ControllerSystem::OnPreUpdate(float deltaTime)
{}

void ControllerSystem::OnUpdate(float deltaTime)
{
        if (GCoreInput::GetKeyState(KeyCode::One) == KeyState::DownFirst)
        {
                IncreaseOrbCount(E_LIGHT_ORBS::RED_LIGHTS);
        }

        if (GCoreInput::GetKeyState(KeyCode::Two) == KeyState::DownFirst)
        {
                IncreaseOrbCount(E_LIGHT_ORBS::BLUE_LIGHTS);
        }

        if (GCoreInput::GetKeyState(KeyCode::Three) == KeyState::DownFirst)
        {
                IncreaseOrbCount(E_LIGHT_ORBS::GREEN_LIGHTS);
        }

        if (GCoreInput::GetKeyState(KeyCode::Zero) == KeyState::DownFirst)
        {
                IncreaseOrbCount(E_LIGHT_ORBS::WHITE_LIGHTS);
        }

        if (IsVibrating == true)
        {
                if (rumbleStrength <= 0)
                {
                        IsVibrating = false;
                }

                else
                {
                        rumbleStrength = MathLibrary::MoveTowards(rumbleStrength, 0, deltaTime * 1.5f);
                        GamePad::Get()->IsVibrating(rumbleStrength);
                }
        }


        if (GCoreInput::GetKeyState(KeyCode::Tab) == KeyState::DownFirst)
        {
                using namespace DirectX;

                m_Controllers[m_CurrentController]->SetEnabled(false);

                IController* prevController = m_Controllers[m_CurrentController];

                m_CurrentController = (E_CONTROLLERS)((m_CurrentController + 1) % E_CONTROLLERS::COUNT);

                IController* currController = m_Controllers[m_CurrentController];
                currController->GetControlledEntity().GetComponent<TransformComponent>()->transform.translation +=
                    prevController->worldOffset;

                m_Controllers[m_CurrentController]->SetEnabled(true);
                HandleManager*  handleManager    = GEngine::Get()->GetHandleManager();
                EntityHandle    controllerHandle = m_Controllers[m_CurrentController]->GetControlledEntity();
                ComponentHandle cameraHandle     = controllerHandle.GetComponentHandle<CameraComponent>();
                RenderSystem*   renderSystem     = GEngine::Get()->GetSystemManager()->GetSystem<RenderSystem>();

                renderSystem->SetMainCameraComponent(cameraHandle);
        }

        int colorsPressed    = 0;
        int lastColorPressed = -1;
        for (int i = 0; i < 3; ++i)
        {
                if (InputActions::CheckAction(i) == KeyState::Down)
                {
                        colorsPressed++;
                        lastColorPressed = i;
                }
        }

        if (colorsPressed == 1)
        {
                desiredColorAlpha = 1.0f;
                desiredColor      = DirectX::PackedVector::XMLoadColor(&E_LIGHT_ORBS::ORB_COLORS[lastColorPressed]);
        }
        else
        {
                desiredColorAlpha = 0.0f;
                desiredColor      = DirectX::PackedVector::XMLoadColor(&E_LIGHT_ORBS::ORB_COLORS[E_LIGHT_ORBS::WHITE_LIGHTS]);
        }

        currentColor      = DirectX::XMVectorLerp(currentColor, desiredColor, deltaTime * 8.0f);
        currentColorAlpha = MathLibrary::lerp(currentColorAlpha, desiredColorAlpha, deltaTime * 2.0f);

        for (int i = 0; i < E_CONTROLLERS::COUNT; ++i)
        {
                m_Controllers[i]->OnUpdate(deltaTime);
        }
}

void ControllerSystem::OnPostUpdate(float deltaTime)
{}

void ControllerSystem::OnInitialize()
{

        IsVibrating    = false;
        rumbleStrength = 0.0f;

        m_SystemManager = GEngine::Get()->GetSystemManager();
        m_HandleManager = GEngine::Get()->GetHandleManager();


        m_Controllers[E_CONTROLLERS::PLAYER] = new PlayerController;
        m_Controllers[E_CONTROLLERS::DEBUG]  = new DebugCameraController;


        // Player entity setup
        {
                EntityHandle eHandle = m_HandleManager->CreateEntity();

                ComponentHandle tHandle = eHandle.AddComponent<TransformComponent>();
                ComponentHandle cHandle = eHandle.AddComponent<CameraComponent>();

                eHandle.AddComponent<TransformComponent>();
                eHandle.AddComponent<CameraComponent>();

                auto tComp                   = eHandle.GetComponent<TransformComponent>();
                tComp->wrapping              = false;
                tComp->transform.translation = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
                tComp->transform.rotation =
                    DirectX::XMQuaternionRotationRollPitchYaw(DirectX::XMConvertToRadians(0.0f), 0.0f, 0.0f);

                CameraComponent* cameraComp            = cHandle.Get<CameraComponent>();
                cameraComp->m_Settings.m_HorizontalFOV = 100.0f;

                GEngine::Get()->GetSystemManager()->GetSystem<RenderSystem>()->SetMainCameraComponent(cHandle);
                m_Controllers[E_CONTROLLERS::PLAYER]->Init(eHandle);
        }

        {
                EntityHandle eHandle = m_HandleManager->CreateEntity();

                ComponentHandle tHandle = eHandle.AddComponent<TransformComponent>();
                ComponentHandle cHandle = eHandle.AddComponent<CameraComponent>();

                auto tComp                             = eHandle.GetComponent<TransformComponent>();
                tComp->transform.translation           = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
                tComp->wrapping                        = false;
                auto cameraComp                        = cHandle.Get<CameraComponent>();
                cameraComp->m_Settings.m_HorizontalFOV = 90.0f;
                m_Controllers[E_CONTROLLERS::DEBUG]->Init(eHandle);
                m_Controllers[E_CONTROLLERS::DEBUG]->SetEnabled(false);
        }

        m_CurrentController = E_CONTROLLERS::PLAYER;
}

void ControllerSystem::OnShutdown()
{
        for (int i = 0; i < E_CONTROLLERS::COUNT; ++i)
        {
                if (m_Controllers[i])
                {
                        m_Controllers[i]->Shutdown();
                        delete m_Controllers[i];
                }
        }
}

void ControllerSystem::OnResume()
{}

void ControllerSystem::OnSuspend()
{}