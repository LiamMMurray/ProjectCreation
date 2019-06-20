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

struct SurfacePBR
{
        float3 diffuseColor;
        float  metallic;
        float3 emissiveColor;
        float  specular;
        float  roughness;
        float3 normal;
        float  ambient;
};

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


// using Schlick's approximation for calculating fresnel. K is a constant for non metallics
float3 FresnelSchlick(float3 H, float3 V, float3 specColor)
{
        float cosAngle = max(dot(H, V), 0.f);

        return specColor + (1.f - specColor) * pow(1.f - cosAngle, 5.f);
}

// Variation using only roughness for IBL calculations
float3 FresnelSchlickEnvironment(float3 N, float3 V, float3 specColor, float roughness)
{
        float cosAngle = max(dot(N, V), 0.f);

        return specColor + (max(1.f - roughness, specColor) - specColor) * pow(1.f - cosAngle, 5.f);
}

// Specular based on Cook-Torrance model
float NormalDistribution(float3 N, float3 H, float roughness)
{
        float a      = roughness * roughness;
        float a2     = a * a;
        float NdotH  = max(dot(N, H), 0.f);
        float NdotH2 = NdotH * NdotH;

        float d = (NdotH2 * (a2 - 1.f) + 1.f);
        d       = PI * d * d;

        return a2 / d;
}


float GeometryTermSchlick(float NdotV, float roughness)
{
        float r = (roughness + 1.f);
        float k = (r * r) / 8.f;

        float num = NdotV;
        float den = NdotV * (1.f - k) + k;

        return num / den;
}
float GeometryTermSmith(float3 N, float3 V, float3 L, float roughness)
{
        float NdotV = max(dot(N, V), 0.f);
        float NdotL = max(dot(N, L), 0.f);
        float term1 = GeometryTermSchlick(NdotL, roughness);
        float term2 = GeometryTermSchlick(NdotV, roughness);

        return term1 * term2;
}

float3 IBL(SurfacePBR surface, float3 viewWS, float3 specColor, float3 IBLDiffuse, float3 IBLSpecular, float2 IBLIntegration)
{
        float3 N = surface.normal;
        float3 V = -viewWS;

        float3 F  = FresnelSchlickEnvironment(N, V, specColor, surface.roughness);
        float3 kD = 1.f - F;
        kD *= 1.f - surface.metallic;

        float3 diffuse  = surface.diffuseColor * IBLDiffuse;
        float3 specular = IBLSpecular * (F * IBLIntegration.x + IBLIntegration.y);

        return (kD * diffuse + specular) * surface.ambient * _AmbientColor;
}

float3 PBR(SurfacePBR surface, float3 lightDir, float3 viewWS, float3 specColor)
{
        float3 N = surface.normal;
        float3 L = lightDir;
        float3 V = -viewWS;
        float3 H = normalize(L + V);

        // Cook-Torrance FGD/4nlnv
        float  D = NormalDistribution(N, H, surface.roughness);
        float3 F = FresnelSchlick(H, V, specColor);
        float  G = GeometryTermSmith(N, V, L, surface.roughness);

        float3 kD = 1.f - F;
        kD *= 1.f - surface.metallic;

        float3 num = D * F * G;
        float  den = 4.f * max(dot(N, V), 0.f) * max(dot(N, L), 0.f);
        // limit denominator to 0.001 to prevent divisions by 0
        float3 specular = num / max(den, 0.001f);

        float NdotL = max(dot(N, L), 0.f);

        return NdotL * (kD * surface.diffuseColor / PI + specular);
}

float4 main(INPUT_PIXEL pIn) : SV_TARGET
{
        float3 viewWS = normalize(pIn.PosWS - _EyePosition);
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

                if (IsMasked(_textureFlags))
                {
                        clip(diffuse.a < 0.1f ? -1 : 1);
                }

                surface.diffuseColor *= diffuse.xyz;
                if (IsTransluscent(_textureFlags))
                {
                        clip(diffuse.a < 0.01f ? -1 : 1);
                        surface.diffuseColor *= diffuse.w;
                        alpha = diffuse.w;
                }
        }
        surface.diffuseColor = saturate(surface.diffuseColor);

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
        surface.roughness = min(surface.roughness, 0.08f);
        surface.metallic  = 1.0f;
        surface.diffuseColor   = float3(0.1f, 0.2f, 0.3f);
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
                float3 V = -viewWS;

                float  NdotV       = max(dot(N, V), 0.f);
                float3 diffuse     = IBLDiffuse.Sample(sampleTypeWrap, N).rgb;
                float3 specular    = IBLSpecular.SampleLevel(sampleTypeWrap, reflectionVector, surface.roughness * 10.f).rgb;
                float2 integration = IBLIntegration.Sample(sampleTypeNearest, float2(NdotV, 1.f - surface.roughness)).rg;
                color += IBL(surface, viewWS, specColor, diffuse, specular, integration);
        }

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

        color += surface.emissiveColor + band + veinsEmissive;

        return float4(color, 1.0f);
}