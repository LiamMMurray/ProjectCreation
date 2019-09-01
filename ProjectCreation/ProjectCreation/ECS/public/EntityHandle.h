#pragma once
#include <ECSMem.h>
struct HandleManager;
struct ComponentHandle;
struct Entity;

struct EntityHandle
{
    public:
        static HandleManager* handleContext;
        friend struct HandleManager;

        NMemory::index redirection_index;

        EntityHandle(NMemory::index);
        EntityHandle();
    public:
        Entity* Get();

        void Free();

        bool IsActive();

        void SetIsActive(bool isActive);

        bool operator==(const EntityHandle& other) const;

        template <typename T>
        std::vector<ComponentHandle> GetComponents();

        void FreeComponents();

        template <typename T>
        ComponentHandle GetComponentHandle();

		template <typename T>
        T* GetComponent();

		template <typename T>
        ComponentHandle AddComponent();
};
