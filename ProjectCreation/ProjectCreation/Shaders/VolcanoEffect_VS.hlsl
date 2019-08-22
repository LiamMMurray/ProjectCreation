#include "DefaultPixelIn.hlsl"
#include "DefaultVertexIn.hlsl"
#include "MVPBuffer.hlsl"
#include "Samplers.hlsl"
#include "SceneBuffer.hlsl"

Texture2D diffuseMap : register(t0);

INPUT_PIXEL main(INPUT_VERTEX input)
{

        INPUT_PIXEL output = (INPUT_PIXEL)0;

        output.Tex = input.Tex;
        output.Tex.y += _Time * 0.04f;
        float heightSample = diffuseMap.SampleLevel(sampleTypeWrap, output.Tex, 0);


        output.TangentWS = mul(float4(input.Tangent, 0), World).xyz;
        output.TangentWS = normalize(output.TangentWS);

        output.BinormalWS = mul(float4(input.Binormal, 0), World).xyz;
        output.BinormalWS = normalize(output.BinormalWS);

        output.NormalWS = mul(float4(input.Normal, 0), World).xyz;
        output.NormalWS = normalize(output.NormalWS);

        output.linearDepth = output.Pos.w;

        float4 Pos = float4(input.Pos, 1);
        Pos.xyz += heightSample * output.NormalWS * 5.0f;


        output.PosWS = mul(Pos, World).xyz;
        output.Pos   = mul(float4(output.PosWS, 1.0f), ViewProjection);
        // input.Tex.y         = sin(input.Tex.y * 0.5f * _Time);
        output.Color = input.Color;


        return output;
}