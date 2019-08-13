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

#include "PBRMath.hlsl"

// Check for texture flags
inline int HasDiffuseTexture(int flags)
{
        return ((flags & 1) > 0 ? 1 : 0);
}
inline int HasSpecularTexture(int flags)
{
        return ((flags & 2) > 0 ? 1 : 0);
}
inline int HasMetallicTexture(int flags)
{
        return ((flags & 2) > 0 ? 1 : 0);
}
inline int HasGlossinessTexture(int flags)
{
        return ((flags & 4) > 0 ? 1 : 0);
}
inline int HasRoughnessTexture(int flags)
{
        return ((flags & 4) > 0 ? 1 : 0);
}
inline int HasNormalTexture(int flags)
{
        return ((flags & 8) > 0 ? 1 : 0);
}
inline int HasAOTexture(int flags)
{
        return ((flags & 16) > 0 ? 1 : 0);
}
inline int HasEmissiveMask(int flags)
{
        return ((flags & 32) > 0 ? 1 : 0);
}

inline int HasReflections(int flags)
{
        return ((flags & 64) > 0 ? 1 : 0);
}

inline int IsMasked(int flags)
{
        return ((flags & 128) > 0 ? 1 : 0);
}

inline int IsTransluscent(int flags)
{
        return ((flags & 256) > 0 ? 1 : 0);
}

inline int IsUnlit(int flags)
{
        return ((flags & 512) > 0 ? 1 : 0);
}


float4 main(INPUT_PIXEL pIn) : SV_TARGET
{
        float3 viewWS = -normalize(pIn.PosWS - _EyePosition);
        float3 normalSample;

        SurfacePBR surface;
        surface.diffuseColor = _diffuseColor;
        surface.metallic     = _metallic;

        surface.ambient       = _ambientIntensity;
        surface.emissiveColor = _emissiveColor;

        float alpha = 1.f;

        if (HasDiffuseTexture(_textureFlags))
        {
                float4 diffuse = diffuseMap.Sample(sampleTypeWrap, pIn.Tex);

                surface.diffuseColor *= diffuse.xyz;
        }
        surface.diffuseColor = saturate(surface.diffuseColor);

        float3 genericA = diffuseMap.Sample(sampleTypeWrap, pIn.Tex * 8.0f).a;

        float fresnel = saturate(dot(pIn.NormalWS, viewWS));
        fresnel       = 1.0f - saturate(pow(fresnel + 0.6f, 20.0f));
        fresnel *= saturate(pow(genericA.b + 0.2f, 4.0f));

        fresnel = 1.0f - fresnel;
        clip(fresnel < 0.5f ? -1 : 1);

        if (HasSpecularTexture(_textureFlags))
        {
                surface.metallic *= detailsMap.Sample(sampleTypeWrap, pIn.Tex).y;
        }

        if (HasGlossinessTexture(_textureFlags))
        {
                surface.roughness = lerp(_roughnessMin, _roughnessMax, detailsMap.Sample(sampleTypeWrap, pIn.Tex).x);
        }
        else
        {
                surface.roughness = 0.5f * (_roughnessMin + _roughnessMax);
        }
        // Remapping roughness to prevent errors on normal distribution
        surface.roughness = max(surface.roughness, 0.08f);
        surface.metallic  = 1.0f;
        // surface.diffuseColor = float3(0.1f, 0.2f, 0.3f);
        if (HasAOTexture(_textureFlags))
        {
                surface.ambient *= detailsMap.Sample(sampleTypeWrap, pIn.Tex).z;
        }

        if (HasEmissiveMask(_textureFlags))
        {
                surface.emissiveColor *= emissiveMap.Sample(sampleTypeWrap, pIn.Tex).xyz;
        }

        if (HasNormalTexture(_textureFlags))
        {
                normalSample   = normalMap.Sample(sampleTypeWrap, pIn.Tex).xyz * 2.f - 1.f;
                normalSample.z = sqrt(1 - normalSample.x * normalSample.x - normalSample.y * normalSample.y);
                surface.normal = _normalIntensity * pIn.TangentWS * normalSample.x +
                                 _normalIntensity * pIn.BinormalWS * normalSample.y + pIn.NormalWS * normalSample.z;
        }
        else
        {
                surface.normal = pIn.NormalWS;
        }
        surface.normal = normalize(surface.normal);

        float3 reflectionVector = reflect(viewWS, surface.normal);


        float3 positionWS = pIn.PosWS;
        float3 color      = surface.emissiveColor; // surface.ambient * lightInfo.ambientColor * surface.diffuseColor +

        if (IsUnlit(_textureFlags))
        {
                return (color + surface.diffuseColor, alpha);
        }

        // Non metals use a constant for this value
        float3 specColor = 0.04f;
        // Lerp between diffuse color and constant based on metallic. Ideally metallic should be either 1 or 0
        specColor = lerp(specColor, surface.diffuseColor, surface.metallic);


        // Directional Light
        {
                float3 radiance = _DirectionalLightColor.xyz;
                color += radiance * PBR(surface, -_DirectionalLightDirection, viewWS, specColor);
        }

        // Environment mapping
        {
                surface.ambient *= 1.0f;
                float3 N = surface.normal;
                float3 V = viewWS;

                float  NdotV       = max(dot(N, V), 0.f);
                float3 diffuse     = IBLDiffuse.Sample(sampleTypeWrap, N).rgb;
                float3 specular    = IBLSpecular.SampleLevel(sampleTypeWrap, reflectionVector, surface.roughness * 10.f).rgb;
                float2 integration = IBLIntegration.Sample(sampleTypeNearest, float2(NdotV, 1.f - surface.roughness)).rg;
                color += IBL(surface, viewWS, specColor, diffuse, specular, integration);
        }
        return float4(color, 1.0f);

        float maskA     = Mask1.Sample(sampleTypeWrap, pIn.PosWS.xz / 45.0f + _Time * 0.01f * float2(1.0f, 0.0f)).z;
        float maskB     = Mask1.Sample(sampleTypeWrap, pIn.PosWS.xz / 40.0f + _Time * 0.01f * float2(-1.0f, 0.0f)).z;
        float areaMaskA = Mask1.Sample(sampleTypeWrap, pIn.PosWS.xz / 8.0f + maskA + maskB).z * 2.0f - 1.0f;
        // return areaMaskA;

        float3 dirVec = pIn.PosWS + float3(areaMaskA, 0, areaMaskA) - _EyePosition;
        float  dist   = sqrt(dot(dirVec.xz, dirVec.xz));

        float modulatedDistance = dist / .1f + InterleavedGradientNoise(pIn.Pos.xy + _Time);

        float mask         = saturate(dist / 0.1f - _playerRadius);
        float bandA        = saturate(dist / 0.1f - _playerRadius + 0.5f);
        float bandB        = saturate(dist / 0.1f - _playerRadius - 0.5f);
        float bandCombined = saturate(5.0f * (bandA - bandB));
        color *= (1 - mask);


        float3 band = bandCombined * float3(0.85f, .8f, 0.4f) * 1.1f;

        // return float4(band, 1.0f);

        surface.emissiveColor = _emissiveColor;
        surface.emissiveColor *= emissiveMap.Sample(sampleTypeWrap, pIn.Tex).rgb;


        float veinsMask = Mask1.Sample(sampleTypeWrap, pIn.Tex / 32.0f - float2(0, _Time * 0.002f)).b;
        float veins =
            Mask2.Sample(sampleTypeWrap, pIn.Tex * float2(0.5f, 0.25f) + float2(0, _Time * 0.2f) + veinsMask * 2.0f).r;
        veins                = saturate(pow(veins, 4.0f));
        float3 veinsEmissive = veins * 0.1f * float3(1.0f, 1.0f, 1.0f) * mask;

        clip((1 - bandA + veinsEmissive) < 0.01f ? -1 : 1);

        color += surface.emissiveColor + band; //        +veinsEmissive;

        return float4(color, 1.0f);
}