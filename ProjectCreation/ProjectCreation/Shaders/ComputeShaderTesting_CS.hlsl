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

RWStructuredBuffer<FParticleGPU> ParticleBuffer : register(u0);
StructuredBuffer<FEmitterGPU>    EmitterBuffer : register(t1);

[numthreads(10, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
        ParticleBuffer[DTid.x].position = EmitterBuffer[DTid.y].position;

}