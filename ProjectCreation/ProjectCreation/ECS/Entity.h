#pragma once
#include "ECSTypes.h"
#include "IEntity.h"

template <typename T>
class Entity : public IEntity
{
    private:
        static const TypeId<IEntity> m_TypeId;

    public:
        static const TypeId<IEntity> GetTypeId();
};
template <typename T>
const TypeId<IEntity> Entity<T>::m_TypeId{TypeIdCreator<IEntity>::GetUniqueTypeId<T>()};

template <typename T>
inline const TypeId<IEntity> Entity<T>::GetTypeId()
{
        return m_TypeId;
}