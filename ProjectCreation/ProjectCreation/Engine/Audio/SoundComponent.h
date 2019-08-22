#pragma once
#include <stdint.h>
#include "../../ECS/Component.h"
#include "3DSoundTypes.h"

#include <bitset>

class SoundComponent : public Component<SoundComponent>
{
    public:
        float              m_Volume;
        GW::AUDIO::GSound* m_gSound = nullptr;
};
