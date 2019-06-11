#pragma once
#include "../Handle.h"

template <typename MainType>
class IContainer
{
    public:
        virtual void             Shutdown()                          = 0;
        virtual void             Initialize()                        = 0;
        virtual Handle<MainType> Allocate()                          = 0;
        virtual void             Free(Handle<MainType>& handle)      = 0;
        virtual MainType*        operator[](Handle<MainType> handle) = 0;
};
