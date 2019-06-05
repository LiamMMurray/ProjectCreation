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

        void AddSprite(ID3D11Device*        device,
                       ID3D11DeviceContext* deviceContext,
                       const wchar_t*       FileName,
                       float                PositionX,
                       float                PositionY);
        void RemoveSprite(int id);

        void AddText(ID3D11Device*        device,
                     ID3D11DeviceContext* deviceContext,
                     const wchar_t*       FileName,
                     std::string          TextDisplay,
                     float                PositionX,
                     float                PositionY);
        void RemoveText(int id);


        void UIClipCursor();

        static UIManager* instance;

    private:
        bool                                   m_InMenu = false;
        std::unique_ptr<DirectX::SpriteBatch>  mSpriteBatch;
        std::unique_ptr<DirectX::CommonStates> mStates;
        POINT                                  mCursor;

        std::vector<SpriteComponent> mSprites;
        std::vector<FontComponent*>  mSpriteFonts;
};
