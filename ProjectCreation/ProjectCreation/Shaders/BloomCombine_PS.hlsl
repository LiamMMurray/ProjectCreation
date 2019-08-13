#include "BloomInclude.hlsl"
#include "PostProcessConstantBuffers.hlsl"

#include "Math.hlsl"
#include "Samplers.hlsl"

Texture2D RandomAO : register(t3);
Texture2D ScreenTexture : register(t2);
Texture2D ScreenDepth : register(t1);
Texture2D BloomTexture : register(t0);
Texture2D MaskTexture : register(t5);

static const float ao_sample_rad = 0.5f;
static const float ao_intensity  = 2.0f;
static const float ao_scale      = 1.5f;
static const float ao_bias       = 0.3f;

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

float2 GetRandomNormalFromTexture(in float2 uv, float2 screenSize)
{
        return normalize(RandomAO.Sample(sampleTypeWrap, screenSize * uv / 64).xy * 2.0f - 1.0f);
}

float doAmbientOcclusion(in float2 tcoord, in float2 uv, in float3 p, in float3 cnorm)
{
        float3       diff = VSPositionFromDepth(tcoord + uv) - p;
        const float3 v    = normalize(diff);
        const float  d    = length(diff) * ao_scale;
        return max(0.0, dot(cnorm, v) - ao_bias) * (1.0 / (1.0 + d)) * ao_intensity;
}

float4 main(float4 pos : SV_POSITION, float2 texCoord : TEXCOORD0) : SV_TARGET0
{
        float  depthSample = ScreenDepth.Sample(sampleTypeClamp, texCoord).r;
        float3 viewPos     = VSPositionFromDepth(texCoord);
        float3 worldPos    = mul(viewPos, _invView);
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

        float3 bw               = 0.21 * color.r + 0.71 * color.g + 0.07 * color.b;
        float  desaturationMask = 0.17f;
        color                   = lerp(color, bw, desaturationMask);
        float  fogMask  = saturate((linearDepth - 10.0f) / 300.0f) * 0.8f * (1.0f - saturate((linearDepth - 800.0f) / 100.0f));
        float3 fogColor = float3(0.6f, 0.7f, 1.0f);
        color           = lerp(color, fogColor, fogMask);

        float2 randOffset = float2(3.0f, 3.0f);

        float3 posVS    = viewPos;
        float3 posVS1   = VSPositionFromDepth(texCoord + float2(_inverseScreenDimensions.x, 0.0f));
        float3 posVS2   = VSPositionFromDepth(texCoord + float2(0.0f, _inverseScreenDimensions.y));
        float3 normalVS = normalize(cross(posVS1 - posVS, posVS2 - posVS));
        float3 normalWS = mul(normalVS, _invView);
        float  rad      = ao_sample_rad / posVS.z;

        float2 randomVec = GetRandomNormalFromTexture(texCoord, screenSize);

        float ao = 0.0f;

        const float2 vec[4] = {float2(1, 0), float2(-1, 0), float2(0, 1), float2(0, -1)};


        int sampleCount = 4;
        if (linearDepth < 300.0f)
                for (int i = 0; i < sampleCount; ++i)
                {
                        float2 coord1 = reflect(vec[i], randomVec) * rad;
                        float2 coord2 = float2(coord1.x * 0.707 - coord1.y * 0.707, coord1.x * 0.707 + coord1.y * 0.707);

                        ao += doAmbientOcclusion(texCoord, coord1 * 0.25, posVS, normalVS);
                        ao += doAmbientOcclusion(texCoord, coord2 * 0.5, posVS, normalVS);
                        ao += doAmbientOcclusion(texCoord, coord1 * 0.75, posVS, normalVS);
                        ao += doAmbientOcclusion(texCoord, coord2, posVS, normalVS);
                }
		
        // return 1.0f - depthDelta;
        // ao = saturate(ao);

		//return ao;

        float3 dither = InterleavedGradientNoise(pos.xy + _time);

        color += bloom;
        color += 0.004f * dither / 255;
        color = color / (color + 1.f);
        // color *= 1.5f;
        color = pow(color, 1.f / 2.2f);

        color = lerp(color, colorVignette, vignetteIntensity);
        color *= lerp(1.0f, 0.8f, ao);

        color += dither / 255;


        return float4(color, 1.f);
}