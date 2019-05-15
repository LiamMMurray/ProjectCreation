#pragma once

#include "BasicIOTypes.h"

#include <vector>

#include "../Rendering/Vertex.h"
#include "../Engine/Animation/AnimationContainers.h"

namespace FileIO
{
        struct FSkeletalMeshData
        {
                std::string                   name;
                std::vector<Animation::Joint> joints;
                std::vector<Animation::Joint> inverseJoints;
                std::vector<FSkinnedVertex>   vertices;
                std::vector<uint32_t>         indices;
                file_path_t                   materialName;
        };


} // namespace FileIO