static const unsigned int gMaxEmitterCount       = 2 << 10;
static const unsigned int gMaxParticleCount      = 2 << 18;
static const unsigned int gMaxParticlePerEmitter = gMaxParticleCount / gMaxEmitterCount;

struct FParticleGPU
{
        float4 position;
        float4 prevPos;
        float4 color;
        float3 velocity;
        float2 uv;
        float  time;
        float  lifeSpan;
        int    emitterIndex;
        float  scale;
        float3 acceleration;
};


struct FEmitterGPU
{
        float4 lifeSpan; // x is life time, y,z is fed in ad out
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

RWStructuredBuffer<FParticleGPU> ParticleBuffer : register(u0);
StructuredBuffer<FEmitterGPU>    EmitterBuffer : register(t0);
Texture2D                        SceneDepth : register(t1);
StructuredBuffer<FSegmentBuffer> SegmentBuffer : register(t2);