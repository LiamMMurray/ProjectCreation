#include "Math.hlsl"
#include "Samplers.hlsl"
#include "SceneBuffer.hlsl"
#include "Wrap.hlsl"
static const unsigned int gMaxEmitterCount  = 2 << 10;
static const unsigned int gMaxParticleCount = 2 << 16;
static const unsigned int gMaxParticlePerEmitter = gMaxParticleCount / gMaxEmitterCount;

struct FParticleGPU
{
        float4 position;
        float4 prevPos;
        float4 color;
        float3 velocity;
        float2 uv;
        float  time;
        int    active;
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
        float3 direction;
        int    id = DTid.x;
        int    emitterIndex = id / gMaxParticlePerEmitter;

        if (ParticleBuffer[id].time > 0.0f)
        {
                float depth = tex2d.SampleLevel(sampleTypeClamp, EmitterBuffer[emitterIndex].uv, 0).r * 5.0f; // heigth mapset up

                float wValue = 1.0f;

                float alpha              = 1.0f - ParticleBuffer[id].time / EmitterBuffer[emitterIndex].lifeSpan;
                ParticleBuffer[id].color = lerp(EmitterBuffer[emitterIndex].initialColor, EmitterBuffer[emitterIndex].finalColor, alpha);
                // ParticleBuffer[id].scale = lerp(EmitterBuffer[emitterIndex].particleScale.x,EmitterBuffer[emitterIndex].particleScale.y, alpha);
                ParticleBuffer[DTid.x].time -= _DeltaTime;

                // ParticleBuffer[DTid.x].position += 1.0f*float4(ParticleBuffer[DTid.x].velocity * _DeltaTime,
                // 0.0f);

                float2 Min = float2(-0.5f * _Scale, -0.5f * _Scale);
                // Min        = float2(-10.0f, -10.0f);
                float2 Max = -Min;
                // ParticleBuffer[id].position.xyz =
                //  WrapPosition(ParticleBuffer[id].position.xyz, _EyePosition + Min, _EyePosition + Max);
                ParticleBuffer[id].position.xz =
                    wrap(ParticleBuffer[id].position.xz, _EyePosition.xz + Min, _EyePosition.xz + Max);

                ParticleBuffer[id].acceleration = EmitterBuffer[emitterIndex].acceleration;
                ParticleBuffer[id].prevPos      = ParticleBuffer[id].position;


                ParticleBuffer[id].position =
                    ParticleBuffer[id].position + float4(ParticleBuffer[id].velocity, 1.0f) * _DeltaTime;
                ParticleBuffer[id].velocity = ParticleBuffer[id].velocity + ParticleBuffer[id].acceleration * _DeltaTime;

                // bounce based on texture depth
                if (ParticleBuffer[id].position.y <= depth)
                {
                        ParticleBuffer[id].velocity = float3(3.0f, 10.0f, 1.0f);

                        ParticleBuffer[id].position =
                            ParticleBuffer[id].position + float4(ParticleBuffer[id].velocity, 1.0f) * _DeltaTime;
                }
        }
}

float3 WorldPositionFromDepth(float depth, float2 uv)
{
        float x = uv.x * 2.0f - 1.0f;
        float y = (1 - uv.y) * 2.0f - 1.0f;

        float4 clipSpacePosition = float4(x, y, depth, 1.0f);
        // float4 clipSpacePosition = float4(uv * 2.0f - 1.0f, z, 1.0f);
        float4 viewSpacePosition = mul(clipSpacePosition, _invProj);
        return viewSpacePosition.xyz /= viewSpacePosition.w;
        /* float4 worldPosition = mul(viewSpacePosition, _invView);

         return worldPosition.xyz;*/
}