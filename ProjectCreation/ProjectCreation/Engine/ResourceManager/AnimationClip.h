#pragma once

#include "Resource.h"

#include <D3DNativeTypes.h>

#include "../../Engine/Animation/AnimationContainers.h"

struct AnimationClip : public Resource<AnimationClip>
{
        virtual void Release() override;

        Animation::FAnimClip m_AnimClip;
};