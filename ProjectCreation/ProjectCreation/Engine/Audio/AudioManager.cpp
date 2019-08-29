#include "AudioManager.h"
#include <assert.h>
#include <sstream>
#include "../../Utility/MemoryLeakDetection.h"
#include "../MathLibrary/MathLibrary.h"

#include "..//GEngine.h"
#include "ContinousSoundSystem.h"

using namespace GW::AUDIO;

AudioManager *AudioManager::instance;

EntityHandle AudioManager::PlaySoundWithVolume(float volume, const char *name)
{
        return GET_SYSTEM(SpatialSoundSystem)->PlaySoundWithVolume(volume, name);
}


EntityHandle AudioManager::PlaySoundAtLocation(const DirectX::XMVECTOR &pos, SoundComponent3D::FSettings &settings)
{
        return GET_SYSTEM(SpatialSoundSystem)->PlaySoundAtLocation(pos, settings);
}

GW::AUDIO::GSound *AudioManager::CreateSFX(const char *name)
{
        GSound *output;

        std::ostringstream filePathStream;
        filePathStream << "../Assets/SFX/" << name << ".wav";
        std::string str = filePathStream.str();

        m_SoundEngine->CreateSound(str.c_str(), &output);
        return output;
}

GMusic *AudioManager::LoadMusic(const char *name)
{
        GMusic *output;

        std::ostringstream filePathStream;
        filePathStream << "../Assets/Music/" << name << ".wav";
        std::string str = filePathStream.str();

        auto it = m_LoadedMusic.find(name);
        if (it == m_LoadedMusic.end())
        {
                GW::GReturn returnCode = m_SoundEngine->CreateMusicStream(str.c_str(), &output);
                output->SetVolume(m_MasterVolume);
                m_LoadedMusic.insert(std::make_pair(name, output));
        }
        else
        {
                output = it->second;
        }

        return output;
}

void AudioManager::SetMasterVolume(float val)
{
        assert(m_SoundEngine);
        // m_SoundEngine->SetMasterVolume(val);
        val *= 0.1f;
        m_MasterVolume = val;
        for (auto &music : m_LoadedMusic)
        {
                music.second->SetVolume(val);
        }
        GET_SYSTEM(SpatialSoundSystem)->SetMasterVolume(val);
}

void AudioManager::ActivateMusicAndPause(GW::AUDIO::GMusic *m, bool looping)
{
        m->StreamStart(looping);
        m->SetVolume(m_MasterVolume);
        m->PauseStream();
}

void AudioManager::PlayMusic(const char *name)
{}

void AudioManager::_shutdown()
{

        for (auto &it : m_LoadedMusic)
        {
                it.second->DecrementCount();
        }
        m_SoundEngine->DecrementCount();
}

void AudioManager::StopAllMusic()
{
        for (auto &music : m_LoadedMusic)
        {
                music.second->StopStream();
        }
}

void AudioManager::Initialize()
{
        assert(!instance);
        instance              = DBG_NEW AudioManager;
        GW::GReturn        gr = (GW::AUDIO::CreateGAudio(&instance->m_SoundEngine));
        assert(G_SUCCESS(gr));
}

void AudioManager::Shutdown()
{
        assert(instance);
        instance->_shutdown();
        delete instance;
        instance = nullptr;
}

AudioManager *AudioManager::Get()
{
        // assert(instance);
        return instance;
}
