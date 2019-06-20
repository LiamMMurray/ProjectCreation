#pragma once
#include "Memory.h"
#include "EntityHandle.h"

struct IPoolElement;
struct HandleManager;

struct ComponentHandle
{
    public:
        static HandleManager* handleContext;
        friend struct HandleManager;

        NMemory::type_index pool_index;
        NMemory::index      redirection_index;

        ComponentHandle(NMemory::type_index pool_index, NMemory::index redirection_index);
        ComponentHandle();

    public:
        template <typename T = IPoolElement>
        T* Get();

        void Free();

        bool IsActive();

        void SetIsActive(bool isActive);

        bool operator==(const ComponentHandle& other) const;

        // EntityHandle GetParent();
};