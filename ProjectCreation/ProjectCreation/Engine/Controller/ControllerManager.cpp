#pragma once
#include "ControllerManager.h"

#include <iostream>
#include "../Controller/IController.h"
#include "../CoreInput/CoreInput.h"
#include "DebugCameraController.h"
#include "PlayerMovement.h"

#include "../../Rendering/DebugRender/debug_renderer.h"
#include "../../Rendering/RenderingSystem.h"

#include "../CollisionLibary/Shapes.h"

#include "../../UI/UIManager.h"

using namespace std;

void ControllerManager::DisplayConsoleMenu()
{
        if (m_CurrentController == E_CONTROLLERS::DEBUG)
        {
                cout << "Switching Active Controller To Player Camera" << endl;
        }

        if (m_CurrentController == E_CONTROLLERS::PLAYER)
        {
                cout << "Switching Active Controller To Debug Camera" << endl;
        }
}

void ControllerManager::Initialize()
{
        m_SystemManager    = GEngine::Get()->GetSystemManager();
        m_ComponentManager = GEngine::Get()->GetComponentManager();
        m_EntityManager    = GEngine::Get()->GetEntityManager();

        m_Controllers[E_CONTROLLERS::PLAYER] = new PlayerController;
        m_Controllers[E_CONTROLLERS::DEBUG]  = new DebugCameraController;


        // Player entity setup
        {
                auto eHandle = m_EntityManager->CreateEntity<BaseEntity>();
                m_Controllers[E_CONTROLLERS::PLAYER]->Init(eHandle);


                GEngine::Get()->GetComponentManager()->AddComponent<TransformComponent>(eHandle);
                GEngine::Get()->GetComponentManager()->AddComponent<CameraComponent>(eHandle);

                auto tComp                   = GEngine::Get()->GetComponentManager()->GetComponent<TransformComponent>(eHandle);
                tComp->transform.translation = DirectX::XMVectorSet(0.0f, 0.3f, -2.0f, 1.0f);

                auto cameraComp = GEngine::Get()->GetComponentManager()->GetComponent<CameraComponent>(eHandle);
                cameraComp->m_Settings.m_HorizontalFOV = 90.0f;
        }

        {
                auto eHandle = m_EntityManager->CreateEntity<BaseEntity>();
                m_Controllers[E_CONTROLLERS::DEBUG]->Init(eHandle);


                GEngine::Get()->GetComponentManager()->AddComponent<TransformComponent>(eHandle);
                GEngine::Get()->GetComponentManager()->AddComponent<CameraComponent>(eHandle);

                auto tComp                   = GEngine::Get()->GetComponentManager()->GetComponent<TransformComponent>(eHandle);
                tComp->transform.translation = DirectX::XMVectorSet(0.0f, 0.3f, -2.0f, 1.0f);

                auto cameraComp = GEngine::Get()->GetComponentManager()->GetComponent<CameraComponent>(eHandle);
                cameraComp->m_Settings.m_HorizontalFOV = 90.0f;
        }

        m_CurrentController = E_CONTROLLERS::PLAYER;
}

void ControllerManager::Update(float delta)
{
        if (GCoreInput::GetKeyState(KeyCode::Esc) == KeyState::DownFirst)
        {
                m_togglePauseInput = !m_togglePauseInput;
        }

        if (GCoreInput::GetKeyState(KeyCode::Tab) == KeyState::DownFirst)
        {
                m_toggleDebug = !m_toggleDebug;
                DisplayConsoleMenu();
                GEngine::Get()->SetDebugMode(m_toggleDebug);

                if (m_toggleDebug == true)
                {
                        m_CurrentController = E_CONTROLLERS::DEBUG;

                        auto tPlayer = m_ComponentManager->GetComponent<TransformComponent>(
                            m_Controllers[E_CONTROLLERS::PLAYER]->GetControlledEntity());
                        auto tDebug = m_ComponentManager->GetComponent<TransformComponent>(
                            m_Controllers[E_CONTROLLERS::DEBUG]->GetControlledEntity());

                        tDebug->transform = tPlayer->transform;
                }

                if (m_toggleDebug == false)
                {
                        m_CurrentController = E_CONTROLLERS::PLAYER;
                }
        }


        if (m_CurrentController == E_CONTROLLERS::PLAYER)
        {
                auto comp = m_ComponentManager->GetComponent<CameraComponent>(
                    m_Controllers[E_CONTROLLERS::PLAYER]->GetControlledEntity());

                m_SystemManager->GetSystem<RenderSystem>()->SetMainCameraComponent(comp->GetHandle());
        }

        if (m_CurrentController == E_CONTROLLERS::DEBUG)
        {
                auto comp = m_ComponentManager->GetComponent<CameraComponent>(
                    m_Controllers[E_CONTROLLERS::DEBUG]->GetControlledEntity());

                m_SystemManager->GetSystem<RenderSystem>()->SetMainCameraComponent(comp->GetHandle());

                m_Controllers[E_CONTROLLERS::PLAYER]->InactiveUpdate(delta);
        }

        if (m_togglePauseInput == false)
        {
                m_Controllers[m_CurrentController]->OnUpdate(delta);
        }

        else if (m_togglePauseInput == true)
        {
                m_Controllers[m_CurrentController]->PauseInput();
		}
}

void ControllerManager::Shutdown()
{
        for (int i = 0; i < E_CONTROLLERS::COUNT; ++i)
        {
                if (m_Controllers[i])
                        delete m_Controllers[i];
        }
}