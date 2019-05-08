#pragma once
#include "ECS.h"

void test()
{
        auto test_0 = Component<int>::m_TypeId;
        auto test_1 = Component<int>::m_TypeId;
        auto test_2 = Component<char>::m_TypeId;
        auto test_3 = Component<int>::m_TypeId;
        auto test_4 = Component<float>::m_TypeId;
}