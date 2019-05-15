#include "cAudioComponent.h"

float cAudioComponent::GetVolume()
{
        return cAudioComponent::fVolume;
}

void cAudioComponent::SetVolume(float volume)
{
        cAudioComponent::fVolume = volume;
}