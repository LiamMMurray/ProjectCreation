#include "AudioManager.h"
#include <assert.h>
#include <sstream>
#include "../../Utility/MemoryLeakDetection.h"
#include "../MathLibrary/MathLibrary.h"
using namespace GW::AUDIO;

AudioManager *AudioManager::instance;

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
                m_SoundEngine->CreateMusicStream(str.c_str(), &output);
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
        m_SoundEngine->SetMasterVolume(val);
        m_MasterVolume = val;
}

void AudioManager::ActivateMusicAndPause(GW::AUDIO::GMusic *m, bool looping)
{
        m->StreamStart(looping);
        m->PauseStream();
}

void AudioManager::_shutdown()
{

        for (auto &it : m_LoadedMusic)
        {
                it.second->DecrementCount();
        }
        m_SoundEngine->DecrementCount();

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
}

AudioManager *AudioManager::Get()
{
        assert(instance);
        return instance;
}
