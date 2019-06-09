#pragma once
#include "../ECS/ECS.h"

#include"ParticleBufferSetup.h"
class ParticleSystem : public ISystem
{

       


    public:
        void Initialize();
        void Render();
        void Shutdown();

        // Inherited via ISystem
        virtual void OnPreUpdate(float deltaTime) override;
        virtual void OnUpdate(float deltaTime) override;
        virtual void OnPostUpdate(float deltaTime) override;
        virtual void OnInitialize() override;
        virtual void OnShutdown() override;
        virtual void OnResume() override;
        virtual void OnSuspend() override;
};
