#pragma once

#include <HandleManager.h>

#include <AnimationClip.h>

class AnimationComponent : public Component<AnimationComponent>
{
        friend class AnimationSystem;

        double                      m_Time = 0.0f;
        std::vector<ResourceHandle> m_Clips;
        std::vector<float>          m_Weights;

    public:
        void SetWeights(int count, float* weights);
        void AddAnims(size_t count, float* weights, ResourceHandle* animHandles);
};
