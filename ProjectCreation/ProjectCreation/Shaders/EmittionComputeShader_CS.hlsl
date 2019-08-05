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

[numthreads(512, 1, 1)] void main(uint3 DTid
                                  : SV_DispatchThreadID) {
        int id            = DTid.x;
        int emitterIndex  = id / gMaxParticlePerEmitter;
        int emitterOffset = id % gMaxParticlePerEmitter;
        int desired       = SegmentBuffer[emitterIndex].desiredCount;

        if (ParticleBuffer[id].time <= 0.0f)
        {
                if (emitterOffset < desired)
                {
                        float seed = _Time * 0.54843f + id * 0.547338f;

                        ParticleBuffer[id].scale = EmitterBuffer[emitterIndex].particleScale.x;

                        ParticleBuffer[id].textureIndex = EmitterBuffer[emitterIndex].textureIndex;
                        ParticleBuffer[id].textueRowCol = EmitterBuffer[emitterIndex].textueRowCol;

                        float timeMin = EmitterBuffer[emitterIndex].lifeSpan.x - EmitterBuffer[emitterIndex].lifeSpan.y;
                        float timeMax = EmitterBuffer[emitterIndex].lifeSpan.x + EmitterBuffer[emitterIndex].lifeSpan.y;
                        ParticleBuffer[id].time     = RandomFloatInRange(seed, timeMin, timeMax);
                        ParticleBuffer[id].lifeSpan = ParticleBuffer[id].time;
                        float3 startPos             = EmitterBuffer[emitterIndex].emitterPosition;

                        ParticleBuffer[id].velocity     = RandomFloat3InRange(seed,
                                                                          EmitterBuffer[emitterIndex].minInitialVelocity,
                                                                          EmitterBuffer[emitterIndex].maxInitialVelocity);
                        ParticleBuffer[id].acceleration = EmitterBuffer[emitterIndex].acceleration;


                        ParticleBuffer[id].uv    = EmitterBuffer[emitterIndex].uv;
                        ParticleBuffer[id].color = EmitterBuffer[emitterIndex].initialColor;

                        float3 offset = RandomFloat3InRange(
                            seed, EmitterBuffer[emitterIndex].minOffset, EmitterBuffer[emitterIndex].maxOffset);
                        ParticleBuffer[id].position = float4(startPos + offset, 1.0f);

                        ParticleBuffer[id].prevPos = ParticleBuffer[id].position;
                }
        }
        else
        {
                ParticleBuffer[id].time -= _DeltaTime;
        }
}
