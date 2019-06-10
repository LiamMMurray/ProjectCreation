#include "CollisionSystem.h"
#include "../GEngine.h"
#include "CollisionComponents.h"
#include "CollisionHelpers.h"
using namespace std;

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
        if (m_ComponentManager->ComponentsExist<SphereComponent>())
        {
                auto itr = m_ComponentManager->GetActiveComponents<SphereComponent>();
                for (auto i = itr.begin(); i != itr.end(); i++)
                {
                        SphereComponent*         sphereComp = static_cast<SphereComponent*>(i.data());
                        std::vector<CollisionID> IDs        = m_Grid.GetPossibleCollisions(&sphereComp->sphere);


                }
        }

        if (m_ComponentManager->ComponentsExist<AABBComponent>())
        {
                auto itr = m_ComponentManager->GetActiveComponents<AABBComponent>();
                for (auto i = itr.begin(); i != itr.end(); i++)
                {
                        AABBComponent*           aabbComp = static_cast<AABBComponent*>(i.data());
                        std::vector<CollisionID> IDs      = m_Grid.GetPossibleCollisions(&aabbComp->aabb);
                }
        }

        if (m_ComponentManager->ComponentsExist<CapsuleComponent>())
        {
                auto itr = m_ComponentManager->GetActiveComponents<CapsuleComponent>();
                for (auto i = itr.begin(); i != itr.end(); i++)
                {
                        CapsuleComponent* capsuleComp = static_cast<CapsuleComponent*>(i.data());
                        m_Grid.GetPossibleCollisions(&capsuleComp->capsule);
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
