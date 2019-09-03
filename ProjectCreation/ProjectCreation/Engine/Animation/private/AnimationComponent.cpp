#include "AnimationComponent.h"

#include <MathLibrary.h>
#include <assert.h>

void AnimationComponent::SetWeights(int count, float* weights)
{
        assert(count <= m_Weights.size());

        for (int i = 0; i < count; ++i)
        {
                m_Weights[i] = weights[i];
        }

        for (int i = count; i < m_Weights.size(); ++i)
        {
                m_Weights[i] = 0.0f;
        }
}

void AnimationComponent::AddAnims(size_t count, float* weights, ResourceHandle* animHandles)
{
        m_Clips.insert(m_Clips.begin(), animHandles, animHandles + count);
        m_Weights.insert(m_Weights.begin(), weights, weights + count);

        MathLibrary::NormalizeArray(m_Weights.size(), m_Weights.data());
}
