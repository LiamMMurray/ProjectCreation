#pragma once
#include "IContainer.h"
#include "FreeList.h"
#include <stdint.h>

template <typename MainType, typename SubType, uint32_t MAX_ELEMENTS>
class Container : public IContainer<MainType>, public FreeList<MainType, SubType, MAX_ELEMENTS>
{
    private:
        typedef FreeList<MainType, SubType, MAX_ELEMENTS> Allocator;

    public:
        void Initialize()
        {
                Allocator::Initialize();
        }
        Handle<MainType> Allocate()
        {
                return Allocator::Allocate();
        }
        void Free(Handle<MainType>& handle)
        {
                Allocator::Free(handle);
        };
        MainType* operator[](Handle<MainType> handle)
        {
                return static_cast<MainType*>(Allocator::operator[](handle));
        };
        void Shutdown()
        {
                Allocator::Shutdown();
        }
};