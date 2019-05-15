#pragma once
#include <stdint.h>
#include <functional>
#include "Handle.h"

namespace std
{
        template <class _Kty>
        struct hash;

        template <class _Ty = void>
        struct equal_to;

        template <class _Ty>
        class allocator;

        template <class _Ty1, class _Ty2>
        struct pair;

        template <class _Kty,
                  class _Ty,
                  class _Hasher = hash<_Kty>,
                  class _Keyeq  = equal_to<_Kty>,
                  class _Alloc  = allocator<pair<const _Kty, _Ty>>>
        class unordered_map;
} // namespace std

template <typename T>
struct ECSTypeId
{
        uint32_t m_Data;

        ECSTypeId() : m_Data(0)
        {}

        ECSTypeId(uint32_t data) : m_Data(data)
        {}

        ECSTypeId operator++(int)
        {
                ECSTypeId temp(*this);
                m_Data++;
                return temp;
        }
        bool operator==(const ECSTypeId<T>& other) const
        {
                return this->m_Data == other.m_Data;
        }
		bool operator!=(const ECSTypeId<T>& other) const
		{
                return this->m_Data != other.m_Data;
		}
};

template <typename T>
class std::hash<ECSTypeId<T>>
{
    public:
        size_t operator()(const ECSTypeId<T> id) const
        {
                return hash<uint32_t>()(id.m_Data);
        }
};

class IComponent;
class IEntity;
class ISystem;

typedef Handle<IComponent>    ComponentHandle;
typedef Handle<IEntity>       EntityHandle;
typedef ECSTypeId<IComponent> ComponentTypeId;
typedef ECSTypeId<IEntity>    EntityTypeId;
typedef ECSTypeId<ISystem>    SystemTypeId;