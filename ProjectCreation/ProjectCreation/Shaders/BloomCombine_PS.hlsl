#include "BloomInclude.hlsl"

#include "Samplers.hlsl"

Texture2D BloomTexture : register(t0);
Texture2D ScreenTexture : register(t1);

float4 main(float4 pos : SV_POSITION, float2 texCoord : TEXCOORD0) : SV_TARGET0
{
        float3 color = ScreenTexture.Sample(sampleTypeClamp, texCoord).rgb;
        float3 bloom = BloomTexture.Sample(sampleTypeClamp, texCoord).rgb * _brightness;

        color += bloom;

		color = color / (color + 1.f);
        color = pow(color, 1.f / 2.2f);

        return float4(color, 1.f);
}