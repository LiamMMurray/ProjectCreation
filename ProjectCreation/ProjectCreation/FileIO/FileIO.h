#pragma once

#include <string>

#include "../ErrorHandling/ErrorTypes.h"

#include "Material.h"
#include "SkeletalMesh.h"
#include "StaticMesh.h"

namespace FileIO
{
        EResult LoadStaticMeshDataFromFile(const char* fileName, FStaticMeshData* staticMeshOutput);
        EResult LoadSkeletalMeshDataFromFile(const char* fileName, FSkeletalMeshData* staticMeshOutput);
        EResult LoadMaterialDataFromFile(const char* fileName, FMaterialData* staticMeshOutput);
}