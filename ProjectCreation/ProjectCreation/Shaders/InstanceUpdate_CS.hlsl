#include "InstanceData.hlsl"
#include "Math.hlsl"
#include "Samplers.hlsl"
#include "Terrain_Includes.hlsl"
#include "Wrap.hlsl"
Texture2D MaskMap : register(t10);


RWStructuredBuffer<FInstanceData> InstanceTransforms : register(u0);
AppendStructuredBuffer<uint>      InstanceIndicesSteep : register(u1);
AppendStructuredBuffer<uint>      InstanceIndicesFlat : register(u2);

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

[numthreads(1, 1, 1)] void main(uint3 DTid
                                : SV_DispatchThreadID) {
        uint   id    = DTid.x;
        float3 pos   = float3(InstanceTransforms[id].mtx._41, InstanceTransforms[id].mtx._42, InstanceTransforms[id].mtx._43);
        float  scale = gScale / 2.0f;

        float2 MinLocal  = float2(-0.5f * scale, -0.5f * scale);
        float2 MinGlobal = float2(-0.5f * gScale, -0.5f * gScale);
        // Min        = float2(-10.0f, -10.0f);
        float2 MaxLocal  = -MinLocal;
        float2 MaxGlobal = -MinGlobal;


        pos.xz                         = wrap(pos.xz, _EyePosition.xz + MinGlobal, _EyePosition.xz + MaxGlobal);
        InstanceTransforms[id].mtx._41 = pos.x;
        InstanceTransforms[id].mtx._43 = pos.z;
        float3 prevPos                 = pos;
        pos.xz                         = wrap(pos.xz, _EyePosition.xz + MinLocal, _EyePosition.xz + MaxLocal);

        float2 tex = pos.xz / (float2(gScale, gScale)) - 0.5f;

        float heightSample = HeightMap.SampleLevel(sampleTypeWrap, tex, 0).r * 2625.f;
        pos.y              = heightSample * (gTerrainAlpha)-1260.0f * (gTerrainAlpha);
        pos.y /= 8000.0f / gScale;


        InstanceTransforms[id].mtx._42 = pos.y;


        float3 slopeMaskSample = MaskMap.SampleLevel(sampleTypeWrap, tex, 0).rgb;

        float deltaPos = distance(pos.xz, prevPos.xz);

        InstanceTransforms[id].lifeTime *= ceil(_InstanceReveal);

        bool isSteep = id > 10000;

        float distanceToEye       = distance(_EyePosition, pos);
        float linearDistanceRatio = saturate((distanceToEye + 30.0f) / (scale * 0.5f));
        uint  maxStep             = 6;
        uint  step                = lerp(0, maxStep - 1, linearDistanceRatio);
        bool  shouldAppend        = (id % maxStep) >= step || isSteep;

        if (shouldAppend == false)
        {
                float seed                      = id * 3.64567f + _Time;
                InstanceTransforms[id].lifeTime = RandomFloatInRange(seed, -5.0f, 0.0f);
                return;
        }

        float3 vMax = pos + float3(10.0f, 10.0f, 10.0f);
        float3 vMin = pos - float3(10.0f, 10.0f, 10.0f);

        float3 boxCenter  = 0.5f * (vMin + vMax);
        float3 boxExtents = 0.5f * (vMax - vMin);
        bool   shouldCull = AABBToFrustum(boxCenter, boxExtents);
        if (deltaPos <= scale)
        {
                InstanceTransforms[id].lifeTime = min(InstanceTransforms[id].lifeTime + _DeltaTime, 1.0f);

                if (InstanceTransforms[id].flags & IS_FLAT)
                {
                        if (!shouldCull)
                                InstanceIndicesFlat.Append(id);
                }
                else if (InstanceTransforms[id].flags & IS_STEEP && isSteep)
                {
                        if (!shouldCull)
                                InstanceIndicesSteep.Append(id);
                }
                return;
        }

        InstanceTransforms[id].mtx._41 = pos.x;
        InstanceTransforms[id].mtx._43 = pos.z;

        float seed                      = id * 3.64567f + _Time;
        InstanceTransforms[id].lifeTime = RandomFloatInRange(seed, -5.0f, 0.0f);


        if (slopeMaskSample.r > 0.5f)
        {
                InstanceTransforms[id].flags |= IS_FLAT;
                if (!shouldCull)
                        InstanceIndicesFlat.Append(id);
        }
        else if (slopeMaskSample.g > 0.5f && isSteep)
        {
                InstanceTransforms[id].flags &= ~IS_FLAT;
                InstanceTransforms[id].flags |= IS_STEEP;
                if (!shouldCull)
                        InstanceIndicesSteep.Append(id);
        }
        else
        {
                InstanceTransforms[id].flags &= ~IS_FLAT;
                InstanceTransforms[id].flags &= ~IS_STEEP;
        }
}
