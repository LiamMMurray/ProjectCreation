#pragma once

#include <string>
#include <vector>
#include "../../ECS/ECSTypes.h"

namespace EntityFactory
{
        EntityHandle CreateStaticMeshEntity(const char*      staticMeshName,
                                            const char*      materialName,
                                            ComponentHandle* outTransformHandle  = nullptr,
                                            ComponentHandle* outStaticMeshHandle = nullptr);

        EntityHandle CreateSkeletalMeshEntity(const char*              skeletalMeshName,
                                              const char*              materialName,
                                              std::vector<std::string> animNames,
                                              float*                   weights               = nullptr,
                                              ComponentHandle*         outTransformHandle    = nullptr,
                                              ComponentHandle*         outSkeletalMeshHandle = nullptr,
                                              ComponentHandle*         outAnimCompHandle     = nullptr);
} // namespace EntityFactory