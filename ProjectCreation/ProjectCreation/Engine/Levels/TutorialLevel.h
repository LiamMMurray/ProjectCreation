#pragma once
#include <DirectXMath.h>
#include "../Controller/ControllerSystem.h"
#include "../GEngine.h"
#include "../Gameplay/OrbRespawnComponent.h"
#include "../Gameplay/OrbitSystem.h"
#include "../Gameplay/SpeedBoostSystem.h"
#include "../Gameplay/SpeedboostComponent.h"
#include "../Gameplay/SplineElementComponent.h"
#include "../GenericComponents/TransformComponent.h"
#include "../Controller/PlayerMovement.h"
#include "../../ECS/HandleManager.h"
#include "../../ECS/EntityHandle.h"
#include "LevelState.h"

class TutorialLevel : public ILevelState
{
        SpeedBoostSystem* m_SpeedBoostSystem;
        HandleManager*    m_HandleManager;
        TransformComponent* m_PlayerTransform;
        EntityHandle        m_PlayerEntityHandle;
        

        bool m_WhiteCollected = false;
        bool m_RedCollected   = false;
        bool m_BlueCollected  = false;
        bool m_GreenCollected = false;

    public:
        // Inherited via ILevelState
        void Enter() override;
        void Update(float deltaTime) override;
        void Exit() override;

        TutorialLevel();
        virtual ~TutorialLevel() = default;

		int whiteCount, redCount, blueCount, greenCount, levelRequested;

        void SpawnFirstWhiteOrb();
        void SpawnFirstRedOrb();
        void SpawnFirstBlueOrb();
        void SpawnFirstGreenOrb();
};