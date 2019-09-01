#pragma once
#include "IResource.h"
#include <TypeIndexFactory.h>
typedef ECSTypeId<IResource> ResourceTypeID;


template <typename ResourceType>
class Resource : public IResource
{
        template <typename T>
        friend class ResourceContainer;

        static const ResourceTypeID m_TypeId;

    public:
        static const ResourceTypeID GetTypeID();
};

template <class T>
const ResourceTypeID Resource<T>::m_TypeId = TypeIndexFactory<IResource>::GetTypeIndex<T>();

template <typename T>
inline const ResourceTypeID Resource<T>::GetTypeID()
{
        static_assert(std::is_base_of<Resource<T>, T>::value, "Resource must follow derive from Resource<T>");

        return m_TypeId;
}