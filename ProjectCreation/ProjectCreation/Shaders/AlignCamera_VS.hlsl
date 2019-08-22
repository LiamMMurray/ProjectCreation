#include "MVPBuffer.hlsl"

struct INPUT_PIXEL
{
        float4 Pos : SV_POSITION;
        float4 Color : COLOR;
};

struct INPUT_VERTEX
{
        float3 Pos : POSITION;
        float4 Color : COLOR;
};

INPUT_PIXEL main(INPUT_VERTEX vIn)
{
        INPUT_PIXEL  output = (INPUT_PIXEL)0;
        const float4 Pos    = float4(vIn.Pos, 1);
        output.Pos          = mul(Pos, ViewProjection);

        output.Color = vIn.Color;

        return output;
}