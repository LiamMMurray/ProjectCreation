static const unsigned int gMaxEmitterCount = 2 << 10;
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


struct FEmitterGPU
{
        int    currentParticleCount;
        float  accumulatedTime;
        bool   active;
        float4 position;
        float4 color;
        float2 uv;
        float3 velocity;
};

struct FSegmentBuffer
{
        int index[gMaxEmitterCount];
};
RWStructuredBuffer<FParticleGPU> ParticleBuffer : register(u0);
StructuredBuffer<FEmitterGPU>    EmitterBuffer : register(t1);
RWStructuredBuffer<FSegmentBuffer> SegmentBuffer : register(u1);

[numthreads(100, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) 
{
        ParticleBuffer[DTid.x].position = EmitterBuffer[DTid.y].position;
        ParticleBuffer[DTid.x].position += 0.0f;

        
}