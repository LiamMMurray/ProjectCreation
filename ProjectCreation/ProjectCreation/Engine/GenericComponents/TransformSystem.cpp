#include "TransformSystem.h"
#include "..//..//Rendering/Terrain/TerrainManager.h"
#include "..//Controller/ControllerSystem.h"
#include "..//GEngine.h"
#include "..//MathLibrary/MathLibrary.h"
#include "TransformComponent.h"

using namespace DirectX;
void TransformSystem::OnPreUpdate(float deltaTime)
{}
void TransformSystem::OnUpdate(float deltaTime)
{
        XMVECTOR& playerPos = playerTransform.Get<TransformComponent>()->transform.translation;
        float     scale     = TerrainManager::Get()->GetScale();
        XMVECTOR  min       = XMVectorSet(-0.5f * scale, 0.0f, -0.5f * scale, 0.0f);
        XMVECTOR  max       = -min;


        playerPos = MathLibrary::WrapPosition(playerPos, min, max);
        playerPos = XMVectorMax(playerPos, TerrainManager::Get()->AlignPositionToTerrain(playerPos));


        for (auto& transComp : m_HandleManager->GetActiveComponents<TransformComponent>())
        {

                if (transComp.wrapping == true)
                {
                        transComp.transform.translation =
                            MathLibrary::WrapPosition(transComp.transform.translation, playerPos + min, playerPos + max);
                        transComp.transform.translation =
                            XMVectorMax(transComp.transform.translation,
                                        TerrainManager::Get()->AlignPositionToTerrain(transComp.transform.translation));

                        int x = 5;
                }
        }
}

void TransformSystem::OnPostUpdate(float deltaTime)
{}

void TransformSystem::OnInitialize()
{
        auto PlayerHandle = GEngine::Get()
                                ->GetSystemManager()
                                ->GetSystem<ControllerSystem>()
                                ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER]
                                ->GetControlledEntity();

        playerTransform = PlayerHandle.GetComponentHandle<TransformComponent>();

        m_HandleManager = GEngine::Get()->GetHandleManager();
}

void TransformSystem::OnShutdown()
{}

void TransformSystem::OnResume()
{}

void TransformSystem::OnSuspend()
{}
