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

struct INPUT_PIXEL
{
        float4 Pos : SV_POSITION;
        float3 PosWS : POSITION;
        float2 Tex : TEXCOORD0;
};

float4 main(INPUT_PIXEL pIn) : SV_TARGET
{
        float3 posMinusEye = pIn.PosWS - _EyePosition;

        float distance = dot(posMinusEye, posMinusEye);

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

        float  uvMaskY       = 1.0f - saturate((abs(pIn.Tex.y * 2.0f - 1.0f) - 0.2f) * 5.0f);
        float  sinDistortion = 0.5f * (sin(_Time * 10.0f - pIn.Tex.y * 20.0f ));
        float2 uv            = pIn.Tex + float2(sinDistortion, 0.0f);
        float  uvMask        = 1.0f - saturate((abs(uv.x * 2.0f - 1.0f) - 0.2f) * 5.0f);
        uvMask *= uvMaskY;
        float alpha = 1.0f * uvMask;

        return float4(blendedColor * alpha, saturate(alpha));
}