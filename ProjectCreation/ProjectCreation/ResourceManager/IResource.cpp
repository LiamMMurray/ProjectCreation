#include "IResource.h"

#include <assert.h>

ResourceHandle IResource::GetHandle()
{
        return m_Handle;
}

ResourceHandle IResource::AcquireHandle()
{
        ++m_ReferenceCount;
        return GetHandle();
}

int16_t IResource::ReleaseHandle()
{
        --m_ReferenceCount;
        return m_ReferenceCount;
}
