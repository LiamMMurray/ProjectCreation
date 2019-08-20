#include "MVPBuffer.hlsl"
#include "Quaternion.hlsli"
#include "SceneBuffer.hlsl"
struct OUTPUT_VERTEX
{
        float4 Pos : SV_POSITION;
        float4 Color : COLOR;
};

struct INPUT_PIXEL
{
        float4 Pos : SV_POSITION;
        float4 Color : COLOR;
        float2 Tex : TEXCOORD0;
};

#include "Bezier.hlsli"

static const uint gDetail = 3;

[maxvertexcount(64)] void main(line OUTPUT_VERTEX input[2], uint primID
                               : SV_PrimitiveID, inout TriangleStream<INPUT_PIXEL> output) {
        float       dt = 1.0f / float(gDetail);
        float       t  = 0.0f;
        INPUT_PIXEL quad[8];


        float3 fw = normalize(input[0].Pos - input[1].Pos);
        float3 up = float3(0.0f, 1.0f, 0.0f);

        float4 quatA = rotate_angle_axis(_Time, fw);
        float4 quatB = rotate_angle_axis(_Time + 1.8f, fw);
        float3 upA   = rotate_vector(up, quatA);
        float3 upB   = rotate_vector(up, quatB);
        float3 rtA   = 0.02f * normalize(cross(fw, upA));
        float3 rtB   = 0.02f * normalize(cross(fw, upB));


        quad[0].Pos   = input[0].Pos;
        quad[0].Color = input[0].Color;
        quad[0].Tex = float2(0.0f, 0.0f);
        quad[0].Pos.xyz -= rtA;

        quad[1].Pos   = input[0].Pos;
        quad[1].Color = input[0].Color;
        quad[1].Tex   = float2(1.0f, 0.0f);
        quad[1].Pos.xyz += rtA;

        quad[2].Pos   = input[1].Pos;
        quad[2].Color = input[1].Color;
        quad[2].Tex   = float2(0.0f, 1.0f);
        quad[2].Pos.xyz -= rtA;

        quad[3].Pos   = input[1].Pos;
        quad[3].Color = input[1].Color;
        quad[3].Tex   = float2(1.0f, 1.0f);
        quad[3].Pos.xyz += rtA;

        quad[4].Pos   = input[0].Pos;
        quad[4].Color = input[0].Color;
        quad[4].Tex   = float2(0.0f, 0.0f);
        quad[4].Pos.xyz -= rtB;

        quad[5].Pos   = input[0].Pos;
        quad[5].Color = input[0].Color;
        quad[5].Tex   = float2(1.0f, 0.0f);
        quad[5].Pos.xyz += rtB;

        quad[6].Pos   = input[1].Pos;
        quad[6].Color = input[1].Color;
        quad[6].Tex   = float2(0.0f, 1.0f);
        quad[6].Pos.xyz -= rtB;

        quad[7].Pos   = input[1].Pos;
        quad[7].Color = input[1].Color;
        quad[7].Tex   = float2(1.0f, 1.0f);
        quad[7].Pos.xyz += rtB;

        for (uint k = 0; k < 8; ++k)
        {
                quad[k].Pos = mul(quad[k].Pos, ViewProjection);
        }

        output.Append(quad[0]);
        output.Append(quad[1]);
        output.Append(quad[2]);
        output.Append(quad[3]);

        output.RestartStrip();

        output.Append(quad[4]);
        output.Append(quad[5]);
        output.Append(quad[6]);
        output.Append(quad[7]);
}