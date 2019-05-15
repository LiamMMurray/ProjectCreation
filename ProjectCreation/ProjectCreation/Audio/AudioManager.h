#pragma once

// Includes
#include <string>
#include <vector>

#include <Interface/G_Audio/GAudio.h>
#include "cAudioComponent.h"
#include "cMusicComponent.h"
#include "cSFXComponent.h"

// Defines

//! Macro used to determine if a function succeeded.
/*!
 *	\param [in] _greturn_ The GReturn value to check.
 *
 *	\retval true GReturn value passed in was SUCCESS.
 *	\retval false GReturn value passed in was a failure code.
 */
#define G_SUCCESS(_greturn_) ((~(_greturn_)) == 0x00000000)

//! Macro used to determine if a function has failed.
/*
 *	\param [in] _greturn_ The GReturn value to check.
 *
 *	\retval true GReturn value passed in was a failure code.
 *	\retval false GReturn value passed in was SUCCESS.
 */
#define G_FAIL(_greturn_) ((_greturn_) < 0xFFFFFFFF)

class AudioManager
{
    public:
        void PlaySounds();

		static void Initialize();
        static void Shutdown();
        static AudioManager* Get();

    private:
		static AudioManager* instance;

        void PlaySFX();
        void PlayMusic();
        //void AddMusic(std::string mAssetPath, float mVolume);
        //void AddSFX(std::string mAssetPath, float mVolume);


        GW::AUDIO::GAudio* fSoundEngine = nullptr;
        GW::AUDIO::GMusic* fMusic       = nullptr;
        GW::AUDIO::GSound* fSFX         = nullptr;

        const std::string music       = "Assets\Music\extreme.wav";
        const float       musicVolume = 0.65f;

        const std::string sfx1      = "..\\Assets\\SFX\\boop.wav";
        const float       sfxVolume = 1.0f;


        std::vector<cAudioComponent*> fSounds;
};