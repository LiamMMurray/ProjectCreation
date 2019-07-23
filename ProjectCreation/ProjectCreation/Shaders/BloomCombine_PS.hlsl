#include "BloomInclude.hlsl"
#include "PostProcessConstantBuffers.hlsl"

#include "Math.hlsl"
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
Texture2D MaskTexture : register(t5);


float4 main(float4 pos : SV_POSITION, float2 texCoord : TEXCOORD0) : SV_TARGET0
{
        float aspectRatio = (1.0f / _inverseScreenDimensions.y) / (1.0f / _inverseScreenDimensions.x);

        float4 uvOffset = float4(_inverseScreenDimensions.x, 0.0f, _inverseScreenDimensions.y, 1.0f);

        float2 uv                = float2(1.0f, aspectRatio) * abs(texCoord * 2.0f - 1.0f);
        float  vignetteIntensity = dot(uv, uv);
        vignetteIntensity        = saturate(vignetteIntensity - 0.6f);
        vignetteIntensity        = saturate(vignetteIntensity * _selectionAlpha);
        // return vignetteIntensity;
        // return fringeIntensity;

        float3 colorOriginal = ScreenTexture.Sample(sampleTypeClamp, texCoord).rgb;
        float3 colorVignette = _selectionColor;
        float3 color         = colorOriginal;
        float3 bloom         = BloomTexture.Sample(sampleTypeClamp, texCoord).rgb * _brightness;
        //return vignetteIntensity;

        float3 bw     = 0.21 * color.r + 0.71 * color.g + 0.07 * color.b;
        color         = lerp(color, bw, 0.1f);
        float3 dither = InterleavedGradientNoise(pos.xy + _time);

        color += bloom;
        color += 0.004f * dither / 255;
        color = color / (color + 1.f);
        // color *= 1.5f;
        color = pow(color, 1.f / 2.2f);

		color = lerp(color, colorVignette, vignetteIntensity);

        color += dither / 255;

        return float4(color, 1.f);
}