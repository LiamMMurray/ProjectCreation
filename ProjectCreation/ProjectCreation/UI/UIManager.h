#pragma once

#include <wrl/client.h>
#include <memory>

#include <DDSTextureLoader.h>
#include "WICTextureLoader.h"

#include <string>
#include <unordered_map>
#include <vector>
#include "CommonStates.h"
#include "FontComponent.h"
#include "SpriteComponent.h"

class RenderSystem;

struct E_MENU_CATEGORIES
{
        enum
        {
                SplashScreen,
                MainMenu,
                PauseMenu,
                OptionsMenu,
                OptionsSubmenu,
                LevelMenu,
                ControlsMenu,
                Demo,
                COUNT
        };
};


struct E_FONT_TYPE
{
        enum
        {
                Angel,
                Calibri,
                CourierNew,
                COUNT
        };
};

struct PrevSettings
{
        bool m_IsFullscreen = false;
        int  m_Resolution   = 8; // 0-8 Representing the resDescriptors resolutions
        int  m_Volume       = 100;
        int  m_Sensitivity  = 1;
};
struct CurrSettings
{
        bool m_IsFullscreen = false;
        int  m_Resolution   = 8; // 0-8 Representing the resDescriptors resolutions
        int  m_Volume       = 100;
        int  m_Sensitivity  = 1;
};

class UIManager
{
        struct TimedFunction
        {
                void (*func)();
                float delay;
        };

        using native_handle_type = void*;
        native_handle_type m_WindowHandle;
        std::vector<TimedFunction> timed_functions;

    public:
        static void Initialize(native_handle_type hwnd);
        static void Update(float deltaTime);
        static void Shutdown();

        // Pause Menu
        int AddSprite(ID3D11Device*        device,
                      ID3D11DeviceContext* deviceContext,
                      int                  category,
                      const wchar_t*       FileName,
                      float                PositionX,
                      float                PositionY,
                      float                scaleX,
                      float                scaleY,
                      bool                 enabled);

        void AddText(ID3D11Device*        device,
                     ID3D11DeviceContext* deviceContext,
                     int                  category,
                     int                  fontType,
                     std::string          TextDisplay,
                     float                scaleX,
                     float                scaleY,
                     float                PositionX,
                     float                PositionY,
                     bool                 enabled,
                     bool                 AddButton                 = false,
                     bool                 bOverrideButtonDimensions = false,
                     float                buttonwidth               = 0.5f,
                     float                buttonheight              = 0.5f);

        void MainTitleUnpause();
        void Pause();
        void Unpause();
        void CheckResolution();
        void StartupResAdjust(HWND window);
        void AdjustResolution(HWND window, int wWidth, int wHeight);
        void SupportedResolutions(); // Creates the supported resolutions for the game
        void DemoEnd();

        void Splash_Start();
        void Splash_FullSail();
        void Splash_GPGames();
        void Splash_Team();
        void Splash_End();

        void WhiteOrbCollected();
        void RedOrbCollected();
        void GreenOrbCollected();
        void BlueOrbCollected();


        void UIClipCursor();


        static UIManager* instance;

        RenderSystem* m_RenderSystem;

        void DrawSprites(float deltaTime);
        void Present();

    private:
        void GameplayUpdate(float deltaTime);
        void SplashUpdate(float globalTimer, float deltaTime);

        bool         m_FirstFull      = true; // Turns false when the game is put to fullscreen on launch
        bool         m_InMenu         = false;
        bool         m_AdjustedScreen = false;
        bool         m_BreakSplash    = false;
        PrevSettings PSettings;
        CurrSettings CSettings;
        float        m_SliderHandle;

        DirectX::XMFLOAT2 m_TexelSize;
        DirectX::XMFLOAT2 m_ScreenSize;
        DirectX::XMVECTOR m_ScreenCenter;

        HWND                                                  m_window;
        std::unique_ptr<DirectX::SpriteBatch>                 m_SpriteBatch;
        std::unique_ptr<DirectX::CommonStates>                m_States;
        RECT                                                  m_fullscreenRect;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>      m_background;
        POINT                                                 m_Cursor;
        std::vector<DXGI_MODE_DESC>                           resDescriptors;
        std::unordered_map<int, std::vector<SpriteComponent>> m_AllSprites;
        std::unordered_map<int, std::vector<FontComponent>>   m_AllFonts;

        DirectX::SpriteFont* m_FontTypes[E_FONT_TYPE::COUNT];
};
