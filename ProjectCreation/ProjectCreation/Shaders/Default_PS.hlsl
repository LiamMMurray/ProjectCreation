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
        float  _Pad;
        float4 _DirectionalLightColor;
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

        return (kD * diffuse + specular) * surface.ambient;
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

        surface.ambient = _ambientIntensity;


        float4 diffuse = diffuseMap.Sample(sampleTypeWrap, pIn.Tex);

        surface.diffuseColor *= diffuse.xyz;

        surface.diffuseColor = saturate(surface.diffuseColor);
        surface.metallic *= detailsMap.Sample(sampleTypeWrap, pIn.Tex).y;
        surface.roughness = lerp(_roughnessMin, _roughnessMax, detailsMap.Sample(sampleTypeWrap, pIn.Tex).x);
        // Remapping roughness to prevent errors on normal distribution
        surface.roughness = remap(surface.roughness, 0.0f, 1.0f, 0.08f, 1.0f);

        normalSample   = normalMap.Sample(sampleTypeWrap, pIn.Tex).xyz * 2.f - 1.f;
        normalSample.z = sqrt(1 - normalSample.x * normalSample.x - normalSample.y * normalSample.y);
        surface.normal = _normalIntensity * pIn.TangentWS * normalSample.x +
                         _normalIntensity * pIn.BinormalWS * normalSample.y + pIn.NormalWS * normalSample.z;

        surface.normal = normalize(surface.normal);

        float3 reflectionVector = reflect(viewWS, surface.normal);

        float3 positionWS = pIn.PosWS;

        float3 color = 0; // surface.ambient * lightInfo.ambientColor * surface.diffuseColor +
        // Non metals use a constant for this value
        float3 specColor = _specular;
        // Lerp between diffuse color and constant based on metallic. Ideally metallic should be either 1 or 0
        specColor = lerp(specColor, surface.diffuseColor, surface.metallic);

        // Directional Light
        {
                float3 radiance = _DirectionalLightColor.xyz * _DirectionalLightColor.w;
                color += radiance * PBR(surface, -_DirectionalLightDirection, viewWS, specColor);
        }

        // Environment mapping
        {
                surface.ambient *= 0.6f;
                float3 N = surface.normal;
                float3 V = -viewWS;

                float  NdotV       = max(dot(N, V), 0.f);
                float3 diffuse     = IBLDiffuse.Sample(sampleTypeWrap, N).rgb;
                float3 specular    = IBLSpecular.SampleLevel(sampleTypeWrap, reflectionVector, surface.roughness * 10.f).rgb;
                float2 integration = IBLIntegration.Sample(sampleTypeNearest, float2(NdotV, 1.f - surface.roughness)).rg;
                color += IBL(surface, viewWS, specColor, diffuse, specular, integration) * surface.ambient;
        }

        float maskX =
            saturate(Mask1.Sample(sampleTypeWrap, pIn.PosWS.xz / 32.0f + _Time * 0.005f * float2(1.0f, 0.0f)).z * 5.0f);
        float maskY =
            saturate(Mask1.Sample(sampleTypeWrap, pIn.PosWS.xz / 28.0f - _Time * 0.005f * float2(0.0f, 1.0f)).z * 5.0f);
        float maskSample2 = Mask1.Sample(sampleTypeWrap, pIn.PosWS.xz / 16.0f + float3(maskX, 0, maskY)).x * 2.0f;

        // return maskSample;

        float3 dirVec = pIn.PosWS + float3(maskSample2, 0, maskSample2) - _EyePosition;
        float  dist   = sqrt(dot(dirVec, dirVec));

        float  mask     = saturate(dist / 0.5f - 4.0f);
        float  inv_mask = 1.0f - mask;
        float3 band     = inv_mask - floor(inv_mask) + mask - floor(mask);

        color *= saturate(inv_mask + color - 1.0f);

        band = saturate(band) * float3(0.85f, .8f, 0.4f);

        surface.emissiveColor = _emissiveColor;
        surface.emissiveColor *= emissiveMap.Sample(sampleTypeWrap, pIn.Tex).rgb;


        float veinsMask = Mask1.Sample(sampleTypeWrap, pIn.Tex / 32.0f - float2(0, _Time * 0.002f)).b;
        float veins =
            Mask2.Sample(sampleTypeWrap, pIn.Tex * float2(0.5f, 0.25f) + float2(0, _Time * 0.2f) + veinsMask * 8.0f).r;
        float3 veinsEmissive = veins * 5.0f * float3(1.0f, 0.05f, 0.05f) * mask ;

        color += surface.emissiveColor + band + veinsEmissive;

        return float4(color, 1.0f);
}