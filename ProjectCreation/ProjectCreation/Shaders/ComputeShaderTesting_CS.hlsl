#include "Math.hlsl"

static const unsigned int gMaxEmitterCount  = 2 << 10;
static const unsigned int gMaxParticleCount = 2 << 16;
struct FParticleGPU
{
        float4 position;
        float4 color;
        float3 velocity;
        float2 uv;
        float  time;
        bool   active;
        int    index;
};

cbuffer SceneInfoBuffer : register(b1)
{
        float3 _EyePosition;
        float  _Time;
        float3 _DirectionalLightDirection;
        float  _playerRadius;
        float3 _DirectionalLightColor;
        // float pad
        float3 _AmbientColor;
        float  _DeltaTime;
};

struct FEmitterGPU
{
        int    currentParticleCount;
        float  accumulatedTime;
        bool   active;
        float4 position;
        float4 color;
        float2 uv;
        float3 minInitialVelocity;
        float3 maxInitialVelocity;
};

struct FSegmentBuffer
{
        int index[gMaxEmitterCount];
};
RWStructuredBuffer<FParticleGPU>   ParticleBuffer : register(u0);
StructuredBuffer<FEmitterGPU>      EmitterBuffer : register(t0);
RWStructuredBuffer<FSegmentBuffer> SegmentBuffer : register(u1);

[numthreads(100, 1, 1)] void main(uint3 DTid
                                     : SV_DispatchThreadID) {
        int id = DTid.x;

        if (ParticleBuffer[id].time <= 0.0f)
        {
                ParticleBuffer[id].time = EmitterBuffer[0].accumulatedTime;

                float alphaA = rand(_Time * id/0.1f);
                float alphaB = rand(alphaA);
                float alphaC = rand(alphaB);

                ParticleBuffer[id].velocity.x =
                    lerp(EmitterBuffer[0].minInitialVelocity.x, EmitterBuffer[0].maxInitialVelocity.x, alphaA);
                ParticleBuffer[id].velocity.y =
                    lerp(EmitterBuffer[0].minInitialVelocity.y, EmitterBuffer[0].maxInitialVelocity.y, alphaB);
                ParticleBuffer[id].velocity.z =
                    lerp(EmitterBuffer[0].minInitialVelocity.z, EmitterBuffer[0].maxInitialVelocity.z, alphaC);

                ParticleBuffer[id].color = EmitterBuffer[0].color;

                ParticleBuffer[id].position =
                    float4(_EyePosition + ParticleBuffer[id].velocity * 1.0f, 1.0f); // EmitterBuffer[0].position;
        }
        else
        {
                ParticleBuffer[DTid.x].time -= _DeltaTime;
                ParticleBuffer[DTid.x].position += 1.0f*float4(ParticleBuffer[DTid.x].velocity * _DeltaTime, 0.0f);
        }
}