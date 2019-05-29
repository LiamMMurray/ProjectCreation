#include "CollisionSystem.h"
using namespace std;

std::vector<CollisionSystem::CollisionID> CollisionSystem::GetIDs(Collision::ECollisionObjectTypes shapes)
{	
        vector<CollisionID> output;
        switch (shapes)
        {
                case Collision::Sphere:

                        break;
                case Collision::Aabb:
                        break;
                case Collision::Plane:
                        break;
                case Collision::Capsule:
                        break;
                default:
                        break;
        }
        return output;
}

void CollisionSystem::OnPreUpdate(float deltaTime)
{
		
}

void CollisionSystem::OnUpdate(float deltaTime)
{}

void CollisionSystem::OnPostUpdate(float deltaTime)
{}

void CollisionSystem::OnInitialize()
{}

void CollisionSystem::OnShutdown()
{}

void CollisionSystem::OnResume()
{}

void CollisionSystem::OnSuspend()
{}
