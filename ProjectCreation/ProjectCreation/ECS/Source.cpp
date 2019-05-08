#include "Entity.h"
int main()
{
        Entity<float> b;
        auto          test  = Component<float>::m_TypeId;
        auto          test2 = Component<int>::m_TypeId;
        auto          test3 = Entity<float>::m_TypeId;
        int pause = 0;
        return 0;
}