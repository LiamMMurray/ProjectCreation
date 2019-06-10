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
        template <typename T>
		void GetCollisionComponent()
		{
                if (m_ComponentManager->ComponentsExist<T>())
                {
                        auto itr = m_ComponentManager->GetActiveComponents<T>();
                        for (auto i = itr.begin(); i != itr.end(); i++)
                        {
                                T* comp = static_cast<T*>(i.data());
                        }
                }
		}

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