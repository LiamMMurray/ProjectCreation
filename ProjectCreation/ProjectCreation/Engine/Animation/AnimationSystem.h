#pragma once

#include <HandleManager.h>
#include <SystemManager.h>
#include <D3DNativeTypes.h>
#include <WinProcTypes.h>

#include "AnimationComponent.h"
#include "AnimationContainers.h"

class ResourceManager;

class AnimationSystem : public ISystem
{
        friend class ResourceManager;

        HandleManager*   m_HandleManager;
        ResourceManager* m_ResourceManager;

        void calcTransforms(Animation::FJoint*          sumVec,
                            int                         jointCount,
                            double                      time,
                            const Animation::FAnimClip& animClip,
                            float                       prevweight,
                            float                       currweight);

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
