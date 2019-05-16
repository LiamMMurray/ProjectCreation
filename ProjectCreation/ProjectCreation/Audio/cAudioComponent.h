#pragma once
#include <string>
class cAudioComponent
{
    public:
        float GetVolume();
        void  SetVolume(float volume);
       
    private:
		float fVolume;

};