#pragma once
#include <BasicIOTypes.h>

#include <string>
#include <vector>

#include <ResourceInfoTypes.h>

namespace FileIO
{
        struct FTextureDesc
        {
                ETexture2DType textureType;
                file_path_t  filePath;

                inline bool operator==(const FTextureDesc& other)
                {
                        return textureType == other.textureType;
                }
        };

        struct FMaterialData
        {
                std::string name;
                FSurfaceProperties       surfaceProperties;
                file_path_t               vertexShader;
                file_path_t               pixelShader;
                std::vector<FTextureDesc> textureDescs;
        };
} // namespace FileIO