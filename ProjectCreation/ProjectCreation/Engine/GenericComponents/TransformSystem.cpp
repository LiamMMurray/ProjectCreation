#include "TransformSystem.h"
#include "..//..//Rendering/Terrain/TerrainManager.h"
#include "..//Controller/ControllerSystem.h"
#include "..//GEngine.h"
#include "..//MathLibrary/MathLibrary.h"
#include "TransformComponent.h"

using namespace DirectX;
void TransformSystem::OnPreUpdate(float deltaTime)
{
        playerTransform =
            controllerSystem->GetCurrentController()->GetControlledEntity().GetComponentHandle<TransformComponent>();
}
void TransformSystem::OnUpdate(float deltaTime)
{

        XMVECTOR& playerPos = playerTransform.Get<TransformComponent>()->transform.translation;
        float     scale     = TerrainManager::Get()->GetScale();
        XMVECTOR  min       = XMVectorSet(-0.5f * scale, 0.0f, -0.5f * scale, 0.0f);
        XMVECTOR  max       = -min;


        XMVECTOR newPlayerPos = MathLibrary::WrapPosition(playerPos, min, max);
        XMVECTOR delta        = newPlayerPos - playerPos;
        GEngine::Get()->m_OriginOffset += delta;
        GEngine::Get()->m_WorldOffsetDelta = delta;
        playerPos                          = newPlayerPos;
        if (controllerSystem->GetCurrentControllerIndex() == 0)
        {
                playerPos = XMVectorMax(playerPos, TerrainManager::Get()->AlignPositionToTerrain(playerPos));
        }

        float deltaLength = MathLibrary::CalulateVectorLength(delta);
        for (auto& transComp : m_HandleManager->GetActiveComponents<TransformComponent>())
        {

                if (transComp.wrapping == true)
                {
                        if (deltaLength > 0.01f)
                                transComp.transform.translation += delta;
                        transComp.transform.translation =
                            XMVectorMax(transComp.transform.translation,
                                        TerrainManager::Get()->AlignPositionToTerrain(transComp.transform.translation));
                }
        }
}

void TransformSystem::OnPostUpdate(float deltaTime)
{}

void TransformSystem::OnInitialize()
{
        m_HandleManager = GEngine::Get()->GetHandleManager();

        controllerSystem = GEngine::Get()->GetSystemManager()->GetSystem<ControllerSystem>();

        playerTransform =
            controllerSystem->GetCurrentController()->GetControlledEntity().GetComponentHandle<TransformComponent>();
}

void TransformSystem::OnShutdown()
{}

void TransformSystem::OnResume()
{}

void TransformSystem::OnSuspend()
{}
