static const unsigned int gMaxEmitterCount       = 2 << 10;
static const unsigned int gMaxParticleCount      = 2 << 18;
static const unsigned int gMaxParticlePerEmitter = gMaxParticleCount / gMaxEmitterCount;

#define ALIGN_TO_VEL (1 << 0)

// 0000 0000 0000 0000 0000 0000 0000 0001

struct FParticleGPU
{
        float4 position;
        float4 prevPos;
        float4 initialColor;
        float4 finalColor;
        float3 velocity;
        float  time;
        float3 lifeSpan;
        uint   flags;
        float2 scale;
        float3 acceleration;
        uint    textureIndex;
};


struct FEmitterGPU
{
        float4 lifeSpan; // x is life time, y,z is fed in ad out
        uint   flags;
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
        uint   index; // tpye of particles
        uint   textureIndex;
};

struct FSegmentBuffer
{
        int desiredCount;
};

RWStructuredBuffer<FParticleGPU> ParticleBuffer : register(u0);
StructuredBuffer<FEmitterGPU>    EmitterBuffer : register(t0);
Texture2D                        SceneDepth : register(t1);
StructuredBuffer<FSegmentBuffer> SegmentBuffer : register(t2);