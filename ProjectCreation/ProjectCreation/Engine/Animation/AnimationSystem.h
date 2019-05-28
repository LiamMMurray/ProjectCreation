#pragma once

#pragma once

#include "../../ECS/ECS.h"
#include "../../Utility/ForwardDeclarations/D3DNativeTypes.h"
#include "../../Utility/ForwardDeclarations/WinProcTypes.h"

#include "AnimationComponent.h"
#include "AnimationContainers.h"

class ResourceManager;

class AnimationSystem : public ISystem
{
        friend class ResourceManager;

        ComponentManager* m_ComponentManager;
        EntityManager*    m_EntityManager;
        ResourceManager*  m_ResourceManager;

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
