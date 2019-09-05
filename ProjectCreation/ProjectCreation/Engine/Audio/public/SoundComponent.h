#pragma once
#include <Component.h>
#include "3DSoundTypes.h"

class SoundComponent : public Component<SoundComponent>
{
    public:
        float              m_Volume;
        GW::AUDIO::GSound* m_gSound = nullptr;
};
