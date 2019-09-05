#pragma once

// Includes
#include <DirectXMath.h>
#include <ECSTypes.h>
#include <Interface/G_Audio/GAudio.h>
#include <string>
#include <unordered_map>
#include "3DSoundComponent.h"

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

struct FMusic
{
        GW::AUDIO::GMusic* music;
        float              volume;
};

class AudioManager
{

    private:
        GW::AUDIO::GAudio*                      m_SoundEngine = nullptr;
        std::unordered_map<std::string, FMusic> m_LoadedMusic;
        float                                   m_MasterVolume     = 1.0f;
        float                                   m_PrevMasterVolume = 1.0f;
        static AudioManager*                    instance;

    public:
        EntityHandle PlaySoundWithVolume(float volume, const char* name);

        EntityHandle PlaySoundAtLocation(const DirectX::XMVECTOR& pos, SoundComponent3D::FSettings& settings);

        GW::AUDIO::GSound* CreateSFX(const char*);
        FMusic*            LoadMusic(const char*, float volume = -1.0f);
        void               SetMasterVolume(float val);
        void               ActivateMusicAndPause(FMusic* music, bool looping = false);


        inline float GetMasterVolume()
        {
                return m_MasterVolume;
        }

        void _shutdown();

        void StopAllMusic();
        void ResetMusic();

    public:
        static void          Initialize();
        static void          Shutdown();
        static AudioManager* Get();
};