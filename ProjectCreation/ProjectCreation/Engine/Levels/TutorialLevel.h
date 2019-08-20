#pragma once
#include <DirectXMath.h>
#include "../../ECS/EntityHandle.h"
#include "../../ECS/HandleManager.h"
#include "../Controller/ControllerSystem.h"
#include "../Controller/PlayerMovement.h"
#include "../GEngine.h"
#include "../Gameplay/OrbRespawnComponent.h"
#include "../Gameplay/OrbitSystem.h"
#include "../Gameplay/SpeedBoostSystem.h"
#include "../Gameplay/SpeedboostComponent.h"
#include "../Gameplay/SplineElementComponent.h"
#include "../GenericComponents/TransformComponent.h"
#include "LevelState.h"

class TutorialLevel : public ILevelState
{
        SpeedBoostSystem*   m_SpeedBoostSystem;
        HandleManager*      m_HandleManager;
        TransformComponent* m_PlayerTransform;
        EntityHandle        m_PlayerEntityHandle;
        PlayerController*   m_PlayerController;
        OrbitSystem*        m_OrbitSystem;


        bool m_WhiteCollected = false;
        bool m_RedCollected   = false;
        bool m_BlueCollected  = false;
        bool m_GreenCollected = false;

		static TutorialLevel* Instance;

    public:
        enum E_TUTORIAL_PHASE
        {
                PHASE_1 = 0,
                PHASE_2,
                PHASE_3,
                PHASE_4,
                COUNT
        };

        E_TUTORIAL_PHASE phases[E_TUTORIAL_PHASE::COUNT];
        E_TUTORIAL_PHASE currPhase;
        int              currPhaseIndex;
        // Inherited via ILevelState
        void Enter() override;
        void Update(float deltaTime) override;
        void Exit() override;

        TutorialLevel();
        virtual ~TutorialLevel() = default;

        int  whiteCount, redCount, blueCount, greenCount, levelRequested;
        bool finished;

        inline void RequestNextPhase()
        {
                if (currPhaseIndex >= E_TUTORIAL_PHASE::COUNT)
                {
                        finished = true;
                }

                else
                {
                        currPhase = phases[currPhaseIndex];
                }
        }

        void                  SpawnFirstWhiteOrb();
        void                  SpawnFirstRedOrb();
        void                  SpawnFirstBlueOrb();
        void                  SpawnFirstGreenOrb();
        static TutorialLevel* Get();
};