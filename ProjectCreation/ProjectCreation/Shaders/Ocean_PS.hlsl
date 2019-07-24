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

float3 PBROcean(float3 C, float N, float L, float V, float metallic, float roughness, float3 specColor)
{
        float3 H = normalize(L + V);

        // Cook-Torrance FGD/4nlnv
        float  D = NormalDistribution(N, H, roughness);
        float3 F = FresnelSchlick(H, V, specColor);
        float  G = GeometryTermSmith(N, V, L, roughness);

        float3 kD = 1.f - F;
        kD *= 1.f - metallic;

        float3 num = D * F * G;
        float  den = 4.f * max(dot(N, V), 0.f) * max(dot(N, L), 0.f);
        // limit denominator to 0.001 to prevent divisions by 0
        float3 specular = num / max(den, 0.001f);

        float NdotL = max(dot(N, L), 0.f);

        return NdotL * (kD * C / PI + specular);
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
        float  offset : TEXCOORD2;
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

        float3 normalA = normalMapA.Sample(sampleTypeWrap, pIn.Tex2 * 0.7f - 0.12f * _Time).rgb * 2.0f - 1.0f;
        normalA.z      = sqrt(1.0f - normalA.x * normalA.x - normalA.y * normalA.y);
        // normalA        = normalize(normalA);
        // return normalA.xyzz*0.5f + 0.5f;
        float3 normalB = normalMapA.Sample(sampleTypeWrap, -pIn.Tex2 * 0.8f + 0.12 * _Time).rgb * 2.0f - 1.0f;
        normalB.z      = sqrt(1 - normalB.x * normalB.x - normalB.y * normalB.y);


        float  shoreMaskA = normalMapA.Sample(sampleTypeWrap, pIn.Tex2 * 0.7f + 0.02f * _Time).r;
        float  shoreMaskB = normalMapA.Sample(sampleTypeWrap, pIn.Tex2 * 0.9f - 0.04f * _Time).g;
        float  shoreMask  = 1.0f - saturate(pow(1.0f - shoreMaskA * shoreMaskB, 24.0f) * 1000.0f);
        float2 terrainTex = pIn.PosWS.xz / (float2(gScale, gScale)) - 0.5f;


        float terrainAlpha    = pow(gTerrainAlpha, 1.2f);
        float heightmapSample = terrainAlpha * Heightmap.Sample(sampleTypeWrap, terrainTex).r * 2625.0f -
                                terrainAlpha * 1260.0f + 1.0f * terrainAlpha;
        heightmapSample -= pIn.offset * 10.0f;
        float heightMapSample2 = heightmapSample - 2.0f * (1.0f - terrainAlpha);
        heightmapSample        = saturate(shoreMask + heightMapSample2 / 8.0f) * saturate(heightmapSample / 8.0f);

        float3     radiance = _DirectionalLightColor.xyz;
        SurfacePBR surface;
        float3     deepWaterColor    = float3(0.0f, 0.2f, 0.6f);
        float3     shallowWaterColor = float3(0.0f, 0.5f, 0.4f);

        surface.metallic      = 0.0f;
        surface.emissiveColor = 0.0f;
        surface.ambient       = 1.0f;

        {
                normalSample = normalize((normalA + normalB));
                // surface.normal = NormalWS;
                surface.normal = 1.0f * (normalSample.x * TangentWS + normalSample.y * BinormalWS) + normalSample.z * NormalWS;
                // surface.normal.xy += 1.5f * (normalSample.xy);
        }
        surface.normal = normalize(surface.normal);

        float foamAlpha = 1.0f - NormalWS.y;
        foamAlpha       = 1.0f - saturate(pow(foamAlpha * 150.0f, 1.0f));
        foamAlpha *= pIn.offset / 2.0f - 0.2f;
        // return foamAlpha;
        float shoreMaskC = normalMapA.Sample(sampleTypeWrap, pIn.Tex2 * 0.15f - float2(0.0f, 1.0f) * 0.01f * _Time).g;
        shoreMaskC       = saturate(pow(shoreMaskC, 18.0f) * 2000.0f);
        // return shoreMaskC;
        float wavesMask = 1.0f - saturate(pow(1.0f - shoreMaskA * shoreMaskB, lerp(8.0f, 45.0f, foamAlpha)) * 3000.0f);
        wavesMask *= shoreMaskC;
        // viewWS = normalize(lerp(viewWS, reflect(_DirectionalLightDirection, surface.normal), 0.8f));
        // return normalB.xyzz;
        // return TangentWS.xxxx;
        // surface.normal = float3(0.0f, 1.0f, 0.0f);
        // surface.normal = float3(0.0f, 1.0f, 0.0f);
        // return surface.normal.xyzz;
        float alpha = 1.f;
        // Remapping roughness to prevent errors on normal distribution
        surface.roughness = 0.5f;
        surface.roughness = max(surface.roughness, 0.08f);
        surface.metallic  = 0.0f;
        // surface.ambient = lerp(roughnessSample.b, roughnessSample.a, blendAlpha);

        float3 reflectionVector = reflect(-viewWS, surface.normal);


        float3 positionWS = pIn.PosWS;
        float3 color      = surface.emissiveColor; // surface.ambient * lightInfo.ambientColor * surface.diffuseColor +

        // Non metals use a constant for this value
        float3 specColor = 0.04f;
        // Lerp between diffuse color and constant based on metallic. Ideally metallic should be either 1 or 0
        specColor = lerp(specColor, surface.diffuseColor, surface.metallic);


        float3 N = surface.normal;
        float3 L = -_DirectionalLightDirection;
        float3 V = viewWS;
        float3 H = normalize(L + V);


        float transluscentHigh = saturate(dot(N, H));
        float transluscentLow  = saturate(dot(NormalWS, H));


        float fresnelLow  = 1.0f - saturate(dot(NormalWS, V));
        float fresnelHigh = 1.0f - saturate(dot(N, V));

        // return fresnel;
        // return fresnelLow;
        float transluscencyExtra = saturate(pow(lerp(fresnelHigh, fresnelLow, 0.7f), 2.0f) * 0.08f);
        float transluscency = saturate(pow(lerp(transluscentHigh, transluscentLow, 0.7f), 15.0f) * 1.0f + transluscencyExtra);
        float fresnel       = 0.04f * saturate(pow(fresnelLow + fresnelHigh * 0.1f, 25.0f));
        // return fresnel;
        // return transluscency;
        float specular = pow(max(dot(H, N), 0.0), 250.0) * 0.2f;
        // return depth;
        // Directional Light
        {
                // Directional Light translucency
                float heightBlend = saturate(pIn.offset / 4.0f);

                color += heightmapSample;

                // return slopeAlpha;
                color += wavesMask;
                color += lerp((deepWaterColor),
                              shallowWaterColor * radiance,
                              saturate(transluscency * 0.5f + heightBlend * 0.2f + shoreMaskC * 0.01f));
                color = saturate(color);


                // Sun specular
                color += specular * radiance;

                // Ambient specular
                color += fresnel * _AmbientColor * surface.ambient;

                // color += lerp(shallowWaterColor, deepWaterColor, fresnelTerm) + specular * radiance;
        }
        // return NormalWS.xyz;
        //return float4(color, 1.0f);

        float maskA     = Mask1.Sample(sampleTypeWrap, pIn.PosWS.xz / 45.0f + _Time * 0.01f * float2(1.0f, 0.0f)).z;
        float maskB     = Mask1.Sample(sampleTypeWrap, pIn.PosWS.xz / 40.0f + _Time * 0.01f * float2(-1.0f, 0.0f)).z;
        float areaMaskA = Mask1.Sample(sampleTypeWrap, pIn.PosWS.xz / 8.0f + maskA + maskB).z * 2.0f - 1.0f;
        // return areaMaskA;

        float3 dirVec = pIn.PosWS + float3(areaMaskA, 0, areaMaskA) - _EyePosition;
        float  dist   = sqrt(dot(dirVec.xz, dirVec.xz));

        float modulatedDistance = dist / .1f + InterleavedGradientNoise(pIn.Pos.xy + _Time);

        float mask         = saturate(dist / 0.1f - _playerRadius*2.0f);
        float bandA        = saturate(dist / 0.1f - _playerRadius*2.0f + 0.5f);
        float bandB        = saturate(dist / 0.1f - _playerRadius*2.0f - 0.5f);
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