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
        int    currentParticleCount;
        float  lifeSpan;
        bool   active;
        float4 position;
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
cbuffer CScreenSpaceBuffer : register(b0)
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

[numthreads(512, 1, 1)] void main(uint3 DTid
                                  : SV_DispatchThreadID) {
        int id            = DTid.x;
        int emitterIndex  = id / gMaxParticlePerEmitter;
        int emitterOffset = id % gMaxParticlePerEmitter;
        int desired       = SegmentBuffer[emitterIndex].desiredCount;

        if (emitterOffset < desired)
        {
                if (ParticleBuffer[id].time <= 0.0f)
                {
                        ParticleBuffer[id].scale = EmitterBuffer[emitterIndex].particleScale.x;
                        ParticleBuffer[id].time  = EmitterBuffer[emitterIndex].lifeSpan;
                        float4 startPos          = EmitterBuffer[emitterIndex].position;

                        float alphaA = rand(_Time * id / 0.1f);
                        float alphaB = rand(alphaA);
                        float alphaC = rand(alphaB);

                        ParticleBuffer[id].velocity.x =
                            lerp(EmitterBuffer[emitterIndex].minInitialVelocity.x, EmitterBuffer[emitterIndex].maxInitialVelocity.x, alphaA);
                        ParticleBuffer[id].velocity.y =
                            lerp(EmitterBuffer[emitterIndex].minInitialVelocity.y, EmitterBuffer[emitterIndex].maxInitialVelocity.y, alphaB);
                        ParticleBuffer[id].velocity.z =
                            lerp(EmitterBuffer[emitterIndex].minInitialVelocity.z, EmitterBuffer[emitterIndex].maxInitialVelocity.z, alphaC);

                        ParticleBuffer[id].uv    = EmitterBuffer[emitterIndex].uv;
                        ParticleBuffer[id].color = EmitterBuffer[emitterIndex].initialColor;

                        ParticleBuffer[id].position = float4(startPos + ParticleBuffer[id].velocity, 1.0f);

                        ParticleBuffer[id].prevPos  = ParticleBuffer[id].position;
                        ParticleBuffer[id].position = ParticleBuffer[id].prevPos;
                }
                else
                {
                        ParticleBuffer[id].time -= _DeltaTime;
                }
        }
        else
        {
                ParticleBuffer[id].time = 0.0f;
        }
}
