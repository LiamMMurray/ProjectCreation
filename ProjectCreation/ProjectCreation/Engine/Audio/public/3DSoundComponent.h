#pragma once

#include <stdint.h>
#include <Component.h>
#include "3DSoundTypes.h"

#include <bitset>


struct SoundComponent3D : public Component<SoundComponent3D>
{
        struct E_FLAGS
        {
                enum
                {
                        Looping,
                        ShouldPlay,
                        DestroyOnEnd,
                };
        };

        struct FSettings
        {
                int             m_SoundType     = 0;
                int             m_SoundVaration = 0;
                float           m_Radius        = 2.0f;
                float           m_Falloff       = 25.0f;
                float           m_Volume        = 1.0f;
                std::bitset<32> flags           = 0;
        };

        FSettings          m_Settings;
        int16_t            m_SoundPoolIndex = -1;
        GW::AUDIO::GSound* m_gwSound        = nullptr;
        virtual ~SoundComponent3D();
};