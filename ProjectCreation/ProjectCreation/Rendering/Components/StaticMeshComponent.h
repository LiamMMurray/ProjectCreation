#pragma once

#include "../../ECS/HandleManager.h"

#include "../../Engine/ResourceManager/IResource.h"
#include "../../ECS/Component.h"

struct StaticMeshComponent : public Component<StaticMeshComponent>
{
        ResourceHandle m_MaterialHandle;
        ResourceHandle m_StaticMeshHandle;
};