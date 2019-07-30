#pragma once

#pragma once

#include <DirectXMath.h>
#include "../../ECS/HandleManager.h"
#include "../GEngine.h"

class ContinousSoundSystem : public ISystem
{
        // Inherited via ISystem
        virtual void OnPreUpdate(float deltaTime) override;
        virtual void OnUpdate(float deltaTime) override;
        virtual void OnPostUpdate(float deltaTime) override;
        virtual void OnInitialize() override;
        virtual void OnShutdown() override;
        virtual void OnResume() override;
        virtual void OnSuspend() override;
};