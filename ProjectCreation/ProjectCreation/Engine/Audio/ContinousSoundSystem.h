#pragma once

#pragma once

#include <DirectXMath.h>
#include <string>
#include <vector>
#include "../../ECS/ECSTypes.h"
#include "../../ECS/ISystem.h"
#include "..//Particle Systems/Pools.h"
#include "3DSoundComponent.h"

class HandleManager;
class SpatialSoundSystem : public ISystem
{
    private:
        HandleManager* m_HandleManager;

        std::vector<Pools::dynamic_pool_t<SoundPoolElement>> m_SoundPools[E_SOUND_TYPE::TYPE_COUNT * 3];

    protected:
        // Inherited via ISystem
        virtual void OnPreUpdate(float deltaTime) override;
        virtual void OnUpdate(float deltaTime) override;
        virtual void OnPostUpdate(float deltaTime) override;
        virtual void OnInitialize() override;
        virtual void OnShutdown() override;
        virtual void OnResume() override;
        virtual void OnSuspend() override;

    public:
        EntityHandle PlaySoundAtLocation(const DirectX::XMVECTOR& pos, SoundComponent3D::FSettings& settings);
        void         FreeSound(int type, int variation, int16_t index);
};