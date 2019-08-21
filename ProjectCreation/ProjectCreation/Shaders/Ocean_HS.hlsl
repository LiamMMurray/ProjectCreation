#include "Terrain_Includes.hlsl"
// Input control point

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

struct VertexOut
{
        float3 PosL : POSITION0;
        float3 PosW : POSITION1;
        float2 Tex : TEXCOORD0;
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

float GetPostProjectionSphereExtent(float3 Origin, float Diameter)
{
        float4 ClipPos = mul(float4(Origin, 1.0f), Projection);
        return abs((Diameter * Projection[0][0]) / ClipPos.w);
}

float2 eyeToScreen(float4 p)
{
        float4 r = mul(p, Projection);
        r.xy /= r.w;                                            // project
        r.xy = r.xy * 0.5 + 0.5;                                // to NDC
        r.xy *= float2(screenDimensions.x, screenDimensions.y); // to pixels
        return r.xy;
}

float CalculateTessellationFactor(float3 Control0, float3 Control1)
{
        float3 w0 = mul(float4(Control0, 1.0), World).xyz;
        float3 w1 = mul(float4(Control1, 1.0), World).xyz;

        float3 wC   = (w0 + w1) / 2;
        float  dist = distance(_EyePosition, wC);

        float alpha = saturate(3.0f * dist / gScale);

        return lerp(64.0f, 4.0f, alpha);
}

bool AABBToPlane(float3 center, float3 extents, float4 plane)
{
        float3 n = abs(plane.xyz);
        float  r = dot(extents, n);
        float  s = dot(center, plane.xyz) - plane.w;
        return (s + r) < 0.0f;
}

bool AABBToFrustum(float3 center, float3 extents)
{
        for (int i = 0; i < 6; ++i)
        {
                if (AABBToPlane(center, extents, gWorldFrustumPlanes[i]))
                {
                        return true;
                }
        }

        return false;
}

float3 CalcGerstnerWaveOffset(float3 v)
{
        float  scale  = gScale / 8000.0f;
        float3 inPos  = v - gOriginOffset;
        float3 output = inPos;

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

                output.xz += alpha * (c + 1.0f) * wave.amplitude * scale * Qi * wave.dir;
        }
        output += gOriginOffset;
        return output;
}

// Patch Constant Function
HullConstantDataOut CalcHSPatchConstants(InputPatch<VertexOut, NUM_CONTROL_POINTS> ip, uint PatchID : SV_PrimitiveID)
{
        HullConstantDataOut hCDOut;

        float minY = -30.0f;
        float maxY = 20.0f;

        float3 vMin = ip[0].PosW;
        float3 vMax = ip[0].PosW;
        for (int i = 1; i < 4; ++i)
        {
                vMin = min(vMin, ip[i].PosW);
                vMax = max(vMax, ip[i].PosW);
        }

        vMin = CalcGerstnerWaveOffset(vMin);
        vMax = CalcGerstnerWaveOffset(vMax);

        vMin.y = minY;
        vMax.y = maxY;

        float3 boxCenter  = 0.5f * (vMin + vMax);
        float3 boxExtents = 0.5f * (vMax - vMin);
        if (AABBToFrustum(boxCenter, boxExtents))
        {
                hCDOut.EdgeTessFactor[0] = 0.0f;
                hCDOut.EdgeTessFactor[1] = 0.0f;
                hCDOut.EdgeTessFactor[2] = 0.0f;
                hCDOut.EdgeTessFactor[3] = 0.0f;

                hCDOut.InsideTessFactor[0] = 0.0f;
                hCDOut.InsideTessFactor[1] = 0.0f;

                return hCDOut;
        }
        else
        {
                hCDOut.EdgeTessFactor[0] = CalculateTessellationFactor(ip[0].PosL, ip[3].PosL);
                hCDOut.EdgeTessFactor[1] = CalculateTessellationFactor(ip[0].PosL, ip[1].PosL);
                hCDOut.EdgeTessFactor[2] = CalculateTessellationFactor(ip[1].PosL, ip[2].PosL);
                hCDOut.EdgeTessFactor[3] = CalculateTessellationFactor(ip[3].PosL, ip[2].PosL);

                hCDOut.InsideTessFactor[0] = 0.25 * (hCDOut.EdgeTessFactor[0] + hCDOut.EdgeTessFactor[1] +
                                                     hCDOut.EdgeTessFactor[2] + hCDOut.EdgeTessFactor[3]);
                hCDOut.InsideTessFactor[1] = hCDOut.InsideTessFactor[0];

                return hCDOut;
        }
}

[domain("quad")][partitioning("fractional_even")][outputtopology("triangle_cw")][outputcontrolpoints(4)]
                [patchconstantfunc("CalcHSPatchConstants")][maxtessfactor(64.0f)] HullOut
                main(InputPatch<VertexOut, NUM_CONTROL_POINTS> ip, uint i
                     : SV_OutputControlPointID, uint                    PatchID
                     : SV_PrimitiveID)
{
        HullOut hOut;
        // Insert code to compute Output here
        hOut.PosL = ip[i].PosL;
        hOut.Tex  = ip[i].Tex;
        return hOut;
}
