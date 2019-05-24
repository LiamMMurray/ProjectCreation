#include "AnimationComponent.h"
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
