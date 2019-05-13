#pragma once
#include <assert.h>
#include <stdint.h>
#include <type_traits>
#include "Component.h"
#include "ECSTypes.h"
#include "Entity.h"
#include "HandleManager.h"
// TODO	-> replace these containers with EASTL containers
// or optionally an custom EntityContainer
#include <unordered_map>
#include <vector>

class EntityManager
{
        // TODO	-> replace these containers with EASTL containers / another container type
        typedef std::vector<IEntity*>                    EntityPool;
        std::unordered_map<TypeId<IEntity>, EntityPool*> entity_pools;
        HandleManager<IEntity>                           m_HandleManager;

    public:
        // XXXXXXXXXXX
        IEntity* GetEntity(Handle<IEntity> handle);
        // OOOOOOOOOOO
        template <typename T>
        Handle<IEntity> CreateEntity();
        // XXXXXXXXXXX
        template <typename T>
        void DestroyEntity(Handle<IEntity> entityHandle);
        // XXXXXXXXXXX
        template <typename T>
        void ActivateEntity(Handle<IEntity> entityHandle);
        // XXXXXXXXXXX
        template <typename T>
        void DeactivateEntity(Handle<IEntity> entityHandle);
        // XXXXXXXXXXX
        template <typename T>
        size_t GetContainerSize();
        // XXXXXXXXXXX
        template <typename entityType>
        size_t GetContainerActiveCount();
        // XXXXXXXXXXX
        template <typename T>
        Component<T>* GetActiveComponents();
        ~EntityManager();
};

//inline IEntity* EntityManager::GetEntity(Handle<IEntity> handle)
//{
//        TypeId<IEntity> id          = T::GetTypeId();
//        auto            pEntPoolItr = entity_pools.find(id);
//        if (pEntPoolItr == entity_pools.end())
//                return nullptr;
//        else
//        {
//                for (auto IEntityItr : *(pEntPoolItr->second))
//                {
//						auto bbbbb = m_HandleManager.GetHandle(IEntityItr);
//                        if (m_HandleManager.GetHandle(IEntityItr) == handle)
//                                return (T*)IEntityItr;
//                }
//        }
//
//        return nullptr;
//}

// Explanation:
//	Gets a reference to the entityContainer (this is a pool-like container)
//				// Create the Entity container if it does not exist
//				// Retrieve the Entity container if it does
//	Create the enitity within the container
//	return the HandleId

template <typename T>
inline Handle<IEntity> EntityManager::CreateEntity()
{
        static_assert(std::is_base_of<Entity<T>, T>::value, "CreateEntity can only accept CRTP classes derived from Entity<T>");
        TypeId<IEntity> id = T::GetTypeId();

        T* allocatedEntity = nullptr;

        // TODO: modify this from using a vector (For reference Tobias Sein uses a linked list of allocated
        // arrays to allocate Entities then he constructs them by passing the variadic template arguments to the placement new
        // operator)
        for (auto& itr : entity_pools)
        {
                if (itr.first == id)
                {
                        // Explanation: A pool was found for the specified id, iterate until we find a nullptr or get to the end
                        // of the array
                        for (size_t i = 0; i <= itr.second->size(); ++i)
                        {
                                if (i == itr.second->size())
                                {
                                        // TODO: as a vector it should grow by a reasonable size upon hitting capacity  however,
                                        // we will want it to grow by 1024 and have a specific allignment based on the allocator
                                        // TODO: calls to new should be modified to use a pool allocator
                                        // TODO: calls to new T() SHOULD call the constructor and pass variadic template
                                        // arguments to the constructor. Using placement new on an address that has already been
                                        // allocated for reference:
                                        // https://stackoverflow.com/questions/222557/what-uses-are-there-for-placement-new
                                        allocatedEntity = new T();
                                        itr.second->push_back(allocatedEntity);
                                        break;
                                }
                                else if ((*itr.second)[i] == nullptr)
                                {
                                        allocatedEntity  = new T();
                                        (*itr.second)[i] = allocatedEntity;
                                        break;
                                }
                        }
                        assert(allocatedEntity != nullptr,
                               "If an id match was found by the iterator, Entity should be allocated by this point, if this is "
                               "nullptr something went wrong");
                        // Early exit
                        return m_HandleManager.GetHandle(allocatedEntity);
                }
        }

        // Early exit was never taken because no TypeId of value %id% was found associated with an Entity Pool
        // Create a new EntityPool and associateit with %id%
        entity_pools[id] = new std::vector<IEntity*>();
        allocatedEntity  = new T();
        entity_pools[id]->push_back(allocatedEntity);
        return m_HandleManager.GetHandle(allocatedEntity);
}

template <typename T>
inline void EntityManager::DestroyEntity(Handle<IEntity> entityHandle)
{}

template <typename T>
inline void EntityManager::ActivateEntity(Handle<IEntity> entityHandle)
{}

template <typename T>
inline void EntityManager::DeactivateEntity(Handle<IEntity> entityHandle)
{}

template <typename T>
inline size_t EntityManager::GetContainerSize()
{
        return size_t();
}

template <typename entityType>
inline size_t EntityManager::GetContainerActiveCount()
{
        return size_t();
}

template <typename T>
inline Component<T>* EntityManager::GetActiveComponents()
{
        return NULL;
}
