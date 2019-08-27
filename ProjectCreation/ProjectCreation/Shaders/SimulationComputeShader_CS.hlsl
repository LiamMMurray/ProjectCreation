#include "MVPBuffer.hlsl"
#include "Math.hlsl"
#include "Samplers.hlsl"
#include "SceneBuffer.hlsl"
#include "Wrap.hlsl"

#include "ParticleData.hlsl"

cbuffer CScreenSpaceBuffer : register(b2)
{
        matrix _invProj;
        matrix _invView;
        float3 _playerPosition;
        float  _time;
};

float3 VSPositionFromDepth(float2 vTexCoord)
{
        // Get the depth value for this pixel
        float z = SceneDepth.SampleLevel(sampleTypeClamp, vTexCoord, 0).r;
        // Get x/w and y/w from the viewport position
        float  x             = vTexCoord.x * 2 - 1;
        float  y             = (1 - vTexCoord.y) * 2 - 1;
        float4 vProjectedPos = float4(x, y, z, 1.0f);
        // Transform by the inverse projection matrix
        float4 vPositionVS = mul(vProjectedPos, _invProj);
        // Divide by w to get the view-space position
        return vPositionVS.xyz / vPositionVS.w;
}


[numthreads(512, 1, 1)] void main(uint3 DTid
                                  : SV_DispatchThreadID) {
        float3 direction;
        int    id           = DTid.x;
        int    emitterIndex = id / gMaxParticlePerEmitter;

        ParticleBuffer[id].prevPos = ParticleBuffer[id].position;

        ParticleBuffer[id].velocity += ParticleBuffer[id].acceleration * _DeltaTime;

        float3 particleNextPos = ParticleBuffer[id].position.xyz + ParticleBuffer[id].velocity * _DeltaTime;
        float4 ndc             = mul(float4(particleNextPos, 1.0f), ViewProjection);
        ndc.xy /= ndc.w;
        ndc.xy = ndc.xy * 0.5f + 0.5f;
        ndc.y  = 1.0f - ndc.y;

        float2 invScreen = 1.0f / _ScreenDimensions;
        float3 posVS     = VSPositionFromDepth(ndc.xy);
        float3 posVS1    = VSPositionFromDepth(ndc.xy + float2(invScreen.x, 0.0f));
        float3 posVS2    = VSPositionFromDepth(ndc.xy + float2(0.0f, invScreen.y));
        float3 normalVS  = normalize(cross(posVS1 - posVS, posVS2 - posVS));
        float3 normalWS  = mul(float4(normalVS,1.0f), _invView).xyz;

        float depth = posVS.z;

        if (ParticleBuffer[id].time > 0.0f)
        {
                ParticleBuffer[DTid.x].time -= _DeltaTime;
                // ParticleBuffer[DTid.x].position += 1.0f*float4(ParticleBuffer[DTid.x].velocity * _DeltaTime,
                // 0.0f);

                float2 Min = float2(-0.5f * _Scale, -0.5f * _Scale);
                // Min        = float2(-10.0f, -10.0f);
                float2 Max = -Min;
                // ParticleBuffer[id].position.xyz =
                //  WrapPosition(ParticleBuffer[id].position.xyz, _EyePosition + Min, _EyePosition + Max);

                // bounce based on texture depth
                if ((ParticleBuffer[id].flags & NO_COLLISION))
                {
                        if (ndc.w > depth)
                        {
                                // ParticleBuffer[id].velocity     = reflect(ParticleBuffer[id].velocity, normalVS);
                                ParticleBuffer[id].velocity -= ParticleBuffer[id].acceleration * _DeltaTime;
                                ParticleBuffer[id].velocity -=
                                    dot(ParticleBuffer[id].velocity, normalWS) * normalWS * 1.5 /* 1.0 + bouciness */;
                        }
                }

                ParticleBuffer[id].position.xyz += ParticleBuffer[id].velocity * _DeltaTime;

                ParticleBuffer[id].position.xyz += _WorldOffsetDelta;
        }
}
