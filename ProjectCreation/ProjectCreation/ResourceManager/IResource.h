#pragma once

#include "../ECS/ECSTypes.h"
#include "../ECS/Util.h"

class IResource;

typedef Handle<IResource> ResourceHandle;

class IResource
{

    protected:
        ResourceHandle m_Handle;
        uint16_t       m_ReferenceCount = 0;

    public:
        ResourceHandle GetHandle();
        ResourceHandle Acquire();
        void Release();

        friend class ResourceManager;
};