#include "AudioManager.h"

#include <assert.h>

AudioManager* AudioManager::instance;

void AudioManager::Initialize()
{
        assert(instance == nullptr);
        instance = new AudioManager();
}

void AudioManager::Shutdown()
{
        assert(instance != nullptr);
        delete instance;
}

AudioManager* AudioManager::Get()
{
        assert(instance != nullptr);
        return instance;
}


// void AudioManager::AddMusic(std::string mAssetPath, float mVolume)
//{
//        cMusicComponent* sMusic = new cMusicComponent();
//        sMusic->SetMusic(mAssetPath.c_str());
//        sMusic->SetVolume(mVolume);
//        fSounds.push_back(sMusic);
//}

// void AudioManager::AddSFX(std::string mAssetPath, float mVolume)
//{
//        cSFXComponent* sBoop = new cSFXComponent();
//        sBoop->SetSFX(mAssetPath.c_str());
//        sBoop->SetVolume(mVolume);
//        fSounds.push_back(sBoop);
//}


void AudioManager::PlayMusic()
{
        if (G_FAIL(fSoundEngine->CreateMusicStream(music.c_str(), &fMusic)))
        {
                printf("ERROR");
        }
        else
        {
                fMusic->SetVolume(musicVolume);
                fMusic->StreamStart(true);
        }
}

void AudioManager::PlaySFX()
{
        if (G_FAIL(fSoundEngine->CreateSound(sfx1.c_str(), &fSFX)))
        {
                printf("ERROR");
        }
        if (fSFX)
        {
                fSFX->SetVolume(sfxVolume);
                fSFX->Play();
        }
}

void AudioManager::PlaySounds()
{
        if (G_SUCCESS(GW::AUDIO::CreateGAudio(&fSoundEngine)))
        {
                AudioManager::PlayMusic();
                AudioManager::PlaySFX();
        }
}