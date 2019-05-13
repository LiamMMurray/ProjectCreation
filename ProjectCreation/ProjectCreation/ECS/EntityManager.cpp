#include "EntityManager.h"

IEntity* EntityManager::GetEntity(Handle<IEntity> handle)
{
        return m_HandleManager.GetObject(handle);
}

EntityManager::~EntityManager()
{
	// TODO: clean up new allocations from m_HandleManager or rewrite allocation code for this class
}
