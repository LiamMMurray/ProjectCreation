#include "Math.hlsl"
#include "Samplers.hlsl"

#include "Terrain_Includes.hlsl"
struct VertexIn
{
        float3 PosL : POSITION;
        float2 Tex : TEXCOORD0;
};

struct VertexOut
{
        float3 PosL : POSITION0;
        float3 PosW : POSITION1;
        float2 Tex : TEXCOORD0;
};

VertexOut main(VertexIn vIn)
{
        VertexOut vOut;

        vOut.PosL = vIn.PosL;

        float3 posWS = mul(float4(vIn.PosL, 1.0f), World).xyz;
        vOut.PosW    = posWS;

        float scale = gScale;

        vOut.Tex = posWS.xz / (float2(scale, scale)) - 0.5f;

        // vOut.PosL.y = (HeightMap.SampleLevel(sampleTypeClamp, vIn.Tex, 0).r - 0.5f) * 2625.f;
        // vOut.Tex = vIn.Tex;

        return vOut;
}