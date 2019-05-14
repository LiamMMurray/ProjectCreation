#pragma once

#include "../ECS/ECS.h"

#include "../ResourceManager/IResource.h"

class StaticMeshComponent : public Component<StaticMeshComponent>
{
        ResourceHandle m_MaterialHandle;
        ResourceHandle m_StaticMeshHandle;

    public:
        void SetMaterialHandle(const ResourceHandle& matHandl);
        const ResourceHandle GetMaterialHandle() const;

        void SetStaticMeshHandle(const ResourceHandle& meshHandl);
        const ResourceHandle GetStaticMeshHandle() const;
};