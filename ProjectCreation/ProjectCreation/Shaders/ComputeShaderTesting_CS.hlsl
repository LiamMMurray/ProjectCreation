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

RWStructuredBuffer<FParticleGPU> ParticleBuffer : register(u0);


[numthreads(10, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
        ParticleBuffer[DTid.x].position += 0.25;
}