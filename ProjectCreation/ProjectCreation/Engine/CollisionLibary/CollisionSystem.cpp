#include "CollisionSystem.h"
#include "../GEngine.h"
#include "../GenericComponents/TransformComponent.h"
#include "CollisionComponents.h"
#include "CollisionHelpers.h"
#include "CollisionLibary.h"
using namespace std;
using namespace Collision;
CollisionGrid CollisionSystem::GetCollisionGrid()
{
        return m_Grid;
}

// Before update
void CollisionSystem::OnPreUpdate(float deltaTime)
{}

// update
void CollisionSystem::OnUpdate(float deltaTime)
{
        // Upate sphere component
        if (m_ComponentManager->ComponentsExist<SphereComponent>())
        {

                auto itr = m_ComponentManager->GetActiveComponents<SphereComponent>();
                for (auto i = itr.begin(); i != itr.end(); i++)
                {
                        SphereComponent* sphereComp = static_cast<SphereComponent*>(i.data());

                        // get collision query
                        auto                query = m_Grid.GetPossibleCollisions(&sphereComp->sphere);
                        TransformComponent* transComp =
                            m_ComponentManager->GetComponent<TransformComponent>(sphereComp->GetOwner());
                        sphereComp->sphere.center = transComp->transform.translation;
                }
        }
        if (m_ComponentManager->ComponentsExist<AABBComponent>())
        {

                auto itr = m_ComponentManager->GetActiveComponents<AABBComponent>();
                for (auto i = itr.begin(); i != itr.end(); i++)
                {
                        AABBComponent* aabbComp = static_cast<AABBComponent*>(i.data());

                        // get collision query
                        auto                query = m_Grid.GetPossibleCollisions(&aabbComp->aabb);
                        TransformComponent* transComp =
                            m_ComponentManager->GetComponent<TransformComponent>(aabbComp->GetOwner());
                        aabbComp->aabb.center = transComp->transform.translation;
                }
        }
}

// after update
void CollisionSystem::OnPostUpdate(float deltaTime)
{}

// on start
void CollisionSystem::OnInitialize()
{
        m_ComponentManager = GEngine::Get()->GetComponentManager();
}

// on game end. release memory here if using
void CollisionSystem::OnShutdown()
{}

void CollisionSystem::OnResume()
{}

void CollisionSystem::OnSuspend()
{}
