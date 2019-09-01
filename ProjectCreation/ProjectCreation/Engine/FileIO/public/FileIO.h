#pragma once

#include <string>

#include <ErrorTypes.h>

#include "DDSTextureLoader.h"

#include "../../Animation/AnimationContainers.h"

#include "Material.h"
#include "SkeletalMesh.h"
#include "StaticMesh.h"
#include "Shader.h"

namespace FileIO
{
        EResult LoadStaticMeshDataFromFile(const char* fileName, FStaticMeshData* staticMeshOutput);
        EResult LoadSkeletalMeshDataFromFile(const char* fileName, FSkeletalMeshData* skelMeshOutput);
        EResult LoadMaterialDataFromFile(const char* fileName, FMaterialData* matDataOutput);
        EResult LoadShaderDataFromFile(const char* fileName, const char* suffix, FShaderData* shaderDataOutput);
        EResult ImportAnimClipData(const char* fileName, Animation::FAnimClip& animClip, const Animation::FSkeleton& skeleton);
}