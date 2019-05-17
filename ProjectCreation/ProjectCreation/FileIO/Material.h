#pragma once
#include "BasicIOTypes.h"
#include <string>
#include <vector>

#include "../Engine/ResourceManager/ResourceInfoTypes.h"

namespace FileIO
{
        struct FTextureDesc
        {
                ETexture2DType textureType;
                file_path_t  filePath;

                inline bool operator==(const FTextureDesc& other)
                {
                        return textureType.textureType == other.textureType.textureType;
                }
        };

        struct FMaterialData
        {
                std::string name;
                EMaterialType            surfaceType;
                FSurfaceProperties       surfaceProperties;
                std::vector<FTextureDesc> textureDescs;
        };
} // namespace FileIO