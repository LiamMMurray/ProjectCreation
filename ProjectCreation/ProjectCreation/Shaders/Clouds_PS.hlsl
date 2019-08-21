#include "DefaultPixelIn.hlsl"

#include "BasePassTextureMaps.hlsl"
#include "Constants.hlsl"
#include "Math.hlsl"
#include "Samplers.hlsl"
#include "SceneBuffer.hlsl"
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
        float2 depthMask = diffuseMap.SampleLevel(sampleTypeWrap, rotateUV(pIn.Tex, -0.004f * _Time), 0).r;
        // return flow;
        // return sphere;

        // get and uncompress the flow vector for this pixel
        float skySpeed        = 0.004f * _Time;
        float distortionSpeed = 0.0135f * _Time;
        // Sample normal map.
        float noiseA = diffuseMap.Sample(sampleTypeWrap, pIn.Tex * 10.0f + distortionSpeed).g;
        float noiseB = diffuseMap.Sample(sampleTypeWrap, pIn.Tex * 24.0f - distortionSpeed / 4.0f).g;
        float noiseC = diffuseMap.Sample(sampleTypeWrap, pIn.Tex * 40.0f + distortionSpeed / 8.0f).g;
        float noiseD = diffuseMap.Sample(sampleTypeWrap, pIn.Tex * 2.0f + distortionSpeed * 2.0f).g;
        float noise  = noiseA * noiseB * noiseC;
        // return noise;
        float skyA = diffuseMap.SampleLevel(sampleTypeWrap, rotateUV(pIn.Tex, skySpeed + noiseA * 0.03f) + 0.015f * noise, 0).b;
        float skyB =
            diffuseMap.SampleLevel(sampleTypeWrap, rotateUV(pIn.Tex, skySpeed + noiseA * 0.03f + 1.0f) + 0.015f * noise, 0).b;

        float timeBlendA = (sin(_Time + noiseD * 0.1f) * 0.5f + 0.5f);
        float timeBlendB = 1.0f - timeBlendA;

        //skyA = saturate(skyA - timeBlendA);
        //skyA = saturate(skyA + (skyA * timeBlendA));
		//
        //skyB = saturate(skyB - timeBlendB);
        //skyB = saturate(skyB + (skyB * timeBlendB));

        float cloudSample = saturate(skyA);
        // return cloudSample;

        float cloudMaskBase    = saturate(pow(cloudSample, 1.3f) * 1.5f);
        float cloudMaskBacklit = saturate(pow(cloudSample, 2.8f) * 3.5f);

        cloudMaskBacklit = saturate((cloudMaskBacklit - cloudMaskBase) * 5.0f);
        // return cloudMaskBacklit;

        float3 L = _DirectionalLightDirection;
        float3 V = viewWS;
        float3 H = normalize(L + V);

        float VDotL = dot(V, L);

        float cloudDepth = 1.0f - saturate(pow(depthMask, 4.0f) * 2.0f);

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
        float3 color = cloudColor * lerp(0.5f, 1.0f, cloudMaskBase);

        float revealAlpha = _playerRadius / 250.0f;
        revealAlpha       = saturate(pow(revealAlpha, 4.0f));
        color *= revealAlpha;

        float2 skyCenterUV  = pIn.Tex * 2.0f - 1.0f;
        float  skyCenter    = dot(skyCenterUV, skyCenterUV);
        float  horizonMask  = saturate(pow(skyCenter, 8.0f) - min(pIn.PosWS.y, 0.0f));
        float3 horizonColor = float3(0.7f, 0.7f, 0.85f);

        float cloudBlendAlpha = cloudSample;

        color = lerp(color, horizonColor, horizonMask);

        clip(cloudBlendAlpha < 0.5f ? -1 : 1);

        return float4(color * revealAlpha, 1.0f);
}