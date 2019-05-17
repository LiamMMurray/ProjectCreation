#pragma once

#pragma once

#include "../MathLibrary/MathLibrary.h"

#include <vector>

namespace Animation
{
        struct FKeyFrame
        {
                double                 time;
                std::vector<FTransform> joints;
        };

        struct FAnimClip
        {
                double                duration;
                std::vector<FKeyFrame> frames;
        };

        struct FJoint
        {
                FTransform transform;
                int32_t   parent_index;
        };

        struct FSkeleton
        {
                std::vector<Animation::FJoint> jointTransforms;
                std::vector<Animation::FJoint> inverseBindPose;
        };

} // namespace Animation