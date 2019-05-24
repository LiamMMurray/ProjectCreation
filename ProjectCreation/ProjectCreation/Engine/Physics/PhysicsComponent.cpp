#include "PhysicsComponent.h"
#include "../GenericComponents/TransformComponent.h"
#include "../../ECS/Entity.h"
#include "../GEngine.h"

void PhysicsComponent::AddForce(DirectX::XMVECTOR force)
{
        m_Force = force;
}

void PhysicsComponent::ApplyForce(DirectX::XMVECTOR force)
{
        m_Velocity = force;
}

DirectX::XMVECTOR PhysicsComponent::GetForce()
{
        return m_Force;
}

void PhysicsComponent::SetMass(float _Mass)
{
        m_Mass = _Mass;
}

float PhysicsComponent::GetMass()
{
        return m_Mass;
}

void PhysicsComponent::SetInverseMass(float _Mass)
{
        m_InverseMass = (1 / _Mass);
}

float PhysicsComponent::GetInverseMass()
{
        return m_InverseMass;
}

void PhysicsComponent::SetVelocity(DirectX::XMVECTOR _Velocity)
{}

DirectX::XMVECTOR PhysicsComponent::GetVelocity()
{
        return m_Velocity;
}

bool PhysicsComponent::UsesGravity()
{
        return false;
}
