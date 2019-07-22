#include "Math.hlsl"
#include "SceneBuffer.hlsl"
#include "Wrap.hlsl"
static const unsigned int gMaxEmitterCount  = 2 << 10;
static const unsigned int gMaxParticleCount = 2 << 16;
struct FParticleGPU
{
        float4 position;
        float4 prevPos;
        float4 color;
        float3 velocity;
        float2 uv;
        float  time;
        bool   active;
        int    index;
        float  scale;
};


struct FEmitterGPU
{
        int    currentParticleCount;
        float  accumulatedTime;
        bool   active;
        float4 position;
        float4 initialColor;
        float4 finalColor;
        float2 uv;
        float3 minInitialVelocity;
        float3 maxInitialVelocity;
        float2 particleScale;
};

struct FPhysicParticle // only for collision for now
{
        float  radius;
        float3 center;
};
RWStructuredBuffer<FParticleGPU>    ParticleBuffer : register(u0);
StructuredBuffer<FEmitterGPU>       EmitterBuffer : register(t0);
RWStructuredBuffer<FPhysicParticle> PhyParticleBuffer : register(u1);

[numthreads(100, 1, 1)] void main(uint3 DTid
                                  : SV_DispatchThreadID) {
        int id = DTid.x;


        if (ParticleBuffer[id].time <= 0.0f)
        {
                ParticleBuffer[id].scale = rand_1_05(EmitterBuffer[0].particleScale);
                ParticleBuffer[id].time  = EmitterBuffer[0].accumulatedTime + 1000.0f;
                float4 startPos          = EmitterBuffer[0].position;

                ParticleBuffer[id].prevPos = startPos;

                float alphaA = rand(_Time * id / 0.1f);
                float alphaB = rand(alphaA);
                float alphaC = rand(alphaB);

                ParticleBuffer[id].velocity.x =
                    lerp(EmitterBuffer[0].minInitialVelocity.x, EmitterBuffer[0].maxInitialVelocity.x, alphaA);
                ParticleBuffer[id].velocity.y =
                    lerp(EmitterBuffer[0].minInitialVelocity.y, EmitterBuffer[0].maxInitialVelocity.y, alphaB);
                ParticleBuffer[id].velocity.z =
                    lerp(EmitterBuffer[0].minInitialVelocity.z, EmitterBuffer[0].maxInitialVelocity.z, alphaC);

                ParticleBuffer[id].uv    = EmitterBuffer[0].uv;
                ParticleBuffer[id].color = EmitterBuffer[0].initialColor;

                ParticleBuffer[id].position =
                    float4(_EyePosition + ParticleBuffer[id].velocity * float3(_Scale, 1.0f, _Scale), 1.0f);
        }
        else
        {
                float alpha              = 1.0f - ParticleBuffer[id].time / EmitterBuffer[0].accumulatedTime;
                ParticleBuffer[id].color = lerp(EmitterBuffer[0].initialColor, EmitterBuffer[0].finalColor, alpha);
                /*ParticleBuffer[id].scale = lerp(EmitterBuffer[0].particleScale.x, EmitterBuffer[0].particleScale.y, alpha);*/
                ParticleBuffer[DTid.x].time -= _DeltaTime;
                // ParticleBuffer[DTid.x].position += 1.0f*float4(ParticleBuffer[DTid.x].velocity * _DeltaTime, 0.0f);
                float2 Min = float2(-0.5f * _Scale, -0.5f * _Scale);
                // Min        = float2(-10.0f, -10.0f);
                float2 Max = -Min;
                // ParticleBuffer[id].position.xyz =
                //  WrapPosition(ParticleBuffer[id].position.xyz, _EyePosition + Min, _EyePosition + Max);
                ParticleBuffer[id].position.xz =
                    wrap(ParticleBuffer[id].position.xz, _EyePosition.xz + Min, _EyePosition.xz + Max);

        }

        
}


void ParticleData(FParticleGPU particle, FPhysicParticle phyParticle)
{
        phyParticle.center = particle.position.xyzx;
        phyParticle.radius = particle.scale;
}