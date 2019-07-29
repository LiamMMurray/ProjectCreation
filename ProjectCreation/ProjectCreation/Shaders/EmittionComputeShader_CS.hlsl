#include "Math.hlsl"
#include "Samplers.hlsl"
#include "SceneBuffer.hlsl"
#include "Wrap.hlsl"
static const unsigned int gMaxEmitterCount       = 2 << 10;
static const unsigned int gMaxParticleCount      = 2 << 16;
static const unsigned int gMaxParticlePerEmitter = gMaxParticleCount / gMaxEmitterCount;

struct FParticleGPU
{
        float4 position;
        float4 prevPos;
        float4 color;
        float3 velocity;
        float2 uv;
        float  time;
        int    emitterIndex;
        float  scale;
        float3 acceleration;
};


struct FEmitterGPU
{
        float3 lifeSpan; // x is life time, y,z is fed in ad out
        int    flags;
        float3 emitterPosition;
        float3 minOffset;
        float3 maxOffset;
        float4 initialColor;
        float4 finalColor;
        float2 uv;
        float3 minInitialVelocity;
        float3 maxInitialVelocity;
        float2 particleScale;
        float3 acceleration;
        int    index; // tpye of particles
};

struct FSegmentBuffer
{
        int desiredCount;
};
cbuffer CScreenSpaceBuffer : register(b2)
{
        matrix _invProj;
        matrix _invView;
        float3 _playerPosition;
        float  _time;
};

RWStructuredBuffer<FParticleGPU> ParticleBuffer : register(u0);
StructuredBuffer<FEmitterGPU>    EmitterBuffer : register(t0);
Texture2D                        tex2d : register(t1);
StructuredBuffer<FSegmentBuffer> SegmentBuffer : register(t2);

float3 RandomFloat3InRange(inout float seed, float3 min, float3 max)
{
        float alphaA = rand(seed);
        float alphaB = rand(alphaA);
        float alphaC = rand(alphaB);
        seed         = alphaC;

        float3 output;
        output.x = lerp(min.x, max.x, alphaA);
        output.y = lerp(min.y, max.y, alphaB);
        output.z = lerp(min.z, max.z, alphaC);

        return output;
}

[numthreads(512, 1, 1)] void main(uint3 DTid
                                  : SV_DispatchThreadID) {
        int id            = DTid.x;
        int emitterIndex  = id / gMaxParticlePerEmitter;
        int emitterOffset = id % gMaxParticlePerEmitter;
        int desired       = SegmentBuffer[emitterIndex].desiredCount;

        if (ParticleBuffer[id].time <= 0.0f)
        {
                if (emitterOffset < desired)
                {
                        ParticleBuffer[id].scale = EmitterBuffer[emitterIndex].particleScale.x;
                        ParticleBuffer[id].time  = EmitterBuffer[emitterIndex].lifeSpan.x;
                        float4 startPos          = EmitterBuffer[emitterIndex].emitterPosition;

                        float seed                  = _Time + id * 18.1999464f;
                        ParticleBuffer[id].velocity = RandomFloat3InRange(seed,
                                                                          EmitterBuffer[emitterIndex].minInitialVelocity,
                                                                          EmitterBuffer[emitterIndex].maxInitialVelocity);

                        ParticleBuffer[id].uv    = EmitterBuffer[emitterIndex].uv;
                        ParticleBuffer[id].color = EmitterBuffer[emitterIndex].initialColor;

                        float3 offset = RandomFloat3InRange(
                            seed, EmitterBuffer[emitterIndex].minOffset, EmitterBuffer[emitterIndex].maxOffset);
                        ParticleBuffer[id].position = float4(startPos + offset, 1.0f);

                        ParticleBuffer[id].prevPos = ParticleBuffer[id].position;
                }
        }
        else
        {
                ParticleBuffer[id].time -= _DeltaTime;
        }
}
