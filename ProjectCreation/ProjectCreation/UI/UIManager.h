#pragma once

#include <wrl/client.h>
#include <memory>

#include <DDSTextureLoader.h>
#include "WICTextureLoader.h"

#include <string>
#include <vector>
#include "CommonStates.h"
#include "FontComponent.h"
#include "SpriteComponent.h"

class RenderSystem;

class UIManager
{
        using native_handle_type = void*;
        native_handle_type m_WindowHandle;

    public:
        static void Initialize(native_handle_type hwnd);
        static void Update();
        static void Shutdown();

        // Pause Menu
        void AddSprite(ID3D11Device*                 device,
                       ID3D11DeviceContext*          deviceContext,
                       std::vector<SpriteComponent>& SpriteVector,
                       const wchar_t*                FileName,
                       float                         PositionX,
                       float                         PositionY,
                       float                         scaleX,
                       float                         scaleY,
                       bool                          enabled);

        void AddText(ID3D11Device*                 device,
                     ID3D11DeviceContext*          deviceContext,
                     std::vector<FontComponent>&   FontVector,
                     std::vector<SpriteComponent>& SpriteVector,
                     const wchar_t*                FileName,
                     std::string                   TextDisplay,
                     float                         scale,
                     float                         PositionX,
                     float                         PositionY,
                     bool                          enabled,
                     bool                          AddButton                 = false,
                     bool                          bOverrideButtonDimensions = false,
                     float                         buttonwidth               = 0.5f,
                     float                         buttonheight              = 0.5f);

        void CreateBackground(ID3D11Device*        device,
                              ID3D11DeviceContext* deviceContext,
                              const wchar_t*       FileName,
                              float                ScreenWidth,
                              float                ScreenHeight);

        void MainTilteUnpause();
        void Pause();
        void Unpause();


        void        UIClipCursor();
        static void OnScreenResize();


        static UIManager* instance;

    private:
        bool                                             m_IsFullscreen = false;
        bool                                             m_InMenu       = false;
        std::unique_ptr<DirectX::SpriteBatch>            m_SpriteBatch;
        std::unique_ptr<DirectX::CommonStates>           m_States;
        RECT                                             m_fullscreenRect;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_background;
        POINT                                            m_Cursor;

        RenderSystem* m_RenderSystem;

        DirectX::XMFLOAT2 m_TexelSize;
        DirectX::XMFLOAT2 m_ScreenSize;
        DirectX::XMVECTOR m_ScreenCenter;

        std::vector<std::vector<SpriteComponent>*> m_AllSprites;
        std::vector<std::vector<FontComponent>*>   m_AllFonts;

        std::vector<SpriteComponent> m_MainSprites;
        std::vector<FontComponent>   m_MainSpriteFonts;

        std::vector<SpriteComponent> m_PauseSprites;
        std::vector<FontComponent>   m_PauseSpriteFonts;

        std::vector<SpriteComponent> m_OptionsSprites;
        std::vector<FontComponent>   m_OptionsSpriteFonts;

        std::vector<SpriteComponent> m_LevelSprites;
        std::vector<FontComponent>   m_LevelSpriteFonts;
};
