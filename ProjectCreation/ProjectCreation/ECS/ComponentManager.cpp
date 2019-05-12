#include "ComponentManager.h"

EResult ComponentManager::CreateComponent(Handle<IComponent> componentHandle)
{
        return EResult();
}

void ComponentManager::DestroyComponent(Handle<IComponent> componentHandle)
{}

void ComponentManager::ActivateComponent(Handle<IComponent> componentHandle)
{}

void ComponentManager::DeactivateComponent(Handle<IComponent> componentHandle)
{}

size_t ComponentManager::GetCapacity()
{
        return size_t();
}


size_t ComponentManager::GetSize()
{
        return size_t();
}

size_t ComponentManager::GetActiveComponentCount()
{
        return size_t();
}
