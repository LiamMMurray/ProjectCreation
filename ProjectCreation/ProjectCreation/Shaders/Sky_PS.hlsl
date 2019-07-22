#include "DefaultPixelIn.hlsl"

#include "BasePassTextureMaps.hlsl"
#include "Constants.hlsl"
#include "Math.hlsl"
#include "Samplers.hlsl"

cbuffer CSurfaceProperties : register(b2)
{
        float3 _diffuseColor;
        float  _ambientIntensity;
        float3 _emissiveColor;
        float  _specular;
        float  _roughnessMin;
        float  _roughnessMax;
        float  _metallic;
        float  _normalIntensity;
        int    _textureFlags;
};

cbuffer SceneInfoBuffer : register(b1)
{
        float3 _EyePosition;
        float  _Time;
        float3 _DirectionalLightDirection;
        float  _playerRadius;
        float3 _DirectionalLightColor;
        // float pad
        float3 _AmbientColor;
};

float2 rotateUV(float2 uv, float rotation)
{
        float mid = 0.5;
        return float2(cos(rotation) * (uv.x - mid) + sin(rotation) * (uv.y - mid) + mid,
                      cos(rotation) * (uv.y - mid) - sin(rotation) * (uv.x - mid) + mid);
}

static const float sunRadius             = 0.00075f;
static const float sunAngularDiameterCos = 1.0f - sunRadius;

float4 main(INPUT_PIXEL pIn) : SV_TARGET
{
        float3 viewWS = -normalize(pIn.PosWS - _EyePosition);

        float2 windDir = float2(1.0f, 1.0f);


        // return noiseBlend;
        float2 flowMap = diffuseMap.SampleLevel(sampleTypeWrap, rotateUV(pIn.Tex, -0.004f * _Time), 0).rg;
        float2 flow    = flowMap * 2.0f - 1.0f;
        // return flow;
        // return sphere;

        // get and uncompress the flow vector for this pixel
        float skySpeed        = 0.004f * _Time;
        float distortionSpeed = 0.0015f * _Time;
        // Sample normal map.
        float noiseA = Mask1.Sample(sampleTypeWrap, pIn.Tex * 8.0f + distortionSpeed).r;
        float noiseB = Mask1.Sample(sampleTypeWrap, pIn.Tex * 16.0f - distortionSpeed / 4.0f).r;
        float noiseC = Mask1.Sample(sampleTypeWrap, pIn.Tex * 32.0f + distortionSpeed / 8.0f).r;
        float noise  = noiseA * noiseB * noiseC;
        // return noise;
        float skyA = diffuseMap.SampleLevel(sampleTypeWrap, rotateUV(pIn.Tex, skySpeed + 0.1f * flowMap) + 0.01f * noise, 0).b;

        float cloudSample = skyA;
        // return cloudSample;

        float2 flowWave = sin(_Time * 0.007f) * flow;
        float2 alpha    = flow * 0.5f + 0.5f;

        float cloudMaskBase    = saturate(pow(cloudSample, 1.3f) * 1.5f);
        float cloudMaskBacklit = saturate(pow(cloudSample, 2.8f) * 3.5f);

        cloudMaskBacklit = saturate((cloudMaskBacklit - cloudMaskBase) * 5.0f);
        // return cloudMaskBacklit;

        float2 skyCenterUV = pIn.Tex * 2.0f - 1.0f;
        float  skyCenter   = dot(skyCenterUV, skyCenterUV);

        float3 deepWaterColor    = float3(0.0f, 0.2f, 0.6f);
        float3 shallowWaterColor = float3(0.0f, 0.5f, 0.4f);
        float  skyColorMask      = saturate(pow(skyCenter, 1.0f));
        float3 skyColor          = lerp(deepWaterColor, shallowWaterColor, skyColorMask * 0.6f);

        float3 L = _DirectionalLightDirection;
        float  N = normalize(_EyePosition - pIn.PosWS);
        float3 V = viewWS;
        float3 H = normalize(L + V);

        float VDotL         = dot(V, L);
        float sunAtmosphere = smoothstep(0.5, 2.0, VDotL);
        float sunMask =
            saturate(smoothstep(sunAngularDiameterCos, sunAngularDiameterCos + 0.0002, VDotL) + sunAtmosphere * 0.3f);
        // return sunAtmosphere;
        // return sunMask;
        float  cloudDepth   = 1.0f - saturate(pow(cloudSample, 16.0f) * 2.0f);
        float3 flatSunColor = float3(1.0f, 0.9f, 0.4f);
        float3 sunColor     = 2.0f * flatSunColor;
        // return 1.0f - cloudDepth;
        float backLitMask = saturate((pow(max(VDotL, 0.0f), 150.0f) * 0.5f) + smoothstep(0.998, 1.1, VDotL) * 200.0f);
        // return backLitMask;
        // return 1.0f - cloudDepth;
        float3 scatteringColor   = float3(1.0f, 0.5f, 0.1f);
        float3 backlitCloudColor = lerp(0.01f, scatteringColor * 2.5f, cloudMaskBacklit);
        // return backlitCloudColor.xyzz;
        float3 cloudColor = lerp(1.1f * float3(1.0f, 1.0f, 1.0f), backlitCloudColor, backLitMask);
        // return cloudColor.xyzz;
        // return backLitMask;
        // return cloudDepth;
        float3 color = skyColor;
        color        = lerp(color, sunColor, sunMask);

        float cloudBlendAlpha = saturate(cloudMaskBase * 5.0f);
        // return cloudBlendAlpha
        color = lerp(color, cloudColor * lerp(0.5f, 1.0f, cloudMaskBase), cloudBlendAlpha);

        float3 horizonColor = float3(0.7f, 0.7f, 0.85f);

        float horizonMask = saturate(pow(skyCenter, 8.0f));
        // return horizonMask;

        color = lerp(color, horizonColor, horizonMask);
        //return color.xyzz;
        float revealAlpha = _playerRadius / 500.0f;
        revealAlpha       = saturate(pow(revealAlpha, 4.0f));

        return float4(color * revealAlpha, 1.0f);
}