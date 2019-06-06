#pragma once
#include <unordered_map>
#include "../../ECS/ECS.h"
#include "CollisionGrid.h"
#include "CollisionResult.h"
#include "CollisionLibary.h"
class CollisionSystem : public ISystem
{
        

        CollisionGrid m_Grid;

    public:
        //bool                     Moved(Shapes::ECollisionObjectTypes shapes);
        //std::vector<CollisionID> GetIDs(Shapes::ECollisionObjectTypes shapes);

        // Inherited via ISystem
        CollisionGrid GetCollisionGrid();
        virtual void OnPreUpdate(float deltaTime) override;
        virtual void OnUpdate(float deltaTime) override;
        virtual void OnPostUpdate(float deltaTime) override;
        virtual void OnInitialize() override;
        virtual void OnShutdown() override;
        virtual void OnResume() override;
        virtual void OnSuspend() override;
};