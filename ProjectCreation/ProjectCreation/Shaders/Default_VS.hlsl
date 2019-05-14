#include "DefaultPixelIn.hlsl"
#include "DefaultVertexIn.hlsl"

INPUT_PIXEL main(INPUT_VERTEX vIn)
{
        INPUT_PIXEL  output = (INPUT_PIXEL)0;
        float4 Pos    = float4(vIn.Pos / 10.0f, 1);
        Pos.z += 0.1f;
        output.PosWS = Pos;
        output.Pos   = Pos;

        output.Tex   = vIn.Tex;
        output.Color = vIn.Color;

        output.TangentWS  = float4(vIn.Tangent, 0);
        output.BinormalWS = float4(vIn.Binormal, 0);
        output.NormalWS   = float4(vIn.Normal, 0);

        output.linearDepth = output.Pos.w;

        return output;
}