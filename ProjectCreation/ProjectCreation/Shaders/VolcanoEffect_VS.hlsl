#include "DefaultPixelIn.hlsl"
#include "DefaultVertexIn.hlsl"
#include "MVPBuffer.hlsl"
#include "SceneBuffer.hlsl"

INPUT_PIXEL main(INPUT_VERTEX input)
{

        INPUT_PIXEL  output = (INPUT_PIXEL)0;
		input.Pos.x += cos(input.Pos.x * 0.5f * _Time);
       // input.Pos.z += sin(input.Pos.z * 0.3f * _Time);
        input.Pos.y += sin(input.Pos.y* 1.5f * _Time);
        const float4 Pos    = float4(input.Pos, 1);
        

        output.PosWS        = mul(Pos, World).xyz;
        output.Pos          = mul(float4(output.PosWS, 1.0f), ViewProjection);

        output.Tex   = input.Tex;
        output.Color = input.Color;

        output.TangentWS = mul(float4(input.Tangent, 0), World).xyz;
        output.TangentWS = normalize(output.TangentWS);

        output.BinormalWS = mul(float4(input.Binormal, 0), World).xyz;
        output.BinormalWS = normalize(output.BinormalWS);

        output.NormalWS = mul(float4(input.Normal, 0), World).xyz;
        output.NormalWS = normalize(output.NormalWS);

        output.linearDepth = output.Pos.w;



        return output;
}