#include "DefaultPixelIn.hlsl"
#include "InstanceData.hlsl"
#include "MVPBuffer.hlsl"
#include "SceneBuffer.hlsl"

struct INPUT_VERTEX
{
        float3 Pos : POSITION;
        float4 Color : COLOR;
        float2 Tex : TEXCOORD;
        float3 Normal : NORMAL;
        float3 Tangent : TANGENT;
        float3 Binormal : BINORMAL;
        uint   instanceID : SV_InstanceID;
};

StructuredBuffer<FInstanceData> TransformMatrices : register(t8);
StructuredBuffer<uint>          TransformIndices : register(t9);


INPUT_PIXEL main(INPUT_VERTEX vIn)
{
        INPUT_PIXEL output = (INPUT_PIXEL)0;
        float4      Pos    = float4(vIn.Pos, 1.0f);

        matrix instanceWorld = TransformMatrices[TransformIndices[vIn.instanceID]].mtx;
        float  lifeTime      = saturate(TransformMatrices[TransformIndices[vIn.instanceID]].lifeTime);
        uint   flags         = TransformMatrices[TransformIndices[vIn.instanceID]].flags;

        Pos.xz *= pow(lifeTime, 2.0f);
        Pos.y *= pow(lifeTime, 0.5f);

        output.PosWS = mul(Pos, instanceWorld).xyz;
        output.Pos   = mul(float4(output.PosWS, 1.0f), ViewProjection);

        output.Tex   = vIn.Tex;
        output.Color = vIn.Color;

        output.TangentWS = mul(float4(vIn.Tangent, 0), instanceWorld).xyz;
        output.TangentWS = normalize(output.TangentWS);

        output.BinormalWS = mul(float4(vIn.Binormal, 0), instanceWorld).xyz;
        output.BinormalWS = normalize(output.BinormalWS);

        output.NormalWS = mul(float4(vIn.Normal, 0), instanceWorld).xyz;
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