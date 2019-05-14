#include "cSFXComponent.h"

const char* cSFXComponent::GetSFX()
{
        return cSFXComponent::fSFX.c_str();
}

void cSFXComponent::SetSFX(const char* mSFX)
{
        cSFXComponent::fSFX = mSFX;
}