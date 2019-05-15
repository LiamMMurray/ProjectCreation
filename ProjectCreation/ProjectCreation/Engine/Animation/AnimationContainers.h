#pragma once

#pragma once

#include "../../MathLibrary/MathLibrary.h"

#include <vector>

namespace Animation
{
        struct KeyFrame
        {
                double                 time;
                std::vector<FTransform> joints;
        };

        struct AnimClip
        {
                double                duration;
                std::vector<KeyFrame> frames;
        };

        struct Joint
        {
                FTransform transform;
                int32_t   parent_index;
        };

        struct Skeleton
        {
                std::vector<Animation::Joint> jointTransforms;
                std::vector<Animation::Joint> inverseBindPose;
        };

} // namespace Animation