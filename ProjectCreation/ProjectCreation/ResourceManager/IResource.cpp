#include "IResource.h"

#include <assert.h>

ResourceHandle IResource::GetHandle()
{
        return m_Handle;
}

ResourceHandle IResource::Acquire()
{
        ++m_ReferenceCount;
        return GetHandle();
}

void IResource::Release()
{
        assert(m_ReferenceCount > 0);
        --m_ReferenceCount;
        return;
}
