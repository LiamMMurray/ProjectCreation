#include "AnimationSystem.h"
#include "AnimationComponent.h"

#include "../../Rendering/Components/SkeletalMeshComponent.h"

#include "../GEngine.h"

#include "../ResourceManager/AnimationClip.h"
#include "../ResourceManager/ResourceManager.h"
#include "../ResourceManager/SkeletalMesh.h"

#include "../MathLibrary/MathLibrary.h"

#include "../../Rendering/DebugRender/debug_renderer.h"

using namespace DirectX;


static FQuaternion BlendQuaternions(int quatCount, FQuaternion* quats, float* weights)
{
        FQuaternion output;

        for (int i = 0; i < quatCount; ++i)
        {
                output = output * FQuaternion::Lerp(FQuaternion(XMQuaternionIdentity()), quats[i], weights[i]);
        }

        return output;
}

static FQuaternion BlendVectors(int vecCount, XMVECTOR* vecs, float* weights)
{
        XMVECTOR output;

        for (int i = 0; i < vecCount; ++i)
        {
                output += XMVectorLerp(XMVectorZero(), vecs[i], weights[i]);
        }

        return output;
}

void AnimationSystem::calcTransforms(Animation::FJoint*          joints,
                                     int                         jointCount,
                                     double                      time,
                                     const Animation::FAnimClip& animClip,
                                     float                       accumWeight,
                                     float                       currweight)
{
        int   prevFrame  = 0;
        int   nextFrame  = 1;
        float ratio      = 0.0f;
        int   frameCount = (int)animClip.frames.size();
        float animTime   = MathLibrary::Warprange(float(time), 0.0f, (float)animClip.duration);

        while (nextFrame < frameCount - 1)
        {
                if (animTime >= animClip.frames[prevFrame].time && animTime < animClip.frames[nextFrame].time)
                {
                        break;
                }

                prevFrame++;
                nextFrame++;
        } 

        auto prevTime = animClip.frames[prevFrame].time;
        auto nextTime = animClip.frames[nextFrame].time;

        ratio = float(((double)animTime - prevTime) / (nextTime - prevTime));

        ratio = MathLibrary::clamp(ratio, 0.0f, 1.0f);

        for (int i = 0; i < jointCount; ++i)
        {
                auto& prevTransform = animClip.frames[prevFrame].joints[i];
                auto& nextTransform = animClip.frames[nextFrame].joints[i];

                // skeletalMesh.joints[i].transform = Transform::Lerp(skeletalMesh.joints[i].transform,
                // nextTransform, ratio);
                XMVECTOR outVec   = XMVectorLerp(prevTransform.translation, nextTransform.translation, ratio);
                XMVECTOR outQuat  = XMQuaternionSlerp(prevTransform.rotation.data, nextTransform.rotation.data, ratio);
                XMVECTOR outScale = XMVectorLerp(prevTransform.scale, nextTransform.scale, ratio);

                float c = currweight, p = accumWeight;
                MathLibrary::NormalizeValues(c, p);

                joints[i].transform.rotation.data = XMQuaternionSlerp(joints[i].transform.rotation.data, outQuat, c);
                joints[i].transform.translation       = XMVectorLerp(joints[i].transform.translation, outVec, c);
                joints[i].transform.scale             = XMVectorLerp(joints[i].transform.scale, outScale, c);
        }
}

void AnimationSystem::OnPreUpdate(float deltaTime)
{}

void AnimationSystem::OnUpdate(float deltaTime)
{
        // ITR_TODO
        if (m_ComponentManager->ComponentsExist<AnimationComponent>())
                return;

        auto animCompItr = m_ComponentManager->GetActiveComponents<AnimationComponent>();

        for (auto itr = animCompItr.begin(); itr != animCompItr.end(); itr++)
        {
                auto animComp = static_cast<AnimationComponent*>(itr.data());

                EntityHandle           ownerHandle = animComp->GetOwner();
                SkeletalMeshComponent* skelComp    = m_ComponentManager->GetComponent<SkeletalMeshComponent>(ownerHandle);
                Animation::FSkeleton&  skel        = skelComp->m_Skeleton;

                animComp->m_Time += deltaTime * 1.0f;


                float normTime = sin(animComp->m_Time / 5.f) * 0.5f + 0.5f;

                float w[3] = {};

                if (normTime < 0.5f)
                {
                        w[1] = normTime * 2.0f;
                        w[0] = 1.0f - w[1];
                }
                else
                {
                        w[2] = normTime * 2.0f - 1.0f;
                        w[1] = 1.0f - w[2];
                }

                animComp->SetWeights(3, w);

                int clipCount = (int)animComp->m_Clips.size();

                int jointCount = (int)skel.jointTransforms.size();

                std::vector<float> weights;
                weights         = animComp->m_Weights;
                float sumWeight = 0.0f;
                for (int currTrack = 0; currTrack < clipCount; ++currTrack)
                {
                        auto clip = m_ResourceManager->GetResource<AnimationClip>(animComp->m_Clips[currTrack]);
                        calcTransforms(skel.jointTransforms.data(),
                                       jointCount,
                                       animComp->m_Time,
                                       clip->m_AnimClip,
                                       sumWeight,
                                       animComp->m_Weights[currTrack]);

                        sumWeight += animComp->m_Weights[currTrack];
                }
        }
}

void AnimationSystem::OnPostUpdate(float deltaTime)
{}

void AnimationSystem::OnInitialize()
{
        m_ComponentManager = GEngine::Get()->GetComponentManager();
        m_EntityManager    = GEngine::Get()->GetEntityManager();
        m_ResourceManager  = GEngine::Get()->GetResourceManager();
}

void AnimationSystem::OnShutdown()
{}

void AnimationSystem::OnResume()
{}

void AnimationSystem::OnSuspend()
{}

void AnimationSystem::AddAnimClipsToComponent(ComponentHandle targetComp,
                                              int             animCount,
                                              ResourceHandle* animClipHandles,
                                              float*          weights)
{
        AnimationComponent* animComp = m_ComponentManager->GetComponent<AnimationComponent>(targetComp);
        animComp->m_Clips.insert(animComp->m_Clips.begin(), animClipHandles, animClipHandles + animCount);
        animComp->m_Weights.insert(animComp->m_Weights.begin(), weights, weights + animCount);

        MathLibrary::NormalizeArray(animComp->m_Weights.size(), animComp->m_Weights.data());
}
