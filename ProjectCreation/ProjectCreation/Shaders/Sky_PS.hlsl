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

        float2 flowMap = diffuseMap.SampleLevel(sampleTypeWrap, pIn.Tex, 0).rg;
        float2 flow    = flowMap * 2.0f - 1.0f;
        // return flow;
        // return sphere;

		        // get and uncompress the flow vector for this pixel
        float skySpeed        = 0.012f * _Time;
        float distortionSpeed = 0.004f * _Time;
        // Sample normal map.
        float noise = Mask1.Sample(sampleTypeWrap, pIn.Tex * 4.0f + distortionSpeed).r;
        noise       = saturate(pow(noise, 4.0f));
        noise       = lerp(0.2f, 1.0f, noise);
        // return noise;
        float3 skyA   = diffuseMap.SampleLevel(sampleTypeWrap, rotateUV(pIn.Tex, skySpeed) + 0.3 * flow * noise, 0).b;
        float  cloudSample = skyA;

        float2 flowWave = sin(_Time * 0.007f) * flow;
        float2 alpha    = flow * 0.5f + 0.5f;
        // return alpha.x;
        float  speed       = _Time * 0.007f;
        float2 uvA         = rotateUV(pIn.Tex, speed);
        float2 uvB         = rotateUV(pIn.Tex, speed + 0.08f);
        float2 uv          = lerp(uvA, uvB, alpha);
        float  cloudMask   = saturate(pow(cloudSample, 1.3f) * 1.5f);

        float2 skyCenterUV = pIn.Tex * 2.0f - 1.0f;
        float  skyCenter   = dot(skyCenterUV, skyCenterUV);

        float3 deepWaterColor    = float3(0.0f, 0.2f, 0.6f);
        float3 shallowWaterColor = float3(0.0f, 0.5f, 0.4f);
        float  skyColorMask      = saturate(pow(skyCenter, 1.0f));
        float3 skyColor          = lerp(deepWaterColor, shallowWaterColor, skyColorMask * 0.6f);

        float3 horizonColor = float3(0.9f, 0.85f, 0.7f);

        float horizonMask = saturate(pow(skyCenter, 6.0f));
        // return horizonMask;

        float3 horizonSkyColor = lerp(skyColor, horizonSkyColor, horizonMask);

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
        float backLitMask = saturate((pow(max(VDotL, 0.0f), 150.0f) * 0.5f) + smoothstep(0.998, 1.1, VDotL) * 10.0f);
        // return backLitMask;
        // return 1.0f - cloudDepth;
        float3 backlitCloudColor = lerp(0.1f, flatSunColor * 1.5f, 1.0f - cloudDepth);
        float3 cloudColor        = lerp(1.1f * float3(1.0f, 1.0f, 1.0f), backlitCloudColor, backLitMask);

        // return cloudDepth;
        float3 color = skyColor;
        color        = lerp(color, sunColor, sunMask);
        color        = lerp(color, cloudColor * lerp(0.6f, 1.0f, cloudMask), saturate(cloudMask * 5.0f));


        return float4(color, 1.0f);
}