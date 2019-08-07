#include "Math.hlsl"
#include "Samplers.hlsl"
#include "Terrain_Includes.hlsl"
#include "Wrap.hlsl"

struct FData
{
        matrix mtx;
};

RWStructuredBuffer<FData> InstanceTransforms : register(u0);


[numthreads(1, 1, 1)] void main(uint3 DTid
                                : SV_DispatchThreadID) {
        uint   id    = DTid.x;
        float3 pos   = float3(InstanceTransforms[id].mtx._41, InstanceTransforms[id].mtx._42, InstanceTransforms[id].mtx._43);
        float  scale = 100.0f;

        float2 Min = float2(-0.5f * scale, -0.5f * scale);
        // Min        = float2(-10.0f, -10.0f);
        float2 Max = -Min;
        pos.xz     = wrap(pos.xz, _EyePosition.xz + Min, _EyePosition.xz + Max);

        float2 tex = pos.xz / (float2(gScale, gScale)) - 0.5f;


        pos.y = HeightMap.SampleLevel(sampleTypeWrap, tex, 0).r * 2625.f * (gTerrainAlpha)-1260.0f * (gTerrainAlpha);
        pos.y /= 8000.0f / gScale;


        InstanceTransforms[id].mtx._41 = pos.x;
        InstanceTransforms[id].mtx._42 = pos.y;
        InstanceTransforms[id].mtx._43 = pos.z;
}
