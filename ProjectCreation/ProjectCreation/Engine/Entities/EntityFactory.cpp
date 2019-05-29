#include "EntityFactory.h"
#include "../../Rendering/Components/SkeletalMeshComponent.h"
#include "../../Rendering/Components/StaticMeshComponent.h"
#include "../Animation/AnimationComponent.h"
#include "../GEngine.h"
#include "../GenericComponents/TransformComponent.h"
#include "BaseEntities.h"

#include "../ResourceManager/SkeletalMesh.h"

EntityHandle EntityFactory::CreateDummyTransformEntity(ComponentHandle* outTransformHandle)
{
        auto componentManager = GEngine::Get()->GetComponentManager();
        auto entityManager    = GEngine::Get()->GetEntityManager();
        auto resourceManager  = GEngine::Get()->GetResourceManager();

        auto outEntityHandle = entityManager->CreateEntity<BaseEntity>();

        auto tCompHandle = componentManager->AddComponent<TransformComponent>(outEntityHandle);

        if (outTransformHandle)
                *outTransformHandle = tCompHandle;

        return outEntityHandle;
}

EntityHandle EntityFactory::CreateStaticMeshEntity(const char*      staticMeshName,
                                                   const char*      materialName,
                                                   ComponentHandle* outTransformHandle,
                                                   ComponentHandle* outStaticMeshHandle)
{
        auto componentManager = GEngine::Get()->GetComponentManager();
        auto entityManager    = GEngine::Get()->GetEntityManager();
        auto resourceManager  = GEngine::Get()->GetResourceManager();

        auto outEntityHandle = entityManager->CreateEntity<BaseEntity>();

        auto tCompHandle = componentManager->AddComponent<TransformComponent>(outEntityHandle);
        auto tComp       = componentManager->GetComponent<TransformComponent>(tCompHandle);

        auto sMeshCompHandle = componentManager->AddComponent<StaticMeshComponent>(outEntityHandle);

        auto meshComp                = componentManager->GetComponent<StaticMeshComponent>(sMeshCompHandle);
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
        auto componentManager = GEngine::Get()->GetComponentManager();
        auto entityManager    = GEngine::Get()->GetEntityManager();
        auto resourceManager  = GEngine::Get()->GetResourceManager();

        int animCount = animNames.size();

        auto outEntityHandle = entityManager->CreateEntity<BaseEntity>();

        auto tCompHandle = componentManager->AddComponent<TransformComponent>(outEntityHandle);
        auto tComp       = componentManager->GetComponent<TransformComponent>(tCompHandle);

        auto sMeshCompHandle           = componentManager->AddComponent<SkeletalMeshComponent>(outEntityHandle);
        auto meshComp                  = componentManager->GetComponent<SkeletalMeshComponent>(sMeshCompHandle);
        meshComp->m_MaterialHandle     = resourceManager->LoadMaterial(materialName);
        meshComp->m_SkeletalMeshHandle = resourceManager->LoadSkeletalMesh(skeletalMeshName);
        meshComp->m_Skeleton = resourceManager->GetResource<SkeletalMesh>(meshComp->m_SkeletalMeshHandle)->m_BindPoseSkeleton;

        auto animCompHandle = componentManager->AddComponent<AnimationComponent>(outEntityHandle);
        auto animComp       = componentManager->GetComponent<AnimationComponent>(animCompHandle);

        std::vector<ResourceHandle> anims;
        anims.reserve(animCount);
        for (int i = 0; i < animCount; ++i)
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
