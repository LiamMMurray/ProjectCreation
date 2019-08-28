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


float4 main(INPUT_PIXEL pIn) : SV_TARGET
{
        float3 posMinusEye = pIn.PosWS - _EyePosition;
        float3 viewDir     = normalize(posMinusEye);
        float  fresnel     = dot(-viewDir, normalize(pIn.NormalWS));
        fresnel            = saturate(pow(abs(fresnel*1.0f), 15.0f));
        float mask1        = Mask1.Sample(sampleTypeWrap, pIn.Tex * float2(2.0f, 8.0f) - float2(_Time * 1.0f, _Time * 1.2f)).r;
        float mask2        = Mask1.Sample(sampleTypeWrap, pIn.Tex * float2(4.0f, 6.0f) + float2(_Time * 1.5f, _Time * 0.8f)).r;
        // return 1.0f - fresnel;
        float maskFresnel = 1.0f - saturate(pow(fresnel , 2.0f));
        float mask        = 1.0f - saturate(pow(mask1 * mask2, 4.0f) * 15.0f);
        float alpha       = saturate(lerp(1.0f + mask1, mask, maskFresnel) * saturate(pow(fresnel, 2.0f)));
        // return maskFresnel;
        // return mask;
        // mask = (1.0f - mask);
        // return maskFresnel;
        // clip(f)
        float distance   = dot(posMinusEye, posMinusEye);
        float dotSelf    = dot(pIn.PosWS, pIn.PosWS);
        float modMask    = sin(_Time * 2.5f) * 0.5f + 0.5f;
        float modulation = lerp(0.9f, 2.0f, modMask);

        float desaturationMaskA = saturate((distance + 10.0f) / 100.0f);
        float desaturationMaskB = saturate((distance + 1000.0f) / 100.0f);

        float3 blendedColor;

        float  grayscale    = dot(_emissiveColor, float3(0.21, 0.71, 0.07));
        float3 closeColor   = _emissiveColor;
        float3 farColor     = lerp(_emissiveColor, grayscale, 0.6f);
        float3 veryFarColor = _emissiveColor;
        blendedColor        = lerp(closeColor, farColor, desaturationMaskA);
        blendedColor        = lerp(blendedColor, veryFarColor, desaturationMaskB);


        float3 modulatedColor = blendedColor * modulation;
        blendedColor          = lerp(modulatedColor, blendedColor, saturate(distance / 200.0f));

        return float4(blendedColor * alpha, saturate(alpha));
}