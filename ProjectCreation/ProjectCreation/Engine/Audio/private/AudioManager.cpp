#include "AudioManager.h"
#include <MathLibrary.h>
#include <MemoryLeakDetection.h>
#include <assert.h>
#include <sstream>

#include <GEngine.h>
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

FMusic *AudioManager::LoadMusic(const char *name, float volume)
{
        FMusic *output;

        std::ostringstream filePathStream;
        filePathStream << "../Assets/Music/" << name << ".wav";
        std::string str = filePathStream.str();

        auto it = m_LoadedMusic.find(name);
        if (it == m_LoadedMusic.end())
        {
                auto newIt = m_LoadedMusic.emplace(name, FMusic());
                output     = &newIt.first->second;
                m_SoundEngine->CreateMusicStream(str.c_str(), &output->music);
        }
        else
        {
                output = &it->second;
        }

        if (volume >= 0.0f)
                output->volume = volume;

        output->music->SetVolume(m_MasterVolume * volume);

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
                music.second.music->SetVolume(val * music.second.volume);
        }
        GET_SYSTEM(SpatialSoundSystem)->SetMasterVolume(val);
}

void AudioManager::ActivateMusicAndPause(FMusic *m, bool looping)
{
        m->music->StreamStart(looping);
        m->music->SetVolume(m_MasterVolume * m->volume);
        m->music->PauseStream();
}


void AudioManager::_shutdown()
{

        for (auto &it : m_LoadedMusic)
        {
                it.second.music->DecrementCount();
        }
        m_SoundEngine->DecrementCount();
}

void AudioManager::StopAllMusic()
{
        for (auto &music : m_LoadedMusic)
        {
                music.second.music->StopStream();
        }
}

void AudioManager::ResetMusic()
{
        StopAllMusic();

        LoadMusic("Ambience_andWaves", 0.6f);
        LoadMusic("EARTHQUAKE_SFX", 0.3f);
        LoadMusic("LEVEL_4_AMBIENCE", 0.6f);

        auto fmusic = LoadMusic("Ambience 16-48k", 1.0f);
        ActivateMusicAndPause(fmusic, true);
        fmusic->music->ResumeStream();
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
