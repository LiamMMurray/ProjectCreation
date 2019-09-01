#pragma once

#include <HandleManager.h>

#include "../../ResourceManager/IResource.h"
#include <Component.h>

struct StaticMeshComponent : public Component<StaticMeshComponent>
{
        ResourceHandle m_MaterialHandle;
        ResourceHandle m_StaticMeshHandle;
};