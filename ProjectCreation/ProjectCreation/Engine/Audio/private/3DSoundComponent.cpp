#include <3DSoundComponent.h>
#include <GEngine.h>
#include <AudioManager.h>
#include <ContinousSoundSystem.h>

SoundComponent3D::~SoundComponent3D()
{
        if (AudioManager::Get() && m_gwSound)
        {
                GET_SYSTEM(SpatialSoundSystem)->FreeSound(m_Settings.m_SoundType, m_Settings.m_SoundVaration, m_SoundPoolIndex);
        }
}