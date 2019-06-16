struct ParticleEmitter
{
        float4 spawnPosition;
        float4 spawnColor;
        float3 velocity;
        float2 uv;
        float  time;
};

[numthreads(1, 1, 1)]
RWStructuredBuffer<ParticleEmitter> buffer : register(u0);

void main(uint3 DTid : SV_DispatchThreadID)
{
        buffer[DTid.x].spawnPosition += 1.0f;
}