#pragma once
#include <DirectXMath.h>
#include <EntityHandle.h>
#include <HandleManager.h>
#include <ControllerSystem.h>
#include <PlayerMovement.h>
#include <GEngine.h>
#include <OrbRespawnComponent.h>
#include <OrbitSystem.h>
#include <SpeedBoostSystem.h>
#include <SpeedboostComponent.h>
#include <SplineElementComponent.h>
#include <TransformComponent.h>
#include <LevelState.h>

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
                PHASE_5,
                COUNT
        };

        E_TUTORIAL_PHASE        phases[E_TUTORIAL_PHASE::COUNT] = {PHASE_1, PHASE_2, PHASE_3, PHASE_4, PHASE_5};
        static E_TUTORIAL_PHASE currPhase;
        int              currPhaseIndex;
        // Inherited via ILevelState
        void Enter() override;
        void Update(float deltaTime) override;
        void Exit() override;

        TutorialLevel();
        virtual ~TutorialLevel() = default;

        int  whiteCount, redCount, blueCount, greenCount, levelRequested;
        bool finished;

        void RequestNextPhase();

		void UpdatePhase1(float deltaTime);
		void UpdatePhase2(float deltaTime);
		void UpdatePhase3(float deltaTime);
		void UpdatePhase4(float deltaTime);
		void UpdatePhase5(float deltaTime);

        void                  SpawnFirstWhiteOrb();
        void                  SpawnFirstRedOrb();
        void                  SpawnFirstBlueOrb();
        void                  SpawnFirstGreenOrb();
        static TutorialLevel* Get();
};