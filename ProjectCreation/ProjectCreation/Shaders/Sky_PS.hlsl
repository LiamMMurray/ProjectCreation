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

        float3 flatSunColor = float3(1.0f, 0.9f, 0.4f);
        float3 sunColor     = 2.0f * flatSunColor;

        float2 skyCenterUV = pIn.Tex * 2.0f - 1.0f;
        float  skyCenter   = dot(skyCenterUV, skyCenterUV);

        float3 deepWaterColor    = float3(0.0f, 0.2f, 0.6f);
        float3 shallowWaterColor = float3(0.0f, 0.5f, 0.4f);
        float  skyColorMask      = saturate(pow(skyCenter, 1.0f));
        float3 skyColor          = lerp(deepWaterColor, shallowWaterColor, skyColorMask * 0.6f);

        float3 L = _DirectionalLightDirection;
        float  N = normalize(_EyePosition - pIn.PosWS).r;
        float3 V = viewWS;
        float3 H = normalize(L + V);

        float VDotL         = dot(V, L);
        float sunAtmosphere = smoothstep(0.5, 2.0, VDotL);
        float sunMask =
            saturate(smoothstep(sunAngularDiameterCos, sunAngularDiameterCos + 0.0002, VDotL) + sunAtmosphere * 0.3f);

        float3 color = skyColor;
        color        = lerp(color, sunColor, sunMask);


        float3 horizonColor = float3(0.7f, 0.7f, 0.85f);

        float horizonMask = saturate(pow(skyCenter, 8.0f) - min(pIn.PosWS.y, 0.0f));
        // return horizonMask;

        color = lerp(color, horizonColor, horizonMask);
        // return color.xyzz;
        float revealAlpha = _playerRadius / 250.0f;
        revealAlpha       = saturate(pow(revealAlpha, 4.0f));

        return float4(color * revealAlpha, 1.0f);
}