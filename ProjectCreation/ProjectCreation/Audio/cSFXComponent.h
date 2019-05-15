#pragma once
// Include
#include <string>
#include "cAudioComponent.h"

class cSFXComponent : public cAudioComponent
{
    public:
        const char* GetSFX();
        void        SetSFX(const char* mSFX);

    private:
        std::string fSFX;
};