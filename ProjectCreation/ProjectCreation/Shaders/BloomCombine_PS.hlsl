#include "BloomInclude.hlsl"
#include "PostProcessConstantBuffers.hlsl"

#include "Samplers.hlsl"


float3 WorldPosFromDepth(float depth, float2 texCoord)
{
        float z = depth;

        float4 clipSpacePosition = float4(texCoord * 2.0 - 1.0, z, 1.0);
        float4 viewSpacePosition = mul(clipSpacePosition, _invProj);

        // Perspective division
        viewSpacePosition /= viewSpacePosition.w;

        float4 worldSpacePosition = mul(viewSpacePosition, _invView);

        return worldSpacePosition.xyz;
}

Texture2D ScreenTexture : register(t2);
Texture2D ScreenDepth : register(t1);
Texture2D BloomTexture : register(t0);


float4 main(float4 pos : SV_POSITION, float2 texCoord : TEXCOORD0) : SV_TARGET0
{
        float3 color = ScreenTexture.Sample(sampleTypeClamp, texCoord).rgb;
        float3 bloom = BloomTexture.Sample(sampleTypeClamp, texCoord).rgb * _brightness;
        float  depth = ScreenDepth.Sample(sampleTypeClamp, texCoord).r;

        float3 worldPos = WorldPosFromDepth(depth, texCoord);

        float3 dirVec = worldPos - _playerPosition;
        float  dist   = sqrt(dot(dirVec, dirVec));
        
		float mask = saturate(1 - dist / 2.0f);
        //return mask;
        color = bloom + lerp(0.0f, color, mask);

        color = color / (color + 1.f);
        color = pow(color, 1.f / 2.2f);

        return float4(color, 1.f);
}