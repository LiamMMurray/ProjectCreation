#include "DefaultPixelIn.hlsl"
#include "DefaultVertexIn.hlsl"
#include "MVPBuffer.hlsl"

#include "SceneBuffer.hlsl"

INPUT_PIXEL main(INPUT_VERTEX vIn)
{
        INPUT_PIXEL  output  = (INPUT_PIXEL)0;
        const float4  Pos    = float4(vIn.Pos, 1);
        output.PosWS         = mul(Pos, World).xyz;
        output.Pos           = mul(float4(output.PosWS, 1.0f), ViewProjection);

        output.Tex   = vIn.Tex;
        output.Color = vIn.Color;

        output.TangentWS = mul(float4(vIn.Tangent, 0), World).xyz;
        output.TangentWS = normalize(output.TangentWS);

        output.BinormalWS = mul(float4(vIn.Binormal, 0), World).xyz;
        output.BinormalWS = normalize(output.BinormalWS);

        output.NormalWS = mul(float4(vIn.Normal, 0), World).xyz;
        output.NormalWS = normalize(output.NormalWS);

        output.linearDepth = output.Pos.w;

		//float3 dirVec = pIn.PosWS - _EyePosition;
        //float  dist   = sqrt(dot(dirVec, dirVec));
		//
        //float maskDetail = Mask1.Sample(sampleTypeWrap, pIn.Tex).x;
        //float mask       = saturate(dist / 0.5f - 4.0f + pow(maskDetail, 2));
        //float inv_mask   = 1.0f - mask;
        //float band       = inv_mask - floor(inv_mask) + mask - floor(mask);


        return output;
}