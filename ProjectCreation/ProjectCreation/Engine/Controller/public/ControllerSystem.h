#pragma once

#include <DirectXMath.h>
#include <HandleManager.h>
#include <CameraComponent.h>
#include <IController.h>
#include <GEngine.h>
#include <LightOrbColors.h>
#include <JGamePad.h>

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


    public:
        // bool m_toggleDebugCamera = false;
        void ResetPlayer();
		void CreatePlayer();

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

        int m_OrbCount = 0;
        int m_PrevOrbColor;

        bool IsVibrating;

        float rumbleStrengthL, rumbleStrengthR;

        void DisplayConsoleMenu();

        // Returns the number of orbs collected of a certain color based on passed in parameter
        // 0 = Red Lights, 1 = Green Lights, 2 = Blue Lights
        int GetOrbCount();

        // Adds one orb to a certain color based on passed in parameter
        // 0 = Red Lights, 1 = Green Lights, 2 = Blue Lights
        void IncreaseOrbCount(int color);

        int GetPrevOrbColor() const
        {
                return m_PrevOrbColor;
        }
        // Resets the orb count of a certain color based on passed in parameter
        // 0 = Red Lights, 1 = Green Lights, 2 = Blue Lights
        void ResetOrbCount();


        // Inherited via ISystem
        virtual void OnPreUpdate(float deltaTime) override;
        virtual void OnUpdate(float deltaTime) override;
        virtual void OnPostUpdate(float deltaTime) override;
        virtual void OnInitialize() override;
        virtual void OnShutdown() override;
        virtual void OnResume() override;
        virtual void OnSuspend() override;
};
