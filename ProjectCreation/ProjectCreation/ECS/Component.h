#pragma once
#include "ECSTypes.h"
#include "Handle.h"
#include "Util.h"

class IComponent
{
    public:
        IComponent()
        {}

        virtual ~IComponent()
        {}

        EntityHandle    GetOwner();
        ComponentHandle GetHandle();
        TypeId          GetTypeID();
};

template <typename T>
class Component
{
    public:
        static const TypeId m_TypeId;
};
template <class T>
const TypeId Component<T>::m_TypeId = TypeIdCreator<IComponent>::GetUniqueTypeId<T>();