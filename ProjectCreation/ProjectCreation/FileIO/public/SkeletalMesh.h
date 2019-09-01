#pragma once

#include "BasicIOTypes.h"

#include <vector>

#include <Vertex.h>
#include "../../Engine/Animation/AnimationContainers.h"

namespace FileIO
{
        struct FSkeletalMeshData
        {
                std::string                   name;
                std::vector<Animation::FJoint> joints;
                std::vector<Animation::FJoint> inverseJoints;
                std::vector<FSkinnedVertex>   vertices;
                std::vector<uint32_t>         indices;
                file_path_t                   materialName;
        };


} // namespace FileIO