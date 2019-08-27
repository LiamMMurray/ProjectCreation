#include "DefaultPixelIn.hlsl"
#include "DefaultVertexIn.hlsl"
#include "MVPBuffer.hlsl"
#include "Samplers.hlsl"
#include "SceneBuffer.hlsl"

Texture2D diffuseMap : register(t0);

INPUT_PIXEL main(INPUT_VERTEX input)
{

        INPUT_PIXEL output = (INPUT_PIXEL)0;

        output.Tex = input.Tex * float2(1.4f, 1.3f);
        output.Tex.y += _Time * 0.04f;
        float heightSample = diffuseMap.SampleLevel(sampleTypeWrap, output.Tex / 2.0f, 0);
        heightSample       = saturate(pow(heightSample * 2.0f - 1.0f, 2.0f));

        output.TangentWS = mul(float4(input.Tangent, 0), World).xyz;
        output.TangentWS = normalize(output.TangentWS);

        output.BinormalWS = mul(float4(input.Binormal, 0), World).xyz;
        output.BinormalWS = normalize(output.BinormalWS);

        output.NormalWS = mul(float4(input.Normal, 0), World).xyz;
        output.NormalWS = normalize(output.NormalWS);


        float4 Pos         = float4(input.Pos, 1);
        output.linearDepth = 1.0f - saturate(Pos.y / 115.0f);
        Pos.xyz += heightSample * output.NormalWS * 5.0f;


        output.PosWS = mul(Pos, World).xyz;
        output.Pos   = mul(float4(output.PosWS, 1.0f), ViewProjection);
        // input.Tex.y         = sin(input.Tex.y * 0.5f * _Time);
        output.Color = input.Color;


        return output;
}