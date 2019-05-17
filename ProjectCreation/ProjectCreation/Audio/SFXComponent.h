#pragma once

#include "../ECS/ECS.h"

namespace GW
{
        namespace AUDIO
        {
                class GSound;
        }
} // namespace GW

struct SFXComponent : public Component<SFXComponent>
{
        GW::AUDIO::GSound* m_SoundEffect;
};