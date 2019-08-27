#include "MVPBuffer.hlsl"
#include "SceneBuffer.hlsl"
struct OUTPUT_VERTEX
{
        float4 Pos : SV_POSITION;
        uint   ColorID : COLOR;
};

struct INPUT_PIXEL
{
        float4 Pos : SV_POSITION;
        float3 PosWS : POSITION;
        float4 Color : COLOR;
        float2 Tex : TEXCOORD0;
};

static const float4 colors[4] = {float4(4.0f, 0.04f, 0.0f, 1.0f),
                                 float4(0.0f, 4.0f, 0.0f, 1.0f),
                                 float4(0.0f, 1.5f, 4.0f, 0.0f),
                                 float4(1.0f, 1.0f, 1.0f, 1.0f)};

[maxvertexcount(64)] void main(line OUTPUT_VERTEX input[2], uint primID
                               : SV_PrimitiveID, inout TriangleStream<INPUT_PIXEL> output) {
        INPUT_PIXEL quad[8];


        float3 fw = normalize(input[0].Pos - input[1].Pos).xyz;
        float3 up = float3(0.0f, 1.0f, 0.0f);
        float3 rt = cross(fw, up);

        float3 rtA = 0.02f * rt;
        float3 rtB = 0.02f * up;

        float4 color = colors[input[0].ColorID];
        

        quad[0].Pos   = input[0].Pos;
        quad[0].Color = color;
        quad[0].Tex   = float2(0.0f, 0.0f);
        quad[0].Pos.xyz -= rtA;

        quad[1].Pos   = input[0].Pos;
        quad[1].Color = color;
        quad[1].Tex   = float2(1.0f, 0.0f);
        quad[1].Pos.xyz += rtA;

        quad[2].Pos   = input[1].Pos;
        quad[2].Color = color;
        quad[2].Tex   = float2(0.0f, 1.0f);
        quad[2].Pos.xyz -= rtA;

        quad[3].Pos   = input[1].Pos;
        quad[3].Color = color;
        quad[3].Tex   = float2(1.0f, 1.0f);
        quad[3].Pos.xyz += rtA;

        quad[4].Pos   = input[0].Pos;
        quad[4].Color = color;
        quad[4].Tex   = float2(0.0f, 0.0f);
        quad[4].Pos.xyz -= rtB;

        quad[5].Pos   = input[0].Pos;
        quad[5].Color = color;
        quad[5].Tex   = float2(1.0f, 0.0f);
        quad[5].Pos.xyz += rtB;

        quad[6].Pos   = input[1].Pos;
        quad[6].Color = color;
        quad[6].Tex   = float2(0.0f, 1.0f);
        quad[6].Pos.xyz -= rtB;
        quad[6].Pos.xyz -= rtB;

        quad[7].Pos   = input[1].Pos;
        quad[7].Color = color;
        quad[7].Tex   = float2(1.0f, 1.0f);
        quad[7].Pos.xyz += rtB;

		[unroll]
        for (uint k = 0; k < 8; ++k)
        {
                quad[k].PosWS = quad[k].Pos.xyz;
                quad[k].Pos   = mul(quad[k].Pos, ViewProjection);
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