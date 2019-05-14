#include "PhysicsSystem.h"
#include "../Components/CTransformComponent.h"
#include "../Components/PhysicsComponent.h"
#include "../Engine/GEngine.h"

#include <DirectXMath.h>

void PhysicsSystem::OnPreUpdate(float deltaTime)
{}

void PhysicsSystem::OnUpdate(float deltaTime)
{
        using namespace DirectX;

        auto iter = GEngine::Get()->GetComponentManager()->GetActiveComponents<PhysicsComponent>();

        // for (iter == iter.begin(); iter != iter.end(); ++iter)
        {
			// CRITICAL_TODO: Implement iterator
                //PhysicsComponent*   currPhysics = nullptr;
                //IEntity*            currEntity  = GEngine::Get()->GetEntityManager()->GetEntity(currPhysics->GetOwner());
                //TransformComponent* currComponent = dynamic_cast<TransformComponent*>(currEntity->GetComponent<TransformComponent>());

                //currComponent->transform.translation = currComponent->transform.translation + currPhysics->GetVelocity() * deltaTime;

                //currPhysics->ApplyForce(currPhysics->GetForce());
        }
}

void PhysicsSystem::OnPostUpdate(float deltaTime)
{}

void PhysicsSystem::OnInitialize()
{
	m_Gravity = m_OneG;
}

void PhysicsSystem::OnShutdown()
{
        int activeCount = GEngine::Get()->GetComponentManager()->GetActiveComponentCount();

        // for (iter == iter.begin(); iter != iter.end(); ++iter)
        {
                // SafeRelease all active Physics Components
        }
}

void PhysicsSystem::OnResume()
{
        int activeCount = GEngine::Get()->GetComponentManager()->GetActiveComponentCount();

        // for (iter == iter.begin(); iter != iter.end(); ++iter)
        {
                m_Gravity = m_OneG;
        }
}

void PhysicsSystem::OnSuspend()
{
        int activeCount = GEngine::Get()->GetComponentManager()->GetActiveComponentCount();

        // for (iter == iter.begin(); iter != iter.end(); ++iter)
        {
                m_Gravity = m_ZeroG;
        }
}
