#pragma once

#include <string>
#include <vector>

#include "BasicIOTypes.h"

#include <Vertex.h>

namespace FileIO
{
        struct FStaticMeshData
        {
                std::string           name;
                std::vector<FVertex>   vertices;
                std::vector<uint32_t> indices;
                file_path_t           materialName;
        };
} // namespace FileIO