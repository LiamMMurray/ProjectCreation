#include "ParticleData.h"

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
