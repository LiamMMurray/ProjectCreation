#pragma once
	//Include
#include <string>
#include "cAudioComponent.h"

class cMusicComponent : public cAudioComponent
{
    public:
        const char* GetMusic();
        void        SetMusic(const char* mSFX);

    private:
        std::string fMusic;
};