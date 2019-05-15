#include "EntityManager.h"

void EntityManager::Initialize(ComponentManager* componentManager)
{
        m_ComponentManager = componentManager;
}

IEntity* EntityManager::GetEntity(EntityHandle handle)
{
        return m_HandleManager.GetObject(handle);
}

EntityHandle EntityManager::GetEntityHandle(IEntity* pEntity)
{
        return m_HandleManager.GetHandle(pEntity);
}

EntityManager::~EntityManager()
{
	// TODO: clean up new allocations from m_HandleManager or rewrite allocation code for this class
}
