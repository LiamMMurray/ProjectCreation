#pragma once
#include <unordered_map>
#include "../../ECS/ECS.h"
#include "CollisionGrid.h"

class ComponentManager;

class CollisionSystem : public ISystem
{

        CollisionGrid m_Grid;
        // bool                     Moved(Shapes::ECollisionObjectTypes shapes);
        // std::vector<CollisionID> GetIDs(Shapes::ECollisionObjectTypes shapes);
        ComponentManager* m_ComponentManager;

    public:
        CollisionGrid GetCollisionGrid();

    protected:
        virtual void OnPreUpdate(float deltaTime) override;
        virtual void OnUpdate(float deltaTime) override;
        virtual void OnPostUpdate(float deltaTime) override;
        virtual void OnInitialize() override;
        virtual void OnShutdown() override;
        virtual void OnResume() override;
        virtual void OnSuspend() override;
};