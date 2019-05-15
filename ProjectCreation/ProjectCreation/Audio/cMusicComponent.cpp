#include "cMusicComponent.h"

const char* cMusicComponent::GetMusic()
{
        return cMusicComponent::fMusic.c_str();
}

void cMusicComponent::SetMusic(const char* mSFX)
{
        cMusicComponent::fMusic = mSFX;
}
