#pragma once
#include <unordered_map>
#include <vector>
#include "../ErrorHandling/ErrorTypes.h"
#include "Component.h"
#include "ECSTypes.h"
#include "HandleManager.h"


class ComponentManager
{

        // TODO make friend class relationship
    public:
        typedef std::unordered_map<ComponentTypeId, ComponentHandle>       ComponentTypeIdHandleMap;
        typedef std::unordered_map<EntityHandle, ComponentTypeIdHandleMap> EntityComponentIdMap;
        typedef std::vector<HandleManager<IComponent>>                     TypeAssociativeHandleManagers;

        HandleManager<IComponent>     m_HandleManager;
        TypeAssociativeHandleManagers m_TypeAssociativeHandleManagers;
        EntityComponentIdMap          m_EntityComponentIdMap;

    public:
        // CRITICAL_TODO optimize this class
        //////////////////////////////////////////////////////
        // pools seperated based on type ?
        // class id_type(true_type / false_type) with static member value pattern to store static id?
        //		(already researched all examples use hacky code to do this)
        // sorted pools based on m_active ?
        //////////////////////////////////////////////////////
        template <typename T>
        class ComponentIterator
        {
                // TODO make friend class relationship
            public:
                uint32_t          m_CurrentIndex;
                ComponentManager* m_ComponentManager;

            public:
                ComponentIterator(uint32_t index) :
                    m_CurrentIndex(index){

                    };


                T* operator->()
                {
                        if (m_CurrentIndex >= m_ComponentManager->GetSize())
                                return nullptr;
                        return (T*)m_ComponentManager->m_HandleManager.m_HandleSpace[m_CurrentIndex].second;
                }

                T& operator*() const
                {
                        return *(m_ComponentManager->m_HandleManager.m_HandleSpace[m_CurrentIndex].second);
                }

                ComponentIterator operator++(int)
                {
                        ComponentIterator temp = *this;
                        m_CurrentIndex++;
                        if (m_CurrentIndex >= m_ComponentManager->GetSize())
                        {
                                return temp;
                        }
                        IComponent* _c = m_ComponentManager->m_HandleManager.m_HandleSpace[m_CurrentIndex].second;

                        ///////////////////////////////////////////////
                        // TODO USE ::value possibly pattern to circumvent this call to temporary var creation
                        // WARNING getting ths to work would be complicated and hacky
                        ///////////////////////////////////////////////
                        auto _tempDesiredComponentInstance = T();
                        auto _desiredTypeId                = _tempDesiredComponentInstance.GetStaticTypeId();
                        ///////////////////////////////////////////////
                        if (_c->GetStaticTypeId() != _desiredTypeId)
                                return this->operator++(0);

                        return temp;
                }

                bool operator!=(const ComponentIterator other) const
                {
                        return this->m_CurrentIndex != other.m_CurrentIndex;
                }

                bool operator==(const ComponentIterator other) const
                {
                        return this->m_CurrentIndex == other.m_CurrentIndex;
                }
        };

    public:
        ComponentManager()
        {}
        template <typename T>
        ComponentHandle AddComponent(const EntityHandle entityHandle);
        template <class T>
        T*              GetComponent(const EntityHandle entityHandle);
        void            ActivateComponent(ComponentHandle componentHandle);
        void            DeactivateComponent(ComponentHandle componentHandle);
        ComponentHandle GetComponentHandle(EntityHandle entityHandle, ComponentTypeId componentTypeId);
        IComponent*     GetComponent(ComponentHandle componentHandle);
        template <typename T>
        ComponentIterator<T> GetActiveComponents();
        template <typename T>
        ComponentIterator<T> end();

        size_t GetSize();
        template <typename T>

        // Non-implemented
        //////////////////////////////////////
        //////////////////////////////////////
        //////////////////////////////////////
        EResult CreateComponent(ComponentHandle componentHandle);
        void    DestroyComponent(ComponentHandle componentHandle);
        size_t  GetCapacity();
        size_t  GetActiveComponentCount();
        //////////////////////////////////////
        //////////////////////////////////////
        //////////////////////////////////////
};

template <typename T>
inline ComponentHandle ComponentManager::AddComponent(const EntityHandle entityHandle)
{
        static_assert(std::is_base_of<Component<T>, T>::value,
                      "Addcomponent can only accept CRTP classes that inherit from Component<T>");
        T*              pComponent      = new T();
        ComponentHandle componentHandle = m_HandleManager.GetHandle(pComponent);
        pComponent->m_Owner             = entityHandle;
        ComponentTypeId componentTypeId = pComponent->GetTypeId();
        // TODO
        // highly convenient but also highly unoptimized (requires at minimum TWO hashes for every time this gets called)
        m_EntityComponentIdMap[entityHandle][componentTypeId] = componentHandle;

		// NOTE 
		// this if check will be called EXTREMELY RARELY
        if (componentTypeId >= m_TypeAssociativeHandleManagers.size())
        {
                m_TypeAssociativeHandleManagers.resize(componentTypeId);
        }
        m_TypeAssociativeHandleManagers[componentTypeId].GetHandle(pComponent);

        return componentHandle;
}

template <class T>
inline T* ComponentManager::GetComponent(const EntityHandle entityHandle)
{
        ComponentTypeId componentTypeId = T::GetTypeId();
        ComponentHandle componentHandle = GetComponentHandle(entityHandle, componentTypeId);
        if (componentHandle.IsValid())
                return nullptr;
        else
                return (T*)m_HandleManager.GetObject(componentHandle);
}

template <typename T>
inline ComponentManager::ComponentIterator<T> ComponentManager::GetActiveComponents()
{
        auto _itr                       = ComponentIterator<T>(0);
        _itr.m_ComponentManager         = this;
        ComponentTypeId componentTypeId = T::GetTypeId();
        auto            test            = _itr->GetTypeId();
        return _itr;
}

template <typename T>
inline ComponentManager::ComponentIterator<T> ComponentManager::end()
{
        return ComponentManager::ComponentIterator<T>(GetSize());
}
