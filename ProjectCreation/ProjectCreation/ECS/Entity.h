#pragma once
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include "IPoolElement.h"


struct Entity : IPoolElement
{
        static NMemory::index s_max_elements;

        static void           SSetMaxElements(NMemory::index max_elements);
        static NMemory::index SGetMaxElements();

        NMemory::entity_component_container m_OwnedComponents;

		~Entity();
};
