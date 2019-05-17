#include "AnimationSystem.h"
#include "AnimationComponent.h"

#include "../../Rendering/Components/SkeletalMeshComponent.h"

#include "../GEngine.h"

#include "../../ResourceManager/AnimationClip.h"
#include "../../ResourceManager/ResourceManager.h"
#include "../../ResourceManager/SkeletalMesh.h"

#include "../../MathLibrary/MathLibrary.h"

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
                                     float                       weight)
{
        int   prevFrame  = 0;
        int   nextFrame  = 0;
        float ratio      = 0.0f;
        int   frameCount = (int)animClip.frames.size();
        float animTime   = MathLibrary::Warprange(float(time), 0.0f, (float)animClip.duration);
        for (prevFrame = 0; prevFrame < frameCount - 1; prevFrame++)
        {
                if (animTime >= animClip.frames[prevFrame].time && animTime <= animClip.frames[prevFrame + 1].time)
                {
                        break;
                }

                nextFrame = prevFrame;
        }

        nextFrame     = prevFrame + 1;
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
                XMVECTOR outVec  = XMVectorLerp(prevTransform.translation, nextTransform.translation, ratio);
                outVec           = XMVectorLerp(XMVectorZero(), outVec, weight);
                XMVECTOR outQuat = XMQuaternionSlerp(prevTransform.rotation.rotation, nextTransform.rotation.rotation, ratio);
                outQuat          = XMQuaternionSlerp(XMQuaternionIdentity(), outQuat, weight);

                joints[i].transform.translation += outVec;
                joints[i].transform.rotation = joints[i].transform.rotation * outQuat;
        }
}

void AnimationSystem::OnPreUpdate(float deltaTime)
{}

void AnimationSystem::OnUpdate(float deltaTime)
{
        //auto it = m_ComponentManager->GetActiveComponents<AnimationComponent>();
        //while (it != m_ComponentManager->end<AnimationComponent>())
        //{
        //        EntityHandle           ownerHandle = it->GetOwner();
        //        IEntity*               owner       = m_EntityManager->GetEntity(ownerHandle);
        //        SkeletalMeshComponent* skelComp    = owner->GetComponent<SkeletalMeshComponent>();
        //        Animation::FSkeleton&  skel        = skelComp->m_Skeleton;

        //        it->m_Time += deltaTime;

        //        int clipCount = (int)it->m_Clips.size();

        //        for (int currTrack = 0; currTrack < clipCount; ++currTrack)
        //        {
        //                auto clip       = m_ResourceManager->GetResource<AnimationClip>(it->m_Clips[currTrack]);
        //                int  jointCount = (int)clip->m_AnimClip.frames[0].joints.size();
        //                calcTransforms(
        //                    skel.jointTransforms.data(), jointCount, it->m_Time, clip->m_AnimClip, it->m_Weights[currTrack]);
        //        }

        //        it++;
        //}
}

void AnimationSystem::OnPostUpdate(float deltaTime)
{}

void AnimationSystem::OnInitialize()
{
        m_ComponentManager = GEngine::Get()->GetComponentManager();
        m_EntityManager    = GEngine::Get()->GetEntityManager();
}

void AnimationSystem::OnShutdown()
{}

void AnimationSystem::OnResume()
{}

void AnimationSystem::OnSuspend()
{}
