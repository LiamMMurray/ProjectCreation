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

struct DomainOutput
{
        float4 Pos : SV_POSITION;
        // TODO: change/add other stuff
        float3 PosWS : POSITION;
        float2 Tex : TEXCOORD0;
        float2 Tex2 : TEXCOORD1;
        float  linearDepth : DEPTH;
};


float4 main(DomainOutput pIn) : SV_TARGET
{
        float2 leftTex   = pIn.Tex + float2(-gTexelSize, 0.0f);
        float2 rightTex  = pIn.Tex + float2(gTexelSize, 0.0f);
        float2 bottomTex = pIn.Tex + float2(0.0f, -gTexelSize);
        float2 topTex    = pIn.Tex + float2(0.0f, +gTexelSize);

        float3 dpdx = ddx(pIn.PosWS);
        float3 dpdy = ddy(pIn.PosWS);

        float leftY   = gTerrainAlpha * 2625.f * HeightMap.SampleLevel(sampleTypeWrap, leftTex, 0).r;
        float rightY  = gTerrainAlpha * 2625.f * HeightMap.SampleLevel(sampleTypeWrap, rightTex, 0).r;
        float bottomY = gTerrainAlpha * 2625.f * HeightMap.SampleLevel(sampleTypeWrap, bottomTex, 0).r;
        float topY    = gTerrainAlpha * 2625.f * HeightMap.SampleLevel(sampleTypeWrap, topTex, 0).r;

        float3 TangentWS  = normalize(float3(2.5f * gWorldCellSpace, rightY - leftY, 0.0f));
        float3 BinormalWS = normalize(float3(0.0f, bottomY - topY, -2.5f * gWorldCellSpace));
        float3 NormalWS   = normalize(cross(TangentWS, BinormalWS));

        float3 viewWS = -normalize(pIn.PosWS - _EyePosition);
        float3 normalSample;

        float blendAlpha = saturate(dot(NormalWS, float3(0.0f, 1.0f, 0.0f)) + 0.05f);
        blendAlpha       = pow(blendAlpha, 50.0f);
        blendAlpha       = saturate(blendAlpha * 50.0f);

        float3 diffuseA = diffuseMapA.Sample(sampleTypeWrap, pIn.Tex2).rgb;
        float3 diffuseB = diffuseMapB.Sample(sampleTypeWrap, pIn.Tex2).rgb;

        float3 normalA = normalMapA.Sample(sampleTypeWrap, pIn.Tex2).rgb;
        float3 normalB = normalMapB.Sample(sampleTypeWrap, pIn.Tex2).rgb;

        SurfacePBR surface;
        surface.diffuseColor = lerp(diffuseA, diffuseB, blendAlpha);
        surface.metallic     = 0.0f;

        surface.ambient       = 1.0f;
        surface.emissiveColor = 0.0f;


        {
                normalSample   = lerp(normalA, normalB, blendAlpha) * 2.0f - 1.0f;
                normalSample.z = sqrt(1 - normalSample.x * normalSample.x - normalSample.y * normalSample.y);
                normalSample   = normalize(normalSample);
                surface.normal = TangentWS * normalSample.x + BinormalWS * normalSample.y + NormalWS * normalSample.z;
        }
        surface.normal = normalize(surface.normal);
        // surface.normal = NormalWS;
        // return surface.normal.xyzz;
        float alpha = 1.f;

        float4 roughnessSample = Roughness.Sample(sampleTypeWrap, pIn.Tex2).rgba;
        surface.roughness      = lerp(roughnessSample.r, roughnessSample.g, blendAlpha);
        // Remapping roughness to prevent errors on normal distribution
        surface.roughness = max(surface.roughness, 0.08f);

        surface.ambient = lerp(roughnessSample.b, roughnessSample.a, blendAlpha);

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
                color += radiance * PBR(surface, -_DirectionalLightDirection, viewWS, specColor);
        }

        // Environment mapping
        {
                surface.ambient *= 0.4f;
                float3 N = surface.normal;
                float3 V = -viewWS;

                float  NdotV       = max(dot(N, V), 0.f);
                float3 diffuse     = IBLDiffuse.Sample(sampleTypeWrap, N).rgb;
                float3 specular    = IBLSpecular.SampleLevel(sampleTypeWrap, reflectionVector, surface.roughness * 10.f).rgb;
                float2 integration = IBLIntegration.Sample(sampleTypeNearest, float2(NdotV, 1.f - surface.roughness)).rg;
                color += IBL(surface, viewWS, specColor, diffuse, specular, integration);
        }
        //return NormalWS.xxxx;
        return float4(color, 1.0f);

        float maskA     = Mask1.Sample(sampleTypeWrap, pIn.PosWS.xz / 45.0f + _Time * 0.01f * float2(1.0f, 0.0f)).z;
        float maskB     = Mask1.Sample(sampleTypeWrap, pIn.PosWS.xz / 40.0f + _Time * 0.01f * float2(-1.0f, 0.0f)).z;
        float areaMaskA = Mask1.Sample(sampleTypeWrap, pIn.PosWS.xz / 8.0f + maskA + maskB).z * 2.0f - 1.0f;
        // return areaMaskA;

        float3 dirVec = pIn.PosWS + float3(areaMaskA, 0, areaMaskA) - _EyePosition;
        float  dist   = sqrt(dot(dirVec.xz, dirVec.xz));

        float modulatedDistance = dist / .1f + InterleavedGradientNoise(pIn.Pos.xy + _Time);

        float playerRadius = _playerRadius * gTerrainAlpha;
        float mask         = saturate(dist / 0.1f - playerRadius * 2.0f);
        float bandA        = saturate(dist / 0.1f - playerRadius * 2.0f + 0.5f);
        float bandB        = saturate(dist / 0.1f - playerRadius * 2.0f - 0.5f);
        float bandCombined = saturate(5.0f * (bandA - bandB));
        color *= (1 - mask);


        float3 band = bandCombined * float3(0.85f, .8f, 0.4f) * 1.1f;

        float veinsMask = Mask1.Sample(sampleTypeWrap, pIn.Tex / 32.0f - float2(0, _Time * 0.002f)).b;
        float veins =
            Mask2.Sample(sampleTypeWrap, pIn.Tex * float2(0.5f, 0.25f) + float2(0, _Time * 0.2f) + veinsMask * 2.0f).r;
        veins                = saturate(pow(veins, 4.0f));
        float3 veinsEmissive = veins * 0.1f * float3(1.0f, 1.0f, 1.0f) * mask;

        clip((1 - bandA) < 0.01f ? -1 : 1);

        color += surface.emissiveColor + band;
        float revealAlpha = _playerRadius / 500.0f;
        revealAlpha       = saturate(pow(revealAlpha, 2.0f));

        return float4(color * revealAlpha, 1.0f);
}