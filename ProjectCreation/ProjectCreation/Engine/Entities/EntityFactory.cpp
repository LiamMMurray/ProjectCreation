#include "EntityFactory.h"
#include "../../Rendering/Components/SkeletalMeshComponent.h"
#include "../../Rendering/Components/StaticMeshComponent.h"
#include "../Animation/AnimationComponent.h"
#include "../GEngine.h"
#include "../GenericComponents/TransformComponent.h"

#include "../ResourceManager/SkeletalMesh.h"

EntityHandle EntityFactory::CreateDummyTransformEntity(ComponentHandle* outTransformHandle)
{
        auto handleManager = GEngine::Get()->GetHandleManager();
        auto resourceManager  = GEngine::Get()->GetResourceManager();

        auto outEntityHandle = handleManager->CreateEntity();

        auto tCompHandle = outEntityHandle.AddComponent<TransformComponent>();

        if (outTransformHandle)
                *outTransformHandle = tCompHandle;

        return outEntityHandle;
}

EntityHandle EntityFactory::CreateStaticMeshEntity(const char*      staticMeshName,
                                                   const char*      materialName,
                                                   ComponentHandle* outTransformHandle,
                                                   ComponentHandle* outStaticMeshHandle)
{
        auto HandleManager = GEngine::Get()->GetHandleManager();
        auto resourceManager  = GEngine::Get()->GetResourceManager();

        auto outEntityHandle = HandleManager->CreateEntity();

        auto tCompHandle = outEntityHandle.AddComponent<TransformComponent>();
        auto tComp       = tCompHandle.Get<TransformComponent>();

        auto sMeshCompHandle = HandleManager->AddComponent<StaticMeshComponent>(outEntityHandle);
        NMemory::type_index index               = StaticMeshComponent::SGetTypeIndex();
        auto meshComp                = sMeshCompHandle.Get<StaticMeshComponent>();
        meshComp->m_MaterialHandle   = resourceManager->LoadMaterial(materialName);
        meshComp->m_StaticMeshHandle = resourceManager->LoadStaticMesh(staticMeshName);

        if (outTransformHandle)
                *outTransformHandle = tCompHandle;

        if (outStaticMeshHandle)
                *outStaticMeshHandle = sMeshCompHandle;

        return outEntityHandle;
}

EntityHandle EntityFactory::CreateSkeletalMeshEntity(const char*              skeletalMeshName,
                                                     const char*              materialName,
                                                     std::vector<std::string> animNames,
                                                     float*                   weights,
                                                     ComponentHandle*         outTransformHandle,
                                                     ComponentHandle*         outSkeletalMeshHandle,
                                                     ComponentHandle*         outAnimCompHandle)
{
        auto HandleManager = GEngine::Get()->GetHandleManager();
        auto resourceManager  = GEngine::Get()->GetResourceManager();

        size_t animCount = animNames.size();

        auto outEntityHandle = HandleManager->CreateEntity();

        auto tCompHandle = outEntityHandle.AddComponent<TransformComponent>();
        auto tComp       = tCompHandle.Get<TransformComponent>();

        auto sMeshCompHandle           = outEntityHandle.AddComponent<SkeletalMeshComponent>();
        auto meshComp                  = sMeshCompHandle.Get<SkeletalMeshComponent>();
        meshComp->m_MaterialHandle     = resourceManager->LoadMaterial(materialName);
        meshComp->m_SkeletalMeshHandle = resourceManager->LoadSkeletalMesh(skeletalMeshName);
        meshComp->m_Skeleton = resourceManager->GetResource<SkeletalMesh>(meshComp->m_SkeletalMeshHandle)->m_BindPoseSkeleton;

        auto animCompHandle = outEntityHandle.AddComponent<AnimationComponent>();
        auto animComp       = animCompHandle.Get<AnimationComponent>();

        std::vector<ResourceHandle> anims;
        anims.reserve(animCount);
        for (size_t i = 0; i < animCount; ++i)
        {
                anims.push_back(resourceManager->LoadAnimationClip(animNames[i].data(), &meshComp->m_Skeleton));
        }

        if (weights != nullptr)
        {
                animComp->AddAnims(animCount, weights, anims.data());
        }
        else
        {
                std::vector<float> defaultWeights(animCount, 0.0f);
                defaultWeights[0] = 1.0f;
                animComp->AddAnims(animCount, defaultWeights.data(), anims.data());
        }

        if (outTransformHandle)
                *outTransformHandle = tCompHandle;

        if (outSkeletalMeshHandle)
                *outSkeletalMeshHandle = sMeshCompHandle;

        if (outAnimCompHandle)
                *outAnimCompHandle = animCompHandle;

        return outEntityHandle;
}
