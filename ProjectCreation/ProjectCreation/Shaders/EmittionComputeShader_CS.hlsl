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

                        float timeMin = EmitterBuffer[emitterIndex].lifeSpan.x - EmitterBuffer[emitterIndex].lifeSpan.y;
                        float timeMax = EmitterBuffer[emitterIndex].lifeSpan.x + EmitterBuffer[emitterIndex].lifeSpan.y;
                        ParticleBuffer[id].time       = RandomFloatInRange(seed, timeMin, timeMax);
                        ParticleBuffer[id].lifeSpan.x = ParticleBuffer[id].time;
                        ParticleBuffer[id].lifeSpan.y = EmitterBuffer[emitterIndex].lifeSpan.z;
                        ParticleBuffer[id].lifeSpan.z = EmitterBuffer[emitterIndex].lifeSpan.w;
                        float3 startPos               = EmitterBuffer[emitterIndex].emitterPosition;

                        ParticleBuffer[id].velocity     = RandomFloat3InRange(seed,
                                                                          EmitterBuffer[emitterIndex].minInitialVelocity,
                                                                          EmitterBuffer[emitterIndex].maxInitialVelocity);
                        ParticleBuffer[id].acceleration = EmitterBuffer[emitterIndex].acceleration;


                        float3 offset = RandomFloat3InRange(
                            seed, EmitterBuffer[emitterIndex].minOffset, EmitterBuffer[emitterIndex].maxOffset);
                        ParticleBuffer[id].position = float4(startPos + offset, 1.0f);

                        ParticleBuffer[id].prevPos = ParticleBuffer[id].position;


                        ParticleBuffer[id].scale.x = EmitterBuffer[emitterIndex].particleScale.x;
                        ParticleBuffer[id].scale.y = EmitterBuffer[emitterIndex].particleScale.y;

                        ParticleBuffer[id].initialColor = EmitterBuffer[emitterIndex].initialColor;
                        ParticleBuffer[id].finalColor   = EmitterBuffer[emitterIndex].finalColor;

                        ParticleBuffer[id].textureIndex = EmitterBuffer[emitterIndex].textureIndex;
                }
        }
        else
        {
                ParticleBuffer[id].time -= _DeltaTime;
        }
}
