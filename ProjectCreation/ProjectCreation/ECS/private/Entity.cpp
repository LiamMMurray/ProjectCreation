#include "Entity.h"

NMemory::index Entity::s_max_elements = 9000;

void Entity::SSetMaxElements(NMemory::index max_elements)
{
        s_max_elements = max_elements;
}

NMemory::index Entity::SGetMaxElements()
{
        return s_max_elements;
}

Entity::~Entity()
{}
