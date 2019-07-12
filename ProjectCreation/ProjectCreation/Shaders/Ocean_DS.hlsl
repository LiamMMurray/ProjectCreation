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

#define NUM_WAVES 7

static Wave  waves[NUM_WAVES] = {{normalize(float2(1.0f, 2.0f)), 2.5f, 200.0f},
                                {normalize(float2(2.0f, 1.0f)), 3.5f, 180.0f},
                                {normalize(float2(-2.0f, 1.0f)), 2.0f, 80.0f},
                                {normalize(float2(-2.0f, 2.0f)), 2.2f, 90.0f},
                                {normalize(float2(-1.0f, 2.0f)), 1.5f, 30.0f},
                                {normalize(float2(2.0f, -1.0f)), 1.3f, 35.0f},
                                {normalize(float2(2.0f, -2.0f)), 0.5f, 16.0f}};
static float steepness        = 1.2;
static float speed            = 0.7f;


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

DomainOutput CalcGerstnerWaveOffset(float3 v)
{
        DomainOutput output = (DomainOutput)0;
        float        scale  = gScale / 8000.0f;
        output.PosWS        = v;
        output.BinormalWS   = float3(1.0f, 0.0f, 0.0f);
        output.NormalWS     = float3(0.0f, 1.0f, 0.0f);
        output.TangentWS    = float3(0.0f, 0.0f, 1.0f);
        [unroll] for (int i = 0; i < NUM_WAVES; i++)
        {
                Wave  wave = waves[i];
                float wi   = 2 / (wave.waveLength * scale);
                float Qi   = steepness / (scale * wave.amplitude * wi * NUM_WAVES);
                float phi  = speed * wi;
                float rad  = dot(wave.dir, v.xz) * wi + _Time * phi;

                float s = sin(rad);
                float c = cos(rad);

                float wa = wi * wave.amplitude * scale;

                output.PosWS.y += (s + 1.0f) * wave.amplitude * scale;
                output.PosWS.xz += (c + 1.0f) * wave.amplitude * scale * Qi * wave.dir;

                //output.NormalWS.xz -= wave.dir * wa * c;
                //output.NormalWS.y -= wa * s; //

                output.TangentWS.z -= Qi * wave.dir.y * wave.dir.x * wa * c;
                output.TangentWS.x -= Qi * wave.dir.y * wave.dir.y * wa * c;
                output.TangentWS.y += wave.dir.y * wa * c;

                output.BinormalWS.z -= Qi * wave.dir.x * wave.dir.x * wa * c;
                output.BinormalWS.x -= Qi * wave.dir.y * wave.dir.x * wa * c;
                output.BinormalWS.y += wave.dir.x * wa * c;
        }

        output.BinormalWS = normalize(output.BinormalWS);
        output.TangentWS  = normalize(output.TangentWS);
        output.NormalWS   = normalize(cross(output.TangentWS, output.BinormalWS));

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
        // Gerstner Wave
        dOut = CalcGerstnerWaveOffset(pos);

        dOut.Tex  = Bilerp(texs, domain);
        dOut.Tex2 = dOut.Tex * gTexScale;

        dOut.Pos         = mul(float4(dOut.PosWS, 1.0f), ViewProjection);
        dOut.linearDepth = dOut.Pos.w;

        return dOut;
}
