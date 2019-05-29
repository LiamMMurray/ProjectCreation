#pragma once
#include <unordered_map>
#include "CollisionLibary.h"
#include"../../ECS/ECS.h"
class CollisionSystem : public ISystem
{
    public:
        static struct CollisionID
        {
                DirectX::XMVECTOR position;
                int               idex;
        };

    private:
        std::unordered_map<CollisionID, std::vector<std::pair<int, ComponentHandle>>> collisionMap;
    public:
        bool                     Moved(Collision::ECollisionObjectTypes shapes);
        std::vector<CollisionID> GetIDs(Collision::ECollisionObjectTypes shapes);

        // Inherited via ISystem
        virtual void OnPreUpdate(float deltaTime) override;
        virtual void OnUpdate(float deltaTime) override;
        virtual void OnPostUpdate(float deltaTime) override;
        virtual void OnInitialize() override;
        virtual void OnShutdown() override;
        virtual void OnResume() override;
        virtual void OnSuspend() override;
};