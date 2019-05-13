#pragma once
#include "ECSTypes.h"
#include "IEntity.h"

template <typename T>
class Entity : public IEntity
{
    private:
        static const TypeId<IEntity> m_TypeId;

    public:
        const TypeId<IEntity>        GetStaticTypeId() const;
        static const TypeId<IEntity> GetTypeId();
};
template <typename T>
const TypeId<IEntity> Entity<T>::m_TypeId{TypeIdCreator<IEntity>::GetUniqueTypeId<T>()};

template <typename T>
inline const TypeId<IEntity> Entity<T>::GetStaticTypeId() const
{
        return m_TypeId;
}

template <typename T>
inline const TypeId<IEntity> Entity<T>::GetTypeId()
{
        static_assert(std::is_base_of<Entity<T>, T>::value, "GetTypeId can only accept CRTP classes derived from Entity<T>");
        return m_TypeId;
}