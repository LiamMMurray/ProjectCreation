#include "DefaultPixelIn.hlsl"
#include "DefaultVertexIn.hlsl"
#include "MVPBuffer.hlsl"

#include "SceneBuffer.hlsl"

INPUT_PIXEL main(INPUT_VERTEX vIn)
{
        INPUT_PIXEL  output = (INPUT_PIXEL)0;
        const float4 Pos    = float4(vIn.Pos, 1);
        // matrix       ToCamera;
        // ToCamera[2].xyz    = normalize(Billboard[3].xyz - World[3].xyz) ;
        //
        // ToCamera[0].xyz = normalize(cross(ToCamera[2].xyz, float3(0.0f, 1.0f, 0.0f)));
        // ToCamera[1].xyz = normalize(cross(ToCamera[0].xyz, ToCamera[2].xyz));
        // ToCamera *= scale;
        // ToCamera[3] =  World[3];
        //
        //
        // output.PosWS        = mul(Pos, ToCamera).xyz;
        output.PosWS          = mul(Pos, Billboard).xyz;
        output.Pos   = mul(float4(output.PosWS, 1.0f), ViewProjection);
        // output.Pos = mul(output.Pos, Billboard);
        // output.Pos = mul(output.Pos, Projection);


        output.Tex   = vIn.Tex;
        output.Color = vIn.Color;

        output.TangentWS = mul(float4(vIn.Tangent, 0), Billboard).xyz;
        output.TangentWS = normalize(output.TangentWS);

        output.BinormalWS = mul(float4(vIn.Binormal, 0), Billboard).xyz;
        output.BinormalWS = normalize(output.BinormalWS);

        output.NormalWS = mul(float4(vIn.Normal, 0), Billboard).xyz;
        output.NormalWS = normalize(output.NormalWS);

        output.linearDepth = output.Pos.w;

        // float3 dirVec = pIn.PosWS - _EyePosition;
        // float  dist   = sqrt(dot(dirVec, dirVec));
        //
        // float maskDetail = Mask1.Sample(sampleTypeWrap, pIn.Tex).x;
        // float mask       = saturate(dist / 0.5f - 4.0f + pow(maskDetail, 2));
        // float inv_mask   = 1.0f - mask;
        // float band       = inv_mask - floor(inv_mask) + mask - floor(mask);


        return output;
}