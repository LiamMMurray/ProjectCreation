#pragma once

#include "BasicIOTypes.h"

#include <vector>

namespace FileIO
{
        struct FSkeletalMeshData
        {
                std::string                   name;
                //std::vector<Animation::Joint> joints;
                //std::vector<Animation::Joint> inverseJoints;
                //std::vector<SkinnedVertex>    vertices;
                std::vector<uint32_t>         indices;
                file_path_t                   materialName;
        };


} // namespace FileIO