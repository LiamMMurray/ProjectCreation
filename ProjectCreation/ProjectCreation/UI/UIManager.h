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
                     std::vector<FontComponent*>&  FontVector,
                     std::vector<SpriteComponent>& SpriteVector,
                     const wchar_t*                FileName,
                     std::string                   TextDisplay,
                     float                         PositionX,
                     float                         PositionY,
                     bool                          AddButton,
                     bool                          enabled);

        void CreateBackground(ID3D11Device*        device,
                              ID3D11DeviceContext* deviceContext,
                              const wchar_t*       FileName,
                              float                ScreenWidth,
                              float                ScreenHeight);

        void MainTilteUnpause();
        void Pause();
        void Unpause();


        void UIClipCursor();

        static UIManager* instance;

    private:
        bool                                             m_EscPush = false;
        bool                                             m_IsFullscreen = false;
        bool                                             m_InMenu = false;
        std::unique_ptr<DirectX::SpriteBatch>            m_SpriteBatch;
        std::unique_ptr<DirectX::CommonStates>           m_States;
        RECT                                             m_fullscreenRect;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_background;
        POINT                                            m_Cursor;

        std::vector<SpriteComponent> m_MainSprites;
        std::vector<FontComponent*>  m_MainSpriteFonts;

        std::vector<SpriteComponent> m_PauseSprites;
        std::vector<FontComponent*>  m_PauseSpriteFonts;

        std::vector<SpriteComponent> m_OptionsSprites;
        std::vector<FontComponent*>  m_OptionsSpriteFonts;

        std::vector<SpriteComponent> m_LevelSprites;
        std::vector<FontComponent*>  m_LevelSpriteFonts;
};
