#pragma once

#include <string>
#include "../../ECS/ECSTypes.h"

class IResource;

typedef GHandle<IResource> ResourceHandle;

class IResource
{
    protected:
        ResourceHandle m_Handle;
        int16_t        m_ReferenceCount = 0;
        std::string    m_Name;


        ResourceHandle AcquireHandle();
        int16_t        ReleaseHandle();
        virtual void   Release() = 0;
        virtual void   Copy(IResource* other);

    public:
        ResourceHandle GetHandle();

        void Init(std::string name, ResourceHandle handle)
        {
                m_Handle = handle;
                m_Name   = name;
        }

        inline std::string GetName() const
        {
                return m_Name;
        }
};