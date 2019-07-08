#include "Interpolation.hlsl"
#include "MVPBuffer.hlsl"
#include "Samplers.hlsl"
#include "Terrain_Includes.hlsl"

struct Wave
{
        float2 dir;
        float  amplitude;
        float  waveLength;
};

#define NUM_WAVES 8

static Wave  waves[NUM_WAVES] = {{normalize(float2(1.0f, 2.0f)), 2.5f, 32.0f},
                                {normalize(float2(-1.0f, -2.0f)), 1.8f, 27.0f},
                                {normalize(float2(2.0f, 1.0f)), 3.5f, 300.0f},
                                {normalize(float2(1.0f, 0.0f)), 2.5f, 65.0f},
                                {normalize(float2(-1.0f, 0.0f)), 2.2f, 40.0f},
                                {normalize(float2(-2.0f, -1.0f)), 2.5f, 340.0f},
                                {normalize(float2(0.5f, 0.5f)), 1.0f, 160.0f},
                                {normalize(float2(1.9f, 2.3f)), 1.50f, 16.0f}};
static float steepness        = 2.2;
static float speed            = 0.5f;

float3 CalcGerstnerWaveOffset(float3 v)
{
        float  scale = gScale / 8000.0f;
        float3 sum   = float3(0, 0, 0);
        [unroll] for (int i = 0; i < NUM_WAVES; i++)
        {
                Wave  wave = waves[i];
                float wi   = 2 / (wave.waveLength * scale);
                float Qi   = steepness / (scale * wave.amplitude * wi * NUM_WAVES);
                float phi  = speed * wi;
                float rad  = dot(wave.dir, v.xz) * wi + _Time * phi;
                sum.y += sin(rad) * wave.amplitude * scale;
                sum.xz += cos(rad) * wave.amplitude * scale * Qi * wave.dir;
        }
        return sum;
}

struct DomainOutput
{
        float4 Pos : SV_POSITION;
        // TODO: change/add other stuff
        float3 PosWS : POSITION;
        float2 Tex : TEXCOORD0;
        float2 Tex2 : TEXCOORD1;
        float  linearDepth : DEPTH;
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

#define NUM_CONTROL_POINTS 4

[domain("quad")] DomainOutput main(HullConstantDataOut input, float2                                   domain
                                   : SV_DomainLocation, const OutputPatch<HullOut, NUM_CONTROL_POINTS> patch) {
        DomainOutput dOut;

        float3 pws[4]  = {patch[0].PosL, patch[1].PosL, patch[2].PosL, patch[3].PosL};
        float2 texs[4] = {patch[0].Tex, patch[1].Tex, patch[2].Tex, patch[3].Tex};

        dOut.Tex  = Bilerp(texs, domain);
        dOut.Tex2 = dOut.Tex * gTexScale;

        float3 pos = Bilerp(pws, domain);
        pos        = mul(float4(pos, 1.0f), World).xyz;
        pos.y      = 0.0f;
        // Gerstner Wave
        dOut.PosWS       = pos + CalcGerstnerWaveOffset(pos);
        dOut.Pos         = mul(float4(dOut.PosWS, 1.0f), ViewProjection);
        dOut.linearDepth = dOut.Pos.w;

        return dOut;
}
