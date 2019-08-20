#include "MVPBuffer.hlsl"
struct OUTPUT_VERTEX
{
        float4 Pos : SV_POSITION;
        float4 Color : COLOR;
};

struct INPUT_VERTEX
{
        float3 Pos : POSITION;
        float4 Color : COLOR;
};

OUTPUT_VERTEX main(INPUT_VERTEX vIn)
{
        OUTPUT_VERTEX output = (OUTPUT_VERTEX)0;
        output.Pos           = float4(vIn.Pos, 1.0f);
        output.Color         = vIn.Color;

        return output;
}