#pragma once
#include "ECSTypes.h"
#include "Handle.h"
#include "IComponent.h"
template <typename T>
class ComponentManager
{
    public:
        ComponentManager()
        {}
         T       GetComponent(ComponentHandle componentHandle);
         EResult CreateComponent(ComponentHandle componentHandle);
};