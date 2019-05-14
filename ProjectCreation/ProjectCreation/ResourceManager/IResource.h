#pragma once

#include <string>
#include "../ECS/ECSTypes.h"
#include "../ECS/Util.h"

class IResource;

typedef Handle<IResource> ResourceHandle;

class IResource
{

    protected:
        ResourceHandle m_Handle;
        int16_t        m_ReferenceCount = 0;
        std::string    m_Name;


    public:
        void Init(std::string name, ResourceHandle handle)
        {
                m_Handle = handle;
				m_Name = name;
		}

        inline std::string GetName() const
        {
                return m_Name;
        }
        ResourceHandle GetHandle();
        ResourceHandle AcquireHandle();
        int16_t        ReleaseHandle();
        virtual void   Release() = 0;
};