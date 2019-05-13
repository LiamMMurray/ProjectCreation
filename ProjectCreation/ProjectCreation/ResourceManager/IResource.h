#pragma once

#include "../ECS/ECSTypes.h"
#include "../ECS/Util.h"
#include <string>

class IResource;

typedef Handle<IResource> ResourceHandle;

class IResource
{

    protected:
        ResourceHandle m_Handle;
        uint16_t       m_ReferenceCount = 0;
        std::string    m_Name;


    public:
        ResourceHandle GetHandle();
        ResourceHandle Acquire();
        uint16_t       Release();

        friend class ResourceManager;
};