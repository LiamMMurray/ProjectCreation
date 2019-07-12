Texture2D   diffuseMapA : register(t0);
Texture2D   normalMapA : register(t1);
Texture2D   diffuseMapB : register(t2);
Texture2D   normalMapB : register(t3);
TextureCube IBLDiffuse : register(t4);
TextureCube IBLSpecular : register(t5);
Texture2D   IBLIntegration : register(t6);
Texture2D   Mask1 : register(t7);
Texture2D   Mask2 : register(t8);
Texture2D   Heightmap : register(t9);
Texture2D   Roughness : register(t10);

#include "Constants.hlsl"
#include "Math.hlsl"
#include "Samplers.hlsl"
#include "Terrain_Includes.hlsl"


#include "PBRMath.hlsl"

float3 PBROcean(SurfacePBR surface, float3 lightDir, float3 viewWS, float3 specColor)
{
        float3 N = surface.normal;
        float3 L = lightDir;
        float3 V = normalize(lerp(viewWS, reflect(-lightDir, surface.normal), 0.3f));
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

struct DomainOutput
{
        float4 Pos : SV_POSITION;
        // TODO: change/add other stuff
        float3 PosWS : POSITION;
        float3 NormalWS : NORMAL;
        float3 BinormalWS : BINORMAL;
        float3 TangentWS : TANGENT;
        float2 Tex : TEXCOORD0;
        float2 Tex2 : TEXCOORD1;
        float  linearDepth : DEPTH;
};


float4 main(DomainOutput pIn) : SV_TARGET
{

        float3 TangentWS  = pIn.TangentWS;
        float3 BinormalWS = pIn.BinormalWS;
        float3 NormalWS   = pIn.NormalWS;

        float3 viewWS = -normalize(pIn.PosWS - _EyePosition);
        float3 normalSample;

        float3 diffuseA = diffuseMapA.Sample(sampleTypeWrap, pIn.Tex2).rgb;
        float3 diffuseB = diffuseMapB.Sample(sampleTypeWrap, pIn.Tex2).rgb;

        float3 normalA = normalMapA.Sample(sampleTypeWrap, pIn.Tex2 * 0.8f - 0.025f * _Time).rgb;
        normalA.z      = sqrt(1 - normalA.x * normalA.x - normalA.y * normalA.y);
        float3 normalB = normalMapA.Sample(sampleTypeWrap, pIn.Tex2 * 0.6f + 0.022f * _Time).rgb;
        normalB.z      = sqrt(1 - normalB.x * normalB.x - normalB.y * normalB.y);

        SurfacePBR surface;
        surface.diffuseColor =
            0.8f * lerp(float3(0.0f, 0.8f, 0.6f), float3(0.0f, 0.2f, 0.4f), 1.0f - saturate((pIn.PosWS.y - 0.9f) / 0.6f));

        surface.metallic = 0.0f;

        surface.ambient       = 1.0f;
        surface.emissiveColor = 0.0f;


        {
                normalSample = normalize((normalA + normalB));
                // surface.normal = NormalWS;
                surface.normal = 1.0f * (normalSample.x * TangentWS + normalSample.y * BinormalWS) + normalSample.z * NormalWS;
                // surface.normal.xy += 1.5f * (normalSample.xy);
        }
        surface.normal = normalize(surface.normal);
        return normalB.xyzz;
        // return TangentWS.xxxx;
        // surface.normal = float3(0.0f, 1.0f, 0.0f);
        // surface.normal = float3(0.0f, 1.0f, 0.0f);
        // return surface.normal.xyzz;
        float alpha = 1.f;
        // Remapping roughness to prevent errors on normal distribution
        surface.roughness = 0.4f;
        surface.roughness = max(surface.roughness, 0.08f);

        // surface.ambient = lerp(roughnessSample.b, roughnessSample.a, blendAlpha);

        float3 reflectionVector = reflect(-viewWS, surface.normal);


        float3 positionWS = pIn.PosWS;
        float3 color      = surface.emissiveColor; // surface.ambient * lightInfo.ambientColor * surface.diffuseColor +

        // Non metals use a constant for this value
        float3 specColor = 0.04f;
        // Lerp between diffuse color and constant based on metallic. Ideally metallic should be either 1 or 0
        specColor = lerp(specColor, surface.diffuseColor, surface.metallic);


        // Directional Light
        {
                float3 radiance = _DirectionalLightColor.xyz;
                color += radiance * PBROcean(surface, -_DirectionalLightDirection, viewWS, specColor);
        }
        surface.roughness = 0.0f;

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

        float veinsMask = Mask1.Sample(sampleTypeWrap, pIn.Tex / 32.0f - float2(0, _Time * 0.002f)).b;
        float veins =
            Mask2.Sample(sampleTypeWrap, pIn.Tex * float2(0.5f, 0.25f) + float2(0, _Time * 0.2f) + veinsMask * 2.0f).r;
        veins                = saturate(pow(veins, 4.0f));
        float3 veinsEmissive = veins * 0.1f * float3(1.0f, 1.0f, 1.0f) * mask;

        clip((1 - bandA + veinsEmissive) < 0.01f ? -1 : 1);

        color += surface.emissiveColor + band;

        return float4(color, 1.0f);
}