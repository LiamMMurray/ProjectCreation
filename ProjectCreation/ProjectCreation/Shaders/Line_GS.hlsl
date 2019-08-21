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

static float4 colors[3] = {float4(2.0f, 0.0f, 0.0f, 1.0f), float4(0.0f, 2.0f, 0.0f, 1.0f), float4(0.0f, 2.0f, 4.0f, 0.0f)};

[maxvertexcount(64)] void main(line OUTPUT_VERTEX input[2], uint primID
                               : SV_PrimitiveID, inout TriangleStream<INPUT_PIXEL> output) {
        INPUT_PIXEL quad[8];


        float3 fw = normalize(input[0].Pos - input[1].Pos);
        float3 up = float3(0.0f, 1.0f, 0.0f);
        float3 rt = cross(fw, up);

        float3 rtA = 0.02f * rt;
        float3 rtB = 0.02f * up;


        quad[0].Pos   = input[0].Pos;
        quad[0].Color = colors[input[0].ColorID];
        quad[0].Tex   = float2(0.0f, 0.0f);
        quad[0].Pos.xyz -= rtA;

        quad[1].Pos   = input[0].Pos;
        quad[1].Color = colors[input[1].ColorID];
        quad[1].Tex   = float2(1.0f, 0.0f);
        quad[1].Pos.xyz += rtA;

        quad[2].Pos   = input[1].Pos;
        quad[2].Color = colors[input[1].ColorID];
        quad[2].Tex   = float2(0.0f, 1.0f);
        quad[2].Pos.xyz -= rtA;

        quad[3].Pos   = input[1].Pos;
        quad[3].Color = colors[input[1].ColorID];
        quad[3].Tex   = float2(1.0f, 1.0f);
        quad[3].Pos.xyz += rtA;

        quad[4].Pos   = input[0].Pos;
        quad[4].Color = colors[input[1].ColorID];
        quad[4].Tex   = float2(0.0f, 0.0f);
        quad[4].Pos.xyz -= rtB;

        quad[5].Pos   = input[0].Pos;
        quad[5].Color = colors[input[1].ColorID];
        quad[5].Tex   = float2(1.0f, 0.0f);
        quad[5].Pos.xyz += rtB;

        quad[6].Pos   = input[1].Pos;
        quad[6].Color = colors[input[1].ColorID];
        quad[6].Tex   = float2(0.0f, 1.0f);
        quad[6].Pos.xyz -= rtB;

        quad[7].Pos   = input[1].Pos;
        quad[7].Color = colors[input[1].ColorID];
        quad[7].Tex   = float2(1.0f, 1.0f);
        quad[7].Pos.xyz += rtB;

        for (uint k = 0; k < 8; ++k)
        {
                quad[k].PosWS = quad[k].Pos;
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