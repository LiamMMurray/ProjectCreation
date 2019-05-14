#include "StaticMeshComponent.h"

void StaticMeshComponent::SetMaterialHandle(const ResourceHandle& matHandl)
{
        m_MaterialHandle = matHandl;
}

const ResourceHandle StaticMeshComponent::GetMaterialHandle() const
{
        return m_MaterialHandle;
}

void StaticMeshComponent::SetStaticMeshHandle(const ResourceHandle& meshHandl)
{
        m_StaticMeshHandle = meshHandl;
}

const ResourceHandle StaticMeshComponent::GetStaticMeshHandle() const
{
        return m_StaticMeshHandle;
}
