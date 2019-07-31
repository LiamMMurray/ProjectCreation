#include "ContinousSoundSystem.h"
#include "..//GenericComponents/TransformComponent.h"
#include "..//MathLibrary/MathLibrary.h"
#include "../Controller/ControllerSystem.h"
#include "../GEngine.h"
#include "3DSoundComponent.h"
#include "AudioManager.h"

void SpatialSoundSystem::OnPreUpdate(float deltaTime)
{}

void SpatialSoundSystem::OnUpdate(float deltaTime)
{
        ControllerSystem*   controllerSystem  = GEngine::Get()->GetSystemManager()->GetSystem<ControllerSystem>();
        IController*        currentController = controllerSystem->GetCurrentController();
        TransformComponent* playerTransformComponent =
            currentController->GetControlledEntity().GetComponent<TransformComponent>();

        for (auto& soundComp : m_HandleManager->GetActiveComponents<SoundComponent3D>())
        {
                EntityHandle        parent             = soundComp.GetParent();
                TransformComponent* transformComponent = parent.GetComponent<TransformComponent>();

                int16_t index = soundComp.m_SoundPoolIndex;
                int     type  = soundComp.m_Settings.m_SoundType;

                bool isPlaying;
                m_SoundPools[type][index].gwSound->isSoundPlaying(isPlaying);

                if (isPlaying == false)
                {
                        if (soundComp.m_Settings.flags.test(SoundComponent3D::E_FLAGS::Looping) == true)
                        {
                                if (soundComp.m_Settings.flags.test(SoundComponent3D::E_FLAGS::ShouldPlay) == true)
                                        m_SoundPools[type][index].gwSound->Play();
                        }
                        else
                        {

                                if (soundComp.m_Settings.flags.test(SoundComponent3D::E_FLAGS::DestroyOnEnd) == true)
                                        parent.Free();
                        }
                };

                float distance = MathLibrary::CalulateDistance(playerTransformComponent->transform.translation,
                                                               transformComponent->transform.translation);

                float alpha =
                    1.0f - MathLibrary::saturate((distance - soundComp.m_Settings.m_Radius) / soundComp.m_Settings.m_Falloff);

                m_SoundPools[type][index].gwSound->SetVolume(alpha * soundComp.m_Settings.m_Volume);
        }
}
void SpatialSoundSystem::OnPostUpdate(float deltaTime)
{}

void SpatialSoundSystem::OnInitialize()
{
        using namespace Pools;

        m_HandleManager = GEngine::Get()->GetHandleManager();

        for (int i = 0; i < 3; ++i)
        {
                for (int j = 0; j < E_SOUND_TYPE::TYPE_COUNT; ++j)
                {
                        m_SoundPoolSizes[SOUND_COLOR_TYPE(i, j)] = E_SOUND_POOL_SIZES[j];
                }
        }

        for (size_t i = 0; i < E_SOUND_TYPE::TOTAL_TYPE_COUNT; ++i)
        {
                m_SoundPools[i].Init(m_SoundPoolSizes[i]);
                for (size_t j = 0; j < m_SoundPoolSizes[i]; j++)
                {
                        m_SoundPools[i][j].gwSound = AudioManager::Get()->CreateSFX("test");
                }
        }

        ControllerSystem*   controllerSystem  = GEngine::Get()->GetSystemManager()->GetSystem<ControllerSystem>();
        IController*        currentController = controllerSystem->GetCurrentController();
        TransformComponent* playerTransformComponent =
            currentController->GetControlledEntity().GetComponent<TransformComponent>();

        // SoundComponent3D::FSettings settings;
        // settings.flags.set(SoundComponent3D::E_FLAGS::Looping, true);
        // settings.flags.set(SoundComponent3D::E_FLAGS::ShouldPlay, true);
        // PlaySoundAtLocation(playerTransformComponent->transform.translation, settings);
}

void SpatialSoundSystem::OnShutdown()
{}

void SpatialSoundSystem::OnResume()
{}

void SpatialSoundSystem::OnSuspend()
{}

EntityHandle SpatialSoundSystem::PlaySoundAtLocation(const DirectX::XMVECTOR& pos, SoundComponent3D::FSettings& settings)
{
        EntityHandle entity = m_HandleManager->CreateEntity();

        ComponentHandle     transCompHandle = entity.AddComponent<TransformComponent>();
        TransformComponent* transComp       = transCompHandle.Get<TransformComponent>();
        transComp->transform.translation    = pos;

        ComponentHandle   soundCompHandle = entity.AddComponent<SoundComponent3D>();
        SoundComponent3D* soundComponent  = soundCompHandle.Get<SoundComponent3D>();
        soundComponent->m_Settings        = settings;
        soundComponent->m_SoundPoolIndex  = m_SoundPools[settings.m_SoundType].alloc();

        if (soundComponent->m_SoundPoolIndex != -1)
        {
                soundComponent->m_gwSound = m_SoundPools[settings.m_SoundType][soundComponent->m_SoundPoolIndex].gwSound;
                soundComponent->m_gwSound->Play();
        }
		else
		{
                entity.Free();
		}

        return entity;
}

void SpatialSoundSystem::FreeSound(uint16_t type, int16_t index)
{
        m_SoundPools[type][index].gwSound->StopSound();
        m_SoundPools[type].free(index);
}
