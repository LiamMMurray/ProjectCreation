#pragma once

#include <string>
#include <vector>

#include "BasicIOTypes.h"

namespace FileIO
{
        struct FStaticMeshData
        {
                std::string           name;
                //std::vector<Vertex>   vertices;
                std::vector<uint32_t> indices;
                file_path_t           materialName;
        };
} // namespace FileIO