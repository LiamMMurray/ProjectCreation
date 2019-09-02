#pragma once

#pragma once

#include <HandleManager.h>

#include <IResource.h>
#include "../../Animation/AnimationContainers.h"

struct SkeletalMeshComponent : public Component<SkeletalMeshComponent>
{
        ResourceHandle m_MaterialHandle;
        ResourceHandle m_SkeletalMeshHandle;

		Animation::FSkeleton m_Skeleton;
};