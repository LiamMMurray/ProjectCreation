#include "BloomInclude.hlsl"
#include "PostProcessConstantBuffers.hlsl"

#include "Math.hlsl"
#include "Samplers.hlsl"

Texture2D ScreenTexture : register(t2);
Texture2D AO : register(t4);
Texture2D ScreenDepth : register(t1);
Texture2D BloomTexture : register(t0);
Texture2D MaskTexture : register(t5);

float3 VSPositionFromDepth(float2 vTexCoord)
{
        // Get the depth value for this pixel
        float z = ScreenDepth.Sample(sampleTypeClamp, vTexCoord).r;
        // Get x/w and y/w from the viewport position
        float  x             = vTexCoord.x * 2 - 1;
        float  y             = (1 - vTexCoord.y) * 2 - 1;
        float4 vProjectedPos = float4(x, y, z, 1.0f);
        // Transform by the inverse projection matrix
        float4 vPositionVS = mul(vProjectedPos, _invProj);
        // Divide by w to get the view-space position
        return vPositionVS.xyz / vPositionVS.w;
}

float4 main(float4 pos : SV_POSITION, float2 texCoord : TEXCOORD0) : SV_TARGET0
{
        float  depthSample = ScreenDepth.Sample(sampleTypeClamp, texCoord).r;
        float3 viewPos     = VSPositionFromDepth(texCoord);
        float3 worldPos    = mul(float4(viewPos,1.0f), _invView).xyz;
        float  linearDepth = viewPos.z;
        // return worldPos.xyzz / 100.0f;

        float2 screenSize  = float2(1.0f / _inverseScreenDimensions.x, 1.0f / _inverseScreenDimensions.y);
        float  aspectRatio = (screenSize.y / screenSize.x);

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
        // return vignetteIntensity;

        float bw               = 0.21 * color.r + 0.71 * color.g + 0.07 * color.b;
        float desaturationMask = 0.17f;
        color                  = lerp(color, bw, desaturationMask);
        float  fogMask  = saturate((linearDepth - 10.0f) / 300.0f) * 0.8f * (1.0f - saturate((linearDepth - 400.0f) / 100.0f));
        float3 fogColor = float3(0.4f, 0.6f, 0.8f);


        float3 dither = InterleavedGradientNoise(pos.xy + _time);
        color += bloom;
        color += 0.004f * dither / 255;
        color = color / (color + 1.f);
        // color *= 1.5f;
        color = pow(color, 1.f / 2.2f);

        float ao = AO.Sample(sampleTypeClamp, texCoord).r;
        // return ao;
        color *= ao;


        color = lerp(color, fogColor, min(fogMask, 0.6f));
        color = lerp(color, colorVignette, vignetteIntensity);
        color += dither / 255;

        return float4(color, 1.f);
}