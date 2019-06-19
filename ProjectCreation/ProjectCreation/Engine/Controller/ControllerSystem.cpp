#pragma once
#include "ControllerSystem.h"

#include <iostream>
#include "../Controller/IController.h"
#include "../CoreInput/CoreInput.h"
#include "DebugCameraController.h"
#include "PlayerMovement.h"

#include "../../Rendering/DebugRender/debug_renderer.h"
#include "../../Rendering/RenderingSystem.h"

#include "../CollisionLibary/Shapes.h"

#include "../../UI/UIManager.h"

#include <WinUser.h>

using namespace std;

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

int ControllerSystem::GetOrbCount(E_LIGHT_ORBS color)
{
        if (color == E_LIGHT_ORBS::RED_LIGHTS)
        {
                return m_RedOrbCount;
        }

        if (color == E_LIGHT_ORBS::BLUE_LIGHTS)
        {
                return m_BlueOrbCount;
        }

        if (color == E_LIGHT_ORBS::GREEN_LIGHTS)
        {
                return m_GreenOrbCount;
        }
}

void ControllerSystem::SetOrbCount(E_LIGHT_ORBS color)
{
        if (color == E_LIGHT_ORBS::RED_LIGHTS)
        {
                m_RedOrbCount += 1;
                std::cout << "Red Count: " << GetOrbCount(E_LIGHT_ORBS::RED_LIGHTS) << std::endl;
        }

        if (color == E_LIGHT_ORBS::BLUE_LIGHTS)
        {
                m_BlueOrbCount += 1;
                std::cout << "Blue Count: " << GetOrbCount(E_LIGHT_ORBS::BLUE_LIGHTS) << std::endl;
        }

        if (color == E_LIGHT_ORBS::GREEN_LIGHTS)
        {
                m_GreenOrbCount += 1;
                std::cout << "Green Count: " << GetOrbCount(E_LIGHT_ORBS::GREEN_LIGHTS) << std::endl;
        }
}

void ControllerSystem::OnPreUpdate(float deltaTime)
{}

void ControllerSystem::OnUpdate(float deltaTime)
{
        if (GCoreInput::GetKeyState(KeyCode::R) == KeyState::DownFirst)
        {
                SetOrbCount(E_LIGHT_ORBS::RED_LIGHTS);
                std::cout << "Red Count: " << GetOrbCount(E_LIGHT_ORBS::RED_LIGHTS) << std::endl;
        }

        if (GCoreInput::GetKeyState(KeyCode::B) == KeyState::DownFirst)
        {
                SetOrbCount(E_LIGHT_ORBS::BLUE_LIGHTS);
                std::cout << "Blue Count: " << GetOrbCount(E_LIGHT_ORBS::BLUE_LIGHTS) << std::endl;
        }

        if (GCoreInput::GetKeyState(KeyCode::G) == KeyState::DownFirst)
        {
                SetOrbCount(E_LIGHT_ORBS::GREEN_LIGHTS);
                std::cout << "Green Count: " << GetOrbCount(E_LIGHT_ORBS::GREEN_LIGHTS) << std::endl;
        }

        if (GCoreInput::GetKeyState(KeyCode::Tab) == KeyState::DownFirst)
        {
                m_Controllers[m_CurrentController]->SetEnabled(false);
                m_CurrentController = (E_CONTROLLERS)((m_CurrentController + 1) % E_CONTROLLERS::COUNT);
                m_Controllers[m_CurrentController]->SetEnabled(true);
                HandleManager*  handleManager    = GEngine::Get()->GetHandleManager();
                EntityHandle    controllerHandle = m_Controllers[m_CurrentController]->GetControlledEntity();
                ComponentHandle cameraHandle     = controllerHandle.GetComponentHandle<TransformComponent>();
                RenderSystem*   renderSystem     = GEngine::Get()->GetSystemManager()->GetSystem<RenderSystem>();

                renderSystem->SetMainCameraComponent(cameraHandle);
        }

        for (int i = 0; i < E_CONTROLLERS::COUNT; ++i)
        {
                m_Controllers[i]->OnUpdate(deltaTime);
        }
}

void ControllerSystem::OnPostUpdate(float deltaTime)
{}

void ControllerSystem::OnInitialize()
{
        m_SystemManager = GEngine::Get()->GetSystemManager();
        m_HandleManager = GEngine::Get()->GetHandleManager();

        m_Controllers[E_CONTROLLERS::PLAYER] = new PlayerController;
        m_Controllers[E_CONTROLLERS::DEBUG]  = new DebugCameraController;


        // Player entity setup
        {
                EntityHandle eHandle = m_HandleManager->CreateEntity();

                ComponentHandle tHandle = eHandle.AddComponent<TransformComponent>();
                ComponentHandle cHandle = eHandle.AddComponent<CameraComponent>();

                TransformComponent* tComp    = tHandle.Get<TransformComponent>();
                tComp->transform.translation = DirectX::XMVectorSet(0.0f, 0.0f, -2.0f, 1.0f);
                tComp->transform.rotation =
                    DirectX::XMQuaternionRotationRollPitchYaw(DirectX::XMConvertToRadians(-90.0f), 0.0f, 0.0f);

                CameraComponent* cameraComp            = cHandle.Get<CameraComponent>();
                cameraComp->m_Settings.m_HorizontalFOV = 90.0f;

                GEngine::Get()->GetSystemManager()->GetSystem<RenderSystem>()->SetMainCameraComponent(cHandle);
                m_Controllers[E_CONTROLLERS::PLAYER]->Init(eHandle);
        }

        {
                EntityHandle eHandle = m_HandleManager->CreateEntity();

                ComponentHandle tHandle = eHandle.AddComponent<TransformComponent>();
                ComponentHandle cHandle = eHandle.AddComponent<CameraComponent>();

                TransformComponent* tComp    = tHandle.Get<TransformComponent>();
                tComp->transform.translation = DirectX::XMVectorSet(0.0f, 0.3f, -2.0f, 1.0f);

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
                        delete m_Controllers[i];
        }
}

void ControllerSystem::OnResume()
{}

void ControllerSystem::OnSuspend()
{}