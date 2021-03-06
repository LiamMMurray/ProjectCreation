#include "Constants.hlsl"
#include "Interpolation.hlsl"
#include "Samplers.hlsl"
#include "Terrain_Includes.hlsl"
#include "DoSpeedWave.hlsli"
struct Wave
{
        float2 dir;
        float  amplitude;
        float  waveLength;
};

#define NUM_WAVES 8

static Wave  waves[8]  = {{normalize(float2(1.0f, 2.0f)), 5.5f, 200.0f},
                        {normalize(float2(1.0f, 1.0f)), 5.5f, 180.0f},
                        {normalize(float2(-2.0f, 1.0f)), 3.5f, 80.0f},
                        {normalize(float2(-1.0f, 2.0f)), 3.7f, 90.0f},
                        {normalize(float2(2.0f, 2.4f)), 2.0f, 30.0f},
                        {normalize(float2(2.0f, 1.0f)), 1.8f, 35.0f},
                        {normalize(float2(2.0f, 2.0f)), 0.3f, 12.0f},
                        {normalize(float2(2.0f, 1.4f)), 0.3f, 10.0f}};
static float steepness = 1.3;
static float speed     = 0.75f;


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

// Output control point
struct HullOut
{
        float3 PosL : POSITION;
        float2 Tex : TEXCOORD0;
};

// Output patch constant data.
struct HullConstantDataOut
{
        float EdgeTessFactor[4] : SV_TessFactor;         // e.g. would be [4] for a quad domain
        float InsideTessFactor[2] : SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
                                                         // TODO: change/add other stuff
};

float3 GerstnerWaveNormal(float2 D, float WA, float Q, float C, float S)
{
        float  WAC    = WA * C;
        float3 normal = float3(-D.x * WAC, 1.0 - Q * WA * S, -D.y * WAC);
        return normalize(normal.xyz);
}

float3 GerstnerWaveTangent(float2 D, float WA, float Q, float C, float S)
{
        float  WAS    = WA * S;
        float3 normal = float3(Q * -D.x * D.y * WAS, D.y * WA * C, 1.0 - Q * D.y * D.y * WAS);
        return normalize(normal.xyz);
}

DomainOutput CalcGerstnerWaveOffset(float3 v)
{
        DomainOutput output = (DomainOutput)0;
        float        scale  = gScale / 8000.0f;
        float3       inPos  = v - gOriginOffset;
        output.Tex          = inPos.xz / (float2(gScale, gScale)) - 0.5f;
        output.PosWS        = inPos;
        output.NormalWS = output.TangentWS = float3(0.0f, 1.0f, 0.0f);

        float dist          = distance(_EyePosition, v) - 50.0f;
        float distanceBlend = 1.0f - saturate(dist / 200.0f);


        [unroll] for (int i = 0; i < NUM_WAVES; i++)
        {
                float distanceFactor = saturate(i / (NUM_WAVES - 6));
                float alpha          = lerp(1.0f, distanceBlend, distanceFactor);

                Wave  wave = waves[i];
                float wi   = 2 / (wave.waveLength * scale);
                float Qi   = steepness / (scale * wave.amplitude * wi * NUM_WAVES);
                float phi  = speed * wi;
                // phi = PI;
                float rad = dot(wave.dir, inPos.xz) * wi + _Time * phi;

                // s =
                float s = sin(rad);
                float c = cos(rad);

                float wa = wi * wave.amplitude * scale;

                output.PosWS.y += alpha * (s + 1.0f) * wave.amplitude * scale;
                output.PosWS.xz += alpha * (c + 1.0f) * wave.amplitude * scale * Qi * wave.dir;

                output.NormalWS += alpha * wave.amplitude * GerstnerWaveNormal(wave.dir, wa, Qi, c, s);
                output.TangentWS += alpha * GerstnerWaveTangent(wave.dir, wa, Qi, c, s);
        }

        output.TangentWS  = normalize(output.TangentWS);
        output.NormalWS   = normalize(output.NormalWS);
        output.BinormalWS = normalize(cross(output.NormalWS, output.TangentWS));
        output.offset     = output.PosWS.y;

        return output;
}

#define NUM_CONTROL_POINTS 4

[domain("quad")] DomainOutput main(HullConstantDataOut input, float2                                   domain
                                   : SV_DomainLocation, const OutputPatch<HullOut, NUM_CONTROL_POINTS> patch) {
        DomainOutput dOut;

        float3 pws[4]  = {patch[0].PosL, patch[1].PosL, patch[2].PosL, patch[3].PosL};
        float2 texs[4] = {patch[0].Tex, patch[1].Tex, patch[2].Tex, patch[3].Tex};


        float3 pos = Bilerp(pws, domain);
        pos        = mul(float4(pos, 1.0f), World).xyz;
        pos.y      = 0.0f;

        pos  = DoSpeedWave(pos, 1.5f);
        dOut = CalcGerstnerWaveOffset(pos);
        // dOut.Tex  = Bilerp(texs, domain);
        dOut.Tex2 = dOut.Tex * gTexScale;

        dOut.PosWS += gOriginOffset;

        dOut.Pos = mul(float4(dOut.PosWS, 1.0f), ViewProjection);


        dOut.linearDepth = dOut.Pos.w;

        return dOut;
}
