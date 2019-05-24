#include "BloomInclude.hlsl"

#include "Samplers.hlsl"
Texture2D ScreenTexture : register(t0);

float4 main(float4 pos : SV_POSITION, float2 texCoord : TEXCOORD0) : SV_TARGET0
{
        float3 color = ScreenTexture.Sample(sampleTypeClamp, texCoord).rgb;
        color             = min(float3(256 * 256, 256 * 256, 256 * 256), color);
        float bloomAmount = (color.r + color.g + color.b) / 3.0f - _threshold;

		bloomAmount = saturate(bloomAmount);

		return float4(color * bloomAmount, 0.0f);
}