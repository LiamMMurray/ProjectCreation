#pragma once

#pragma once

#include "../ECS/ECS.h"

#include "../ResourceManager/IResource.h"
#include "../Engine/Animation/AnimationContainers.h"

struct SkeletalMeshComponent : public Component<SkeletalMeshComponent>
{
        ResourceHandle m_MaterialHandle;
        ResourceHandle m_SkeletalMeshHandle;

		Animation::FSkeleton m_Skeleton;
};