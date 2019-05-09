#pragma once
#include <stdint.h>
//class null_type
//{};

// Making TypeId a templated type prevents doing things like comparing
// TypeId (for a component) == TypeId (for an Entity)
// without writing individual inherited classes for every class we wish to have a typeId
//
// /*****(WE DONT HAVE TO DO THIS)*****/
// struct EntityTypeId : public ITypeId
//{
//        EntityTypeId() : ITypeId()
//        {}
//        EntityTypeId(ITypeId other) : ITypeId(other)
//        {}
//};
// struct ComponentTypeId : public ITypeId
//{
//        ComponentTypeId() : ITypeId()
//        {}
//        ComponentTypeId(ITypeId other) : ITypeId(other)
//        {}
//}
// /*****(INSTEAD WE DO THIS)*****/
// TypeId<IEntity> m_TypeId  // (member of IEntity)
// TypeId<IComponent> m_TypeId // (member of IComponent)
// etc...
template <typename T>
struct TypeId
{
        uint32_t m_Data;

        TypeId() : m_Data(0)
        {}
        TypeId(uint32_t data) : m_Data(data)
        {}

        TypeId operator++(int)
        {
                TypeId temp(*this);
                m_Data++;
                return temp;
        }
};

enum ERESULT_FLAG
{
        SUCCESS      = 0b00000000,
        PLACEHOLDER1 = 0b00000001,
        PLACEHOLDER2 = 0b00000010,
        PLACEHOLDER3 = 0b00000100,
        PLACEHOLDER4 = 0b00001000,
        PLACEHOLDER5 = 0b00010000,
        PLACEHOLDER6 = 0b00100000,
        PLACEHOLDER7 = 0b01000000,
        PLACEHOLDER8 = 0b10000000
};
struct EResult
{
        uint64_t m_Flags;
};