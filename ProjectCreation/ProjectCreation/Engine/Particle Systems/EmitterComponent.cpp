#include "EmitterComponent.h"
using namespace ParticleData;
using namespace DirectX;

void EmitterComponent::Zero(FEmitterGPU& emitterGPU)
{
        emitterGPU.lifeSpan        = XMFLOAT3(0.0f, 0.0f, 0.0f);
        emitterGPU.flags           = 0;
        emitterGPU.initialPosition = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
        emitterGPU.endPosition     = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
        emitterGPU.initialColor    = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
        emitterGPU.finalColor      = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
        emitterGPU.uv              = XMFLOAT2(0.0f, 0.0f);
        emitterGPU.minVelocity     = XMFLOAT3(0.0f, 0.0f, 0.0f);
        emitterGPU.maxVelocity     = XMFLOAT3(0.0f, 0.0f, 0.0f);
        emitterGPU.scale           = XMFLOAT2(0.0f, 0.0f); // min sclae and max scale
        emitterGPU.acceleration    = XMFLOAT3(0.0f, 0.0f, 0.0f);
        emitterGPU.index           = 0; // type of particles;
}

void EmitterComponent::FloatParticle(FEmitterGPU&      emitterGPU,
                                              DirectX::XMFLOAT4 initPosition,
                                              DirectX::XMFLOAT4 endPosition,
                                              DirectX::XMFLOAT4 initialColor,
                                              DirectX::XMFLOAT4 finalColor,
                                              XMFLOAT3          lifeSpan)
{

        emitterGPU.lifeSpan        = lifeSpan; // life time
        emitterGPU.flags           = 1;
        emitterGPU.initialPosition = initPosition;
        emitterGPU.endPosition     = endPosition;
        emitterGPU.initialColor    = initialColor;
        emitterGPU.finalColor      = finalColor;
        emitterGPU.uv              = XMFLOAT2(0.0f, 0.0f);
        emitterGPU.minVelocity     = {-3.0f, -0.0f, -3.0f};
        emitterGPU.maxVelocity     = {3.0f, 6.0f, 3.0f};
        emitterGPU.scale           = XMFLOAT2(1.0f, 1.0f); // min sclae and max scale
        emitterGPU.acceleration    = XMFLOAT3(0.0f, 0.0f, 0.0f);
        emitterGPU.index           = 0; // type of particles;
}
