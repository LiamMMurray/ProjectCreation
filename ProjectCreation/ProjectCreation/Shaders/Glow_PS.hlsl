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
        // float  pad;
        float3 _DirectionalLightColor;
        // float pad
        float3 _AmbientColor;
};

float4 main(INPUT_PIXEL pIn) : SV_TARGET
{
        float3 posMinusEye = pIn.PosWS - _EyePosition;
        float3 viewDir     = normalize(posMinusEye);
        float  fresnel     = dot(-viewDir, pIn.NormalWS);
        fresnel            = saturate(pow(fresnel, 1.5f));
        float mask1        = Mask1.Sample(sampleTypeWrap, pIn.Tex * float2(1.0f, 8.0f) - float2(_Time * 1.0f, _Time * 1.2f)).r;
        float mask2        = Mask1.Sample(sampleTypeWrap, pIn.Tex * float2(2.0f, 3.0f) + float2(_Time * 1.5f, _Time * 0.8f)).r;
        float maskFresnel  = 1.0f - saturate(pow(fresnel * 1.2f, 4.0f));
        float mask         = 1.0f - saturate(pow(mask1 * mask2, 2.0f) * 20.0f);
        float alpha        = min(lerp(1.0f + mask1, mask, maskFresnel) * saturate(pow(fresnel, 2.0f)), 1.2f);
        // return maskFresnel;
        // return mask;
        // mask = (1.0f - mask);
        // return maskFresnel;
        // clip(f)
        float distance    = dot(posMinusEye, posMinusEye);
        float modMask = sin(distance / 4.5f + _Time * 2.5f) * 0.5f + 0.5f;
        float modulation  = lerp(0.9f, 2.0f, modMask);

        float3 color = _emissiveColor * modulation;

        return float4(color * alpha, alpha);
}