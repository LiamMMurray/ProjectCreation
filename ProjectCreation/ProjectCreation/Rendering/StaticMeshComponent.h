#pragma once

#include "../ECS/ECS.h"

#include "../ResourceManager/IResource.h"

struct StaticMeshComponent : public Component<StaticMeshComponent>
{
        ResourceHandle m_MaterialHandle;
        ResourceHandle m_StaticMeshHandle;
};