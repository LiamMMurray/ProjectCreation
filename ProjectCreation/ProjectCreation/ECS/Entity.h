#pragma once
#include "ECSTypes.h"
#include "IEntity.h"

template <typename T>
class Entity : public IEntity
{
    public:
        static const EntityTypeId m_TypeId;
        const EntityTypeId        GetStaticEntityTypeID() const
        {
                return m_TypeId;
        }
};
template <typename T>
const EntityTypeId Entity<T>::m_TypeId{TypeIdCreator<IEntity>::GetUniqueTypeId<T>()};