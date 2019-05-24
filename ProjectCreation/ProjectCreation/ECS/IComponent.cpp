#include "IComponent.h"

EntityHandle IComponent::GetOwner()
{
        return m_Owner;
}

ComponentHandle IComponent::GetHandle()
{
        return m_Handle;
}

void IComponent::Enable()
{
        m_Enabled = true;
}
void IComponent::Disable()
{
        m_Enabled = false;
}
void IComponent::SetActive(bool state)
{
        m_Enabled = state;
}
bool IComponent::IsEnabled()
{
        return m_Enabled;
}
