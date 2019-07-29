#include "EmitterComponent.h"
using namespace ParticleData;
using namespace DirectX;

void EmitterComponent::Zero()
{
        EmitterData.lifeSpan           = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
        EmitterData.flags              = 0;
        EmitterData.minOffset          = XMFLOAT3(0.0f, 0.0f, 0.0f);
        EmitterData.maxOffset          = XMFLOAT3(0.0f, 0.0f, 0.0f);
        EmitterData.initialColor       = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
        EmitterData.finalColor         = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
        EmitterData.uv                 = XMFLOAT2(0.0f, 0.0f);
        EmitterData.minInitialVelocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
        EmitterData.maxInitialVelocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
        EmitterData.particleScale      = XMFLOAT2(0.0f, 0.0f); // min sclae and max scale
        EmitterData.acceleration       = XMFLOAT3(0.0f, 0.0f, 0.0f);
        EmitterData.index              = 0; // type of particles;
}

void EmitterComponent::FloatParticle(DirectX::XMFLOAT3 minOffset,
                                     DirectX::XMFLOAT3 maxOffset,
                                     DirectX::XMFLOAT4 initialColor,
                                     DirectX::XMFLOAT4 finalColor,
                                     DirectX::XMFLOAT4 lifeSpan)
{

        EmitterData.lifeSpan           = lifeSpan; // life time
        EmitterData.flags              = 1;
        EmitterData.minOffset          = minOffset;
        EmitterData.maxOffset          = maxOffset;
        EmitterData.initialColor       = initialColor;
        EmitterData.finalColor         = finalColor;
        EmitterData.uv                 = XMFLOAT2(0.0f, 0.0f);
        EmitterData.minInitialVelocity = {-3.0f, -0.0f, -3.0f};
        EmitterData.maxInitialVelocity = {3.0f, 6.0f, 3.0f};
        EmitterData.particleScale      = XMFLOAT2(1.0f, 1.0f); // min sclae and max scale
        EmitterData.acceleration       = XMFLOAT3(0.0f, 0.0f, 0.0f);
        EmitterData.index              = 0; // type of particles;
}
