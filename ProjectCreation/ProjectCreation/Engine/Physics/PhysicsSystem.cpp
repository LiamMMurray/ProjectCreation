#include "PhysicsSystem.h"
#include <iostream>
#include "../GEngine.h"

#include "../Controller/PlayerMovement.h"

#include <DirectXMath.h>

void PhysicsSystem::OnPreUpdate(float deltaTime)
{}

void PhysicsSystem::OnUpdate(float deltaTime)
{
        return;
        using namespace DirectX;

        //auto iter = GEngine::Get()->GetComponentManager()->GetActiveComponents<PhysicsComponent>();


        // for (iter == iter.begin(); iter != iter.end(); ++iter)
        {

			//Testing use of static PhysicsComponent
                //PhysicsComponent* currPhysics = &PlayerMovement::physicsComponent;
				//
                //TransformComponent*       currComponent = &PlayerMovement::transformComponent;

                //IEntity*            currEntity = GEngine::Get()->GetEntityManager()->GetEntity(currPhysics->GetOwner());
                //TransformComponent* currComponent =
                //    dynamic_cast<TransformComponent*>(currEntity->GetComponent<TransformComponent>());

				//Sweep Sphere

				//Set startA to currComponent->transform.translation before adding velocity
                //currComponent->transform.translation = XMVectorLerp(currComponent->transform.translation, currPhysics->GetVelocity(), ) * deltaTime;
				//Set endA to currComponent->transform.translation after adding velocity

				//Check Sweep
				//if no collision apply force
                //currPhysics->ApplyForce(currPhysics->GetForce());

				//else don't add movement

                //std::cout << "Current Position < " << XMVectorGetX(currComponent->transform.translation) << ", "
                //          << XMVectorGetY(currComponent->transform.translation) << ", "
                //          << XMVectorGetZ(currComponent->transform.translation) << ", "
                //          << XMVectorGetW(currComponent->transform.translation) << " >" << std::endl;

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
        //int activeCount = GEngine::Get()->GetComponentManager()->GetActiveComponentCount();

        // for (iter == iter.begin(); iter != iter.end(); ++iter)
        {
                // SafeRelease all active Physics Components
        }
}

void PhysicsSystem::OnResume()
{
        //int activeCount = GEngine::Get()->GetComponentManager()->GetActiveComponentCount();

        // for (iter == iter.begin(); iter != iter.end(); ++iter)
        {
                m_Gravity = m_OneG;
        }
}

void PhysicsSystem::OnSuspend()
{
        //int activeCount = GEngine::Get()->GetComponentManager()->GetActiveComponentCount();

        // for (iter == iter.begin(); iter != iter.end(); ++iter)
        {
                m_Gravity = m_ZeroG;
        }
}
