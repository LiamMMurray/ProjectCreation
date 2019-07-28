#include "ParticleData.h"
using namespace DirectX;
ParticleData::FEmitterCPU::FEmitterCPU()
{}

ParticleData::FEmitterCPU::FEmitterCPU(DirectX::XMFLOAT4 pos,
                                       DirectX::XMFLOAT4 color,
                                       DirectX::XMFLOAT3 vel,
                                       DirectX::XMFLOAT2 uv,
                                       float             t)
{
        spawnPosition = pos;
        spawnColor    = color;
        velocity      = vel;
        texture       = uv;
        time          = t;
}

void ParticleData::FEmitterGPU::Zero(FEmitterGPU& emitterGPU)
{
        emitterGPU.currentParticleCount = 0;
        emitterGPU.lifeSpan             = 0; // life time
        emitterGPU.active               = 0;
        emitterGPU.position             = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
        emitterGPU.initialColor         = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
        emitterGPU.finalColor           = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
        emitterGPU.uv                   = XMFLOAT2(0.0f, 0.0f);
        emitterGPU.minVelocity          = XMFLOAT3(0.0f, 0.0f, 0.0f);
        emitterGPU.maxVelocity          = XMFLOAT3(0.0f, 0.0f, 0.0f);
        emitterGPU.scale                = XMFLOAT2(0.0f, 0.0f); // min sclae and max scale
        emitterGPU.acceleration         = XMFLOAT3(0.0f, 0.0f, 0.0f);
        emitterGPU.index                = 0; // type of particles;
}

void ParticleData::FEmitterGPU::FloatParticle(FEmitterGPU&      emitterGPU,
                                              DirectX::XMFLOAT4 position,
                                              DirectX::XMFLOAT4 initialColor,
                                              DirectX::XMFLOAT4 finalColor)
{
        emitterGPU.currentParticleCount = 0;
        emitterGPU.lifeSpan             = 0; // life time
        emitterGPU.active               = 0;
        emitterGPU.position             = position;
        emitterGPU.initialColor         = initialColor;
        emitterGPU.finalColor           = finalColor;
        emitterGPU.uv                   = XMFLOAT2(0.0f, 0.0f);
        emitterGPU.minVelocity          = {-3.0f, -0.0f, -3.0f};
        emitterGPU.maxVelocity          = {3.0f, 6.0f, 3.0f};
        emitterGPU.scale                = XMFLOAT2(1.0f, 1.0f); // min sclae and max scale
        emitterGPU.acceleration         = XMFLOAT3(0.0f, 0.0f, 0.0f);
        emitterGPU.index                = 0; // type of particles;
}
