#include"Samplers.hlsl"
struct VS_INPUT
{
        float3 Pos : POSITION;
        float4 Color : COLOR;
        float2 Tex : TEXCOORD;
        float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
        float4 Pos : SV_POSITION;
        float4 Color : COLOR;
        float2 Tex : TEXCOORD;
        float3 Normal : NORMAL;
};

VS_OUTPUT main(VS_INPUT input)
{
        VS_OUTPUT output = (VS_OUTPUT)0;

        return output;
}