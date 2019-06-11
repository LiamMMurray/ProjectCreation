#pragma once
#include "ECSTypes.h"
#include "IEntity.h"

template <typename T>
class Entity : public IEntity
{
    private:
        static const EntityTypeId m_TypeId;

    public:
        const EntityTypeId        GetStaticTypeId() const;
        static const EntityTypeId GetTypeId();
};
template <typename T>
const EntityTypeId Entity<T>::m_TypeId{TypeUtility<IEntity>::GetUniqueTypeId<T>()};

template <typename T>
inline const EntityTypeId Entity<T>::GetStaticTypeId() const
{
        return m_TypeId;
}

template <typename T>
inline const EntityTypeId Entity<T>::GetTypeId()
{
        static_assert(std::is_base_of<Entity<T>, T>::value, "GetTypeId can only accept CRTP classes derived from Entity<T>");
        return m_TypeId;
}