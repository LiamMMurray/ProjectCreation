#pragma once	
#include <vector>
#include "ECSTypes.h"
#include "Component.h"
#include "ComponentManager.h"
#include "Handle.h"
class IEntity
{
    public:
        template <typename T>
        ComponentHandle GetComponentHandle();
        template <typename T>
        Component<T>* GetComponent();
        template <typename T>
        std::vector<ComponentHandle> GetComponentHandles();
		template <typename T>
        ComponentIterator<T> GetAllComponents();
		template<typename T>
        EResult CreateComponent(ComponentHandle componentHandle);
		template<typename T>
        void DestroyComponent();
		template<typename T>
        void DestroyComponent(ComponentHandle);
        void Activate();
        void Deactivate();
		EntityTypeID
};