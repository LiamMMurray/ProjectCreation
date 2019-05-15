#pragma once

#pragma once

#include "../../ECS/ECS.h"
#include "../../Utility/ForwardDeclarations/D3DNativeTypes.h"
#include "../../Utility/ForwardDeclarations/WinProcTypes.h"

#include "AnimationComponent.h"

class AnimationSystem : public ISystem
{
        friend class ResourceManager;


    protected:
        virtual void OnPreUpdate(float deltaTime) override;
        virtual void OnUpdate(float deltaTime) override;
        virtual void OnPostUpdate(float deltaTime) override;
        virtual void OnInitialize() override;
        virtual void OnShutdown() override;
        virtual void OnResume() override;
        virtual void OnSuspend() override;

    public:
};
