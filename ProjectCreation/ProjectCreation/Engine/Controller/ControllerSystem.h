#pragma once

#include <DirectXMath.h>
#include "../../ECS/HandleManager.h"
#include "../../Rendering/Components/CameraComponent.h"
#include "../Controller/IController.h"
#include "../GEngine.h"
#include "../Gameplay/LightOrbColors.h"
#include "GamePad.h"

class ControllerSystem : public ISystem
{

        static ControllerSystem* instance;


        void init();
        void shutdown();

        void update(float delta);

        DirectX::XMVECTOR desiredColor      = DirectX::XMVECTORF32{1.0f, 1.0f, 1.0f, 1.0f};
        DirectX::XMVECTOR currentColor      = DirectX::XMVECTORF32{1.0f, 1.0f, 1.0f, 1.0f};
        float             desiredColorAlpha = 0.0f;
        float             currentColorAlpha = 0.0f;

        int CollectOrbEventIDs[3] = {-1, -1, -1};

    public:
        // bool m_toggleDebugCamera = false;

        inline int GetCollectOrbEventID(int color) const
        {
                return CollectOrbEventIDs[color];
        }

        inline void resetCollectedOrbEventID(int color)
        {
                CollectOrbEventIDs[color] = -1;
        }

        DirectX::XMFLOAT3 GetCurrentColorSelection() const;
        float             GetCurrentColorAlpha() const;

        // Player Controller Entity
        enum E_CONTROLLERS
        {
                PLAYER,
                DEBUG,
                COUNT
        };

        inline IController* GetCurrentController()
        {
                return m_Controllers[m_CurrentController];
        }

        inline int GetCurrentControllerIndex()
        {
                return (int)m_CurrentController;
        }

        IController* m_Controllers[E_CONTROLLERS::COUNT] = {};

        SystemManager* m_SystemManager;
        HandleManager* m_HandleManager;

        E_CONTROLLERS m_CurrentController;

        void ResetLightOrbCounters();


        int m_OrbCounts[E_LIGHT_ORBS::COUNT] = {};
        int m_PrevOrbColor;

        void DisplayConsoleMenu();

        // Returns the number of orbs collected of a certain color based on passed in parameter
        // 0 = Red Lights, 1 = Green Lights, 2 = Blue Lights
        int GetOrbCount(int color);

        // Adds one orb to a certain color based on passed in parameter
        // 0 = Red Lights, 1 = Green Lights, 2 = Blue Lights
        void IncreaseOrbCount(int color);

        // Resets the orb count of a certain color based on passed in parameter
        // 0 = Red Lights, 1 = Green Lights, 2 = Blue Lights
        void ResetOrbCount(int color);



        // Inherited via ISystem
        virtual void OnPreUpdate(float deltaTime) override;
        virtual void OnUpdate(float deltaTime) override;
        virtual void OnPostUpdate(float deltaTime) override;
        virtual void OnInitialize() override;
        virtual void OnShutdown() override;
        virtual void OnResume() override;
        virtual void OnSuspend() override;
};
