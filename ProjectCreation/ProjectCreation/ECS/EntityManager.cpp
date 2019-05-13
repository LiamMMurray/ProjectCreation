#include "EntityManager.h"

IEntity* EntityManager::GetEntity(Handle<IEntity> handle)
{
                TypeId<IEntity> id          = T::GetTypeId();
                auto            pEntPoolItr = entity_pools.find(id);
                if (pEntPoolItr == entity_pools.end())
                        return nullptr;
                else
                {
                        for (auto IEntityItr : *(pEntPoolItr->second))
                        {
        						auto bbbbb = m_HandleManager.GetHandle(IEntityItr);
                                if (m_HandleManager.GetHandle(IEntityItr) == handle)
                                        return (T*)IEntityItr;
                        }
                }
        
                return nullptr;
}

EntityManager::~EntityManager()
{
		// Deletes allocated memory for the following containers
		//
        // typedef std::vector<IEntity*>                    EntityPool;
        // std::unordered_map<TypeId<IEntity>, EntityPool*> entity_pools;

        // TODO -> delete is likely not to be called in final implementation
        // replace with EASTL container shutdown calls
        for (auto& itr1 : entity_pools)
        {
                for (auto& itr2 : *itr1.second)
                {
                        delete itr2;
                }
                delete itr1.second;
        }
}
