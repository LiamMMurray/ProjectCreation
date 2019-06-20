#include "IPoolElement.h"

EntityHandle IPoolElement::GetParent()
{
        return EntityHandle(m_parent_redirection_index);
}

IPoolElement::~IPoolElement()
{}
