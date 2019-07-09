#pragma once

#include "..//..//ECS/ECS.h"

class TransformSystem : public ISystem
{
        ComponentHandle playerTransform;
        HandleManager*  m_HandleManager;
        // Inherited via ISystem
        virtual void OnPreUpdate(float deltaTime) override;
        virtual void OnUpdate(float deltaTime) override;
        virtual void OnPostUpdate(float deltaTime) override;
        virtual void OnInitialize() override;
        virtual void OnShutdown() override;
        virtual void OnResume() override;
        virtual void OnSuspend() override;
};