#include "MVPBuffer.hlsl"

struct INPUT_PIXEL
{
        float4 Pos : SV_POSITION;
        float3 PosWS : POSITION;
};

struct INPUT_VERTEX
{
        float4 Pos : POSITION;
};

INPUT_PIXEL main(INPUT_VERTEX vIn)
{
        INPUT_PIXEL  output = (INPUT_PIXEL)0;
        const float4 Pos    = float4(vIn.Pos.xyz, 1);
        output.PosWS        = Pos.xyz;
        output.Pos          = mul(Pos, ViewProjection);


        return output;
}