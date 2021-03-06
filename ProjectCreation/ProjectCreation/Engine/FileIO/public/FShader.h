#pragma once
#include <string>
#include <vector>
#include "BasicIOTypes.h"

#include <ResourceInfoTypes.h>

#include <cstddef>

namespace FileIO
{
        struct FShaderData
        {
                std::string       name;
                std::vector<char> bytes;
        };
} // namespace FileIO