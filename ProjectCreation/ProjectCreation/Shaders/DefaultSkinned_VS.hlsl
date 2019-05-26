#include "DefaultPixelIn.hlsl"


Texture2D Mask1 : register(t7);
Texture2D Mask2 : register(t8);

struct INPUT_VERTEX
{
        float3 Pos : POSITION;
        float4 Color : COLOR;
        float2 Tex : TEXCOORD;
        float3 Normal : NORMAL;
        float3 Tangent : TANGENT;
        float3 Binormal : BINORMAL;

        uint4  JointIndices : JOINTINDICES;
        float4 Weights : WEIGHTS;
};

cbuffer MVPBuffer : register(b0)
{
        matrix ViewProjection;
        matrix World;
};

cbuffer AnimationBuffer : register(b3)
{
	matrix jointTransforms[64];
};

INPUT_PIXEL main(INPUT_VERTEX vIn)
{
		float4 skinned_pos = { 0.0f, 0.0f, 0.0f, 0.0f };
		float4 skinned_norm = { 0.0f, 0.0f, 0.0f, 0.0f };
		float4 skinned_binorm = { 0.0f, 0.0f, 0.0f, 0.0f };
		float4 skinned_tang = { 0.0f, 0.0f, 0.0f, 0.0f };
		[unroll]
		for (int j = 0; j < 4; ++j)
		{
			skinned_pos		+= mul(float4(vIn.Pos.xyz, 1.0f), jointTransforms[vIn.JointIndices[j]]) * vIn.Weights[j];
			skinned_norm	+= mul(float4(vIn.Normal.xyz, 0.0f), jointTransforms[vIn.JointIndices[j]]) * vIn.Weights[j];
			skinned_binorm	+= mul(float4(vIn.Binormal.xyz, 0.0f), jointTransforms[vIn.JointIndices[j]]) * vIn.Weights[j];
			skinned_tang	+= mul(float4(vIn.Tangent.xyz, 0.0f), jointTransforms[vIn.JointIndices[j]]) * vIn.Weights[j];
		}

        INPUT_PIXEL  output = (INPUT_PIXEL)0;
        const float4 Pos    = skinned_pos;
        output.PosWS        = mul(Pos, World).xyz;
        output.Pos          = mul(float4(output.PosWS, 1.0f), ViewProjection);

        output.Tex   = vIn.Tex;
        output.Color = vIn.Color;

        output.TangentWS = mul(skinned_tang, World).xyz;
        output.TangentWS = normalize(output.TangentWS);

        output.BinormalWS = mul(skinned_binorm, World).xyz;
        output.BinormalWS = normalize(output.BinormalWS);

        output.NormalWS = mul(skinned_norm, World).xyz;
        output.NormalWS = normalize(output.NormalWS);

        output.linearDepth = output.Pos.w;

        return output;
}