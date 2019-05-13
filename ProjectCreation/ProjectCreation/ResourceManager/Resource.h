#pragma once
#include "IResource.h"

typedef ECSTypeId<IResource> ResourceTypeID;

template <typename ResourceType>
class Resource : public IResource
{
        static const ResourceTypeID m_TypeId;


        static const ResourceTypeID GetTypeID();
};

template <class T>
const ResourceTypeID Resource<T>::m_TypeId = TypeIdCreator<IResource>::GetUniqueTypeId<T>();

template <typename T>
inline const ResourceTypeID Resource<T>::GetTypeID()
{
        static_assert(std::is_base_of<Resource<T>, T>::value, "Resource must follow derive from Resource<T>");

        return m_TypeId;
}