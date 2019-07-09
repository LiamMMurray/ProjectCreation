#include "UIManager.h"
#include <iostream>
#include "../Engine/CoreInput/CoreInput.h"
#include "../Engine/GEngine.h"
#include "../Rendering/RenderingSystem.h"

#include "../Engine/Controller/ControllerSystem.h"

#define WIN32_LEAN_AND_MEAN // Gets rid of bloat on Windows.h
#define NOMINMAX
#include <Windows.h>

#include "../Utility/Macros/DirectXMacros.h"
#include "../Utility/MemoryLeakDetection.h"
UIManager* UIManager::instance;

using namespace DirectX;
// Adding UI
void UIManager::AddSprite(ID3D11Device*        device,
                          ID3D11DeviceContext* deviceContext,
                          int                  category,
                          const wchar_t*       FileName,
                          float                PositionX,
                          float                PositionY,
                          float                scaleX,
                          float                scaleY,
                          bool                 enabled)
{

        SpriteComponent cSprite;

        Microsoft::WRL::ComPtr<ID3D11Resource> resource;
        HRESULT hr = DirectX::CreateDDSTextureFromFile(device, FileName, resource.GetAddressOf(), &cSprite.mTexture);
        if (FAILED(hr))
        {
                exit(-1);
        }

        Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture;
        resource.As(&Texture);

        CD3D11_TEXTURE2D_DESC TextureDesc;
        Texture->GetDesc(&TextureDesc);


        // Add the width and height to the sprite
        cSprite.mWidth  = TextureDesc.Width;
        cSprite.mHeight = TextureDesc.Height;

        // Scale
        cSprite.mScaleX = scaleX * 1.0f / cSprite.mWidth;
        cSprite.mScaleY = scaleY * 1.0f / cSprite.mHeight;

        // Add the origin to the sprite
        cSprite.mOrigin.x = (float)(cSprite.mWidth * 0.5f);
        cSprite.mOrigin.y = (float)(cSprite.mHeight * 0.5f);

        // Sprite Screen Position
        cSprite.SetPosition(PositionX, PositionY);

        // Set the Sprite to enabled
        cSprite.mEnabled = enabled;

        cSprite.mRectangle.center  = XMVectorSet(PositionX, PositionY, 0.0f, 1.0f);
        cSprite.mRectangle.extents = XMVectorSet(scaleX * 0.5f, scaleY * 0.5f, 0.0f, 1.0f);
        // Push back to the vector
        m_AllSprites[category].emplace_back(cSprite);

        // Reset everything that needs to be
        Texture.Reset();
        resource.Reset();
}

// Adds a sprite to the vector of text
// Currently PositionX and PositionY are not used
void UIManager::AddText(ID3D11Device*        device,
                        ID3D11DeviceContext* deviceContext,
                        int                  category,
                        const wchar_t*       FileName,
                        std::string          TextDisplay,
                        float                scale,
                        float                PositionX,
                        float                PositionY,
                        bool                 enabled,
                        bool                 AddButton,
                        bool                 bOverrideButtonDimensions,
                        float                buttonwidth,
                        float                buttonheight)
{
        FontComponent cFont;

        cFont.mSpriteFont  = std::make_unique<DirectX::SpriteFont>(device, FileName);
        cFont.mTextDisplay = TextDisplay;

        // Set the Main Menu text to enabled
        cFont.mEnabled = enabled;

        // Create Dimensions
        XMFLOAT2 dimensions;
        XMStoreFloat2(&dimensions, cFont.mSpriteFont->MeasureString(TextDisplay.c_str()));
        float aspectRatio = dimensions.x / dimensions.y;

        cFont.mOrigin = XMVectorSet(dimensions.x * 0.5f, dimensions.y * 0.5f, 0.0f, 1.0f);
        cFont.mScaleX = aspectRatio * scale * 1.0f / (dimensions.x);
        cFont.mScaleY = scale * 1.0f / (dimensions.y);
        // Text Screen Position
        cFont.mScreenOffset.x = PositionX;
        cFont.mScreenOffset.y = PositionY;

        // This was not the error, fixed by Jose
        {
                // Error C2280
                // attempting to reference a deleted function
                // C:\Program Files(x86)\Microsoft Visual Studio\2019\Preview\VC\Tools\MSVC\14.20.27508\include\xmemory0 819
                // FIXED
                // Making the vector array an array of pointers fixed this issue
        }
        m_AllFonts[category].emplace_back(std::move(cFont));

        float bWidth  = scale * aspectRatio;
        float bHeight = scale;

        if (bOverrideButtonDimensions)
        {
                bWidth  = buttonwidth;
                bHeight = buttonheight;
        }

        if (AddButton)
        {
                instance->AddSprite(device,
                                    deviceContext,
                                    category,
                                    L"../Assets/2d/Sprite/Grey Box Test.dds",
                                    PositionX,
                                    PositionY,
                                    bWidth,
                                    bHeight,
                                    enabled);
        }
}

void UIManager::CreateBackground(ID3D11Device*        device,
                                 ID3D11DeviceContext* deviceContext,
                                 const wchar_t*       FileName,
                                 float                ScreenWidth,
                                 float                ScreenHeight)
{
        HRESULT hr =
            DirectX::CreateDDSTextureFromFile(device, FileName, nullptr, instance->m_background.ReleaseAndGetAddressOf());

        m_fullscreenRect.left   = 0;
        m_fullscreenRect.top    = 0;
        m_fullscreenRect.right  = (long)ScreenWidth;
        m_fullscreenRect.bottom = (long)ScreenHeight;
}


// Helpful Functions
void UIManager::UIClipCursor()
{
        RECT rect;
        GetClientRect((HWND)m_WindowHandle, &rect);

        POINT ul;
        ul.x = rect.left;
        ul.y = rect.top;

        POINT lr;
        lr.x = rect.right;
        lr.y = rect.bottom;

        MapWindowPoints((HWND)m_WindowHandle, nullptr, &ul, 1);
        MapWindowPoints((HWND)m_WindowHandle, nullptr, &lr, 1);

        rect.left = ul.x;
        rect.top  = ul.y;

        rect.right  = lr.x;
        rect.bottom = lr.y;

        ClipCursor(&rect);
}

void UIManager::OnScreenResize()
{}


// Pausing and Unpausing
void UIManager::MainTilteUnpause()
{
        GEngine::Get()->SetGamePaused(false);
        for (int i = 0; i < instance->m_AllFonts[E_MENU_CATEGORIES::MainMenu].size(); i++)
        {
                instance->m_AllFonts[E_MENU_CATEGORIES::MainMenu][i].mEnabled = false;
        }
}

void UIManager::Pause()
{
        instance->m_InMenu = true;
        GEngine::Get()->SetGamePaused(true);
        if (instance->m_InMenu)
        {
                while (ShowCursor(TRUE) < 0);
        }
        else
        {
                while (ShowCursor(FALSE) >= 0)
                        ;
        }

        // Pause Menu Sprites
        for (int i = 0; i < instance->m_AllSprites[E_MENU_CATEGORIES::PauseMenu].size(); i++)
        {
                instance->m_AllSprites[E_MENU_CATEGORIES::PauseMenu][i].mEnabled = true;
        }
        // Text
        for (int i = 0; i < instance->m_AllFonts[E_MENU_CATEGORIES::PauseMenu].size(); i++)
        {
                instance->m_AllFonts[E_MENU_CATEGORIES::PauseMenu][i].mEnabled = true;
        }

        // Options Menu Sprites
        for (int i = 0; i < instance->m_AllSprites[E_MENU_CATEGORIES::OptionsMenu].size(); i++)
        {
                instance->m_AllSprites[E_MENU_CATEGORIES::OptionsMenu][i].mEnabled = false;
        }
        // Text
        for (int i = 0; i < instance->m_AllFonts[E_MENU_CATEGORIES::OptionsMenu].size(); i++)
        {
                instance->m_AllFonts[E_MENU_CATEGORIES::OptionsMenu][i].mEnabled = false;
        }

        // Level Select Menu Sprites
        for (int i = 0; i < instance->m_AllSprites[E_MENU_CATEGORIES::LevelMenu].size(); i++)
        {
                instance->m_AllSprites[E_MENU_CATEGORIES::LevelMenu][i].mEnabled = false;
        }
        // Text
        for (int i = 0; i < instance->m_AllFonts[E_MENU_CATEGORIES::LevelMenu].size(); i++)
        {
                instance->m_AllFonts[E_MENU_CATEGORIES::LevelMenu][i].mEnabled = false;
        }
}

void UIManager::Unpause()
{
        instance->m_InMenu = false;
        GEngine::Get()->SetGamePaused(false);
        if (instance->m_InMenu)
        {
                while (ShowCursor(TRUE) < 0)
                        ;
        }
        else
        {
                while (ShowCursor(FALSE) >= 0)
                        ;
        }

        // Pause Menu Sprites
        for (int i = 0; i < instance->m_AllSprites[E_MENU_CATEGORIES::PauseMenu].size(); i++)
        {
                instance->m_AllSprites[E_MENU_CATEGORIES::PauseMenu][i].mEnabled = false;
        }
        // Text
        for (int i = 0; i < instance->m_AllFonts[E_MENU_CATEGORIES::PauseMenu].size(); i++)
        {
                instance->m_AllFonts[E_MENU_CATEGORIES::PauseMenu][i].mEnabled = false;
        }

        // Options Menu Sprites
        for (int i = 0; i < instance->m_AllSprites[E_MENU_CATEGORIES::OptionsMenu].size(); i++)
        {
                instance->m_AllSprites[E_MENU_CATEGORIES::OptionsMenu][i].mEnabled = false;
        }
        // Text
        for (int i = 0; i < instance->m_AllFonts[E_MENU_CATEGORIES::OptionsMenu].size(); i++)
        {
                instance->m_AllFonts[E_MENU_CATEGORIES::OptionsMenu][i].mEnabled = false;
        }

        // Level Select Menu Sprites
        for (int i = 0; i < instance->m_AllSprites[E_MENU_CATEGORIES::LevelMenu].size(); i++)
        {
                instance->m_AllSprites[E_MENU_CATEGORIES::LevelMenu][i].mEnabled = false;
        }
        // Text
        for (int i = 0; i < instance->m_AllFonts[E_MENU_CATEGORIES::LevelMenu].size(); i++)
        {
                instance->m_AllFonts[E_MENU_CATEGORIES::LevelMenu][i].mEnabled = false;
        }
}


// Core Function
void UIManager::Initialize(native_handle_type hwnd)
{
        assert(!instance);
        instance = DBG_NEW UIManager;

        instance->m_RenderSystem = GEngine::Get()->GetSystemManager()->GetSystem<RenderSystem>();

        instance->m_SpriteBatch = std::make_unique<DirectX::SpriteBatch>(instance->m_RenderSystem->m_Context);
        instance->m_States      = std::make_unique<DirectX::CommonStates>(instance->m_RenderSystem->m_Device);

        // instance->m_AllSprites.push_back(&instance->m_MainSprites);
        // instance->m_AllFonts.push_back(&instance->m_MainSpriteFonts);
        //
        // instance->m_AllSprites.push_back(&instance->m_PauseSprites);
        // instance->m_AllFonts.push_back(&instance->m_PauseSpriteFonts);
        //
        // instance->m_AllSprites.push_back(&instance->m_OptionsSprites);
        // instance->m_AllFonts.push_back(&instance->m_OptionsSpriteFonts);
        //
        // instance->m_AllSprites.push_back(&instance->m_LevelSprites);
        // instance->m_AllFonts.push_back(&instance->m_LevelSpriteFonts);
        for (int i = 0; i < E_MENU_CATEGORIES::COUNT; ++i)
        {
                instance->m_AllSprites.insert(std::make_pair(i, std::vector<SpriteComponent>()));
        }

        for (int i = 0; i < E_MENU_CATEGORIES::COUNT; ++i)
        {
                instance->m_AllFonts.insert(std::make_pair(i, std::vector<FontComponent>()));
        }

        if (instance->m_InMenu)
        {
                while (ShowCursor(TRUE) >= 0)
                        ;
                ClipCursor(nullptr);
        }
        else
        {
                instance->UIClipCursor();
                while (ShowCursor(FALSE) >= 0)
                        ;
        }

        instance->m_WindowHandle = hwnd;

        // Main Menu
        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          E_MENU_CATEGORIES::MainMenu,
                          L"../Assets/2d/Text/angel.spritefont",
                          "INANIS",
                          0.06f,
                          0.0f,
                          -0.35f,
                          true,
                          false);

		// "Press Spacebar, Q, and E to continue. . . "
        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          E_MENU_CATEGORIES::MainMenu,
                          L"../Assets/2d/Text/calibri.spritefont",
                          "Press A, S, and D to continue. . .",
                          0.06f,
                          0.0f,
                          0.1f,
                          true,
                          false);
        
        // (Spacebar, Q, and E)"
        //instance->AddText(instance->m_RenderSystem->m_Device,
        //                  instance->m_RenderSystem->m_Context,
        //                  E_MENU_CATEGORIES::MainMenu,
        //                  L"../Assets/2d/Text/calibri.spritefont",
        //                  "(Spacebar, Q, and E)",
        //                  0.06f,
        //                  0.0f,
        //                  0.2f,
        //                  true,
        //                  false);

        // Pause Menu
        constexpr float pauseButtonWidth  = 0.25f;
        constexpr float pauseButtonHeight = 0.05f;
        instance->AddSprite(instance->m_RenderSystem->m_Device,
                            instance->m_RenderSystem->m_Context,
                            E_MENU_CATEGORIES::PauseMenu,
                            L"../Assets/2d/Sprite/Grey Box Test.dds",
                            0.0f,
                            0.0f,
                            0.4f,
                            0.6f,
                            false);

        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          E_MENU_CATEGORIES::PauseMenu,
                          L"../Assets/2d/Text/angel.spritefont",
                          "INANIS",
                          0.06f,
                          0.0f,
                          -0.35f,
                          false,
                          false);

        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          E_MENU_CATEGORIES::PauseMenu,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Resume",
                          0.05f,
                          0.0f,
                          -0.15f,
                          false,
                          true,
                          true,
                          pauseButtonWidth,
                          pauseButtonHeight);

        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          E_MENU_CATEGORIES::PauseMenu,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Levels",
                          0.05f,
                          0.0f,
                          -0.05f,
                          false,
                          true,
                          true,
                          pauseButtonWidth,
                          pauseButtonHeight);

        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          E_MENU_CATEGORIES::PauseMenu,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Options",
                          0.05f,
                          0.0f,
                          0.05f,
                          false,
                          true,
                          true,
                          pauseButtonWidth,
                          pauseButtonHeight);

        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          E_MENU_CATEGORIES::PauseMenu,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Controls",
                          0.05f,
                          0.0f,
                          0.15f,
                          false,
                          true,
                          true,
                          pauseButtonWidth,
                          pauseButtonHeight);

        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          E_MENU_CATEGORIES::PauseMenu,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Exit",
                          0.05f,
                          0.0f,
                          0.25f,
                          false,
                          true,
                          true,
                          pauseButtonWidth,
                          pauseButtonHeight);

        // Options Menu
        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          E_MENU_CATEGORIES::OptionsMenu,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Back",
                          0.05f,
                          0.0f,
                          -0.15f,
                          false,
                          true,
                          true,
                          pauseButtonWidth,
                          pauseButtonHeight);

        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          E_MENU_CATEGORIES::OptionsMenu,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Window Mode",
                          0.04f,
                          0.0f,
                          0.0f,
                          false,
                          true,
                          true,
                          pauseButtonWidth,
                          pauseButtonHeight);

        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          E_MENU_CATEGORIES::OptionsMenu,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Resolution",
                          0.04f,
                          0.0f,
                          0.1f,
                          false,
                          true,
                          true,
                          pauseButtonWidth,
                          pauseButtonHeight);

        // Level Menu
        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          E_MENU_CATEGORIES::LevelMenu,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Back",
                          0.05f,
                          0.0f,
                          -0.15f,
                          false,
                          true,
                          true,
                          pauseButtonWidth,
                          pauseButtonHeight);

        // Resume Button
        instance->m_AllSprites[E_MENU_CATEGORIES::PauseMenu][1].OnMouseDown.AddEventListener(
            [](UIMouseEvent* e) { instance->Unpause(); });

        // Level Select Button
        instance->m_AllSprites[E_MENU_CATEGORIES::PauseMenu][2].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                // Sprites
                for (int i = 0; i < instance->m_AllSprites[E_MENU_CATEGORIES::PauseMenu].size(); i++)
                {
                        if (i != 0)
                        {
                                instance->m_AllSprites[E_MENU_CATEGORIES::PauseMenu][i].mEnabled = false;
                        }
                }
                // Text
                for (int i = 0; i < instance->m_AllFonts[E_MENU_CATEGORIES::PauseMenu].size(); i++)
                {
                        instance->m_AllFonts[E_MENU_CATEGORIES::PauseMenu][i].mEnabled = false;
                }

                // Sprites
                for (int i = 0; i < instance->m_AllSprites[E_MENU_CATEGORIES::LevelMenu].size(); i++)
                {
                        instance->m_AllSprites[E_MENU_CATEGORIES::LevelMenu][i].mEnabled = true;
                }
                // Text
                for (int i = 0; i < instance->m_AllFonts[E_MENU_CATEGORIES::LevelMenu].size(); i++)
                {
                        instance->m_AllFonts[E_MENU_CATEGORIES::LevelMenu][i].mEnabled = true;
                }
        });

        // Options Button
        instance->m_AllSprites[E_MENU_CATEGORIES::PauseMenu][3].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                // Sprites
                for (int i = 0; i < instance->m_AllSprites[E_MENU_CATEGORIES::PauseMenu].size(); i++)
                {
                        if (i != 0)
                        {
                                instance->m_AllSprites[E_MENU_CATEGORIES::PauseMenu][i].mEnabled = false;
                        }
                }
                // Text
                for (int i = 0; i < instance->m_AllFonts[E_MENU_CATEGORIES::PauseMenu].size(); i++)
                {
                        instance->m_AllFonts[E_MENU_CATEGORIES::PauseMenu][i].mEnabled = false;
                }

                // Sprites
                for (int i = 0; i < instance->m_AllSprites[E_MENU_CATEGORIES::OptionsMenu].size(); i++)
                {
                        instance->m_AllSprites[E_MENU_CATEGORIES::OptionsMenu][i].mEnabled = true;
                }
                // Text
                for (int i = 0; i < instance->m_AllFonts[E_MENU_CATEGORIES::OptionsMenu].size(); i++)
                {
                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsMenu][i].mEnabled = true;
                }
        });

        // Controls Button
        instance->m_AllSprites[E_MENU_CATEGORIES::PauseMenu][4].OnMouseDown.AddEventListener([](UIMouseEvent* e) {});

        // Exit Button
        instance->m_AllSprites[E_MENU_CATEGORIES::PauseMenu][5].OnMouseDown.AddEventListener(
            [](UIMouseEvent* e) { GEngine::Get()->RequestGameExit(); });

        // Options
        // Back Button
        instance->m_AllSprites[E_MENU_CATEGORIES::OptionsMenu][0].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                // Back button to go from the options menu to the pause menu

                // Disable all sprites for the options
                for (int i = 0; i < instance->m_AllSprites[E_MENU_CATEGORIES::OptionsMenu].size(); i++)
                {
                        instance->m_AllSprites[E_MENU_CATEGORIES::OptionsMenu][i].mEnabled = false;
                }
                // Disable all text for the options
                for (int i = 0; i < instance->m_AllFonts[E_MENU_CATEGORIES::OptionsMenu].size(); i++)
                {
                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsMenu][i].mEnabled = false;
                }

                // Enable all sprites for the background
                for (int i = 1; i < instance->m_AllSprites[E_MENU_CATEGORIES::PauseMenu].size(); i++)
                {
                        instance->m_AllSprites[E_MENU_CATEGORIES::PauseMenu][i].mEnabled = true;
                }
                // Enable all text for the background
                for (int i = 0; i < instance->m_AllFonts[E_MENU_CATEGORIES::PauseMenu].size(); i++)
                {
                        instance->m_AllFonts[E_MENU_CATEGORIES::PauseMenu][i].mEnabled = true;
                }
        });

        instance->m_AllSprites[E_MENU_CATEGORIES::OptionsMenu][1].OnMouseDown.AddEventListener([](UIMouseEvent* e){
                
        });

        // Level Select
        // Back Button
        instance->m_AllSprites[E_MENU_CATEGORIES::LevelMenu][0].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                // Back button to go from the options menu to the pause menu

                // Disable all sprites for the options
                for (int i = 0; i < instance->m_AllSprites[E_MENU_CATEGORIES::LevelMenu].size(); i++)
                {
                        instance->m_AllSprites[E_MENU_CATEGORIES::LevelMenu][i].mEnabled = false;
                }
                // Disable all text for the options
                for (int i = 0; i < instance->m_AllFonts[E_MENU_CATEGORIES::LevelMenu].size(); i++)
                {
                        instance->m_AllFonts[E_MENU_CATEGORIES::LevelMenu][i].mEnabled = false;
                }

                // Enable all sprites for the background
                for (int i = 1; i < instance->m_AllSprites[E_MENU_CATEGORIES::PauseMenu].size(); i++)
                {
                        instance->m_AllSprites[E_MENU_CATEGORIES::PauseMenu][i].mEnabled = true;
                }
                // Enable all text for the background
                for (int i = 0; i < instance->m_AllFonts[E_MENU_CATEGORIES::PauseMenu].size(); i++)
                {
                        instance->m_AllFonts[E_MENU_CATEGORIES::PauseMenu][i].mEnabled = true;
                }
        });
}

void UIManager::Update()
{
        using namespace DirectX;

        instance->m_ScreenSize =
            XMFLOAT2{instance->m_RenderSystem->m_BackBufferWidth, instance->m_RenderSystem->m_BackBufferHeight};
        instance->m_ScreenCenter = XMVectorSet(instance->m_ScreenSize.x * 0.5f, instance->m_ScreenSize.y * 0.5f, 0.0f, 1.0f);
        instance->m_TexelSize    = XMFLOAT2{1.0f / instance->m_ScreenSize.x, 1.0f / instance->m_ScreenSize.y};

        float aspectRatio = instance->m_ScreenSize.x / instance->m_ScreenSize.y;
        if (instance->m_InMenu)
        {
                ClipCursor(nullptr);
        }
        else
        {
                instance->UIClipCursor();
        }

        // Pause & Unpause
        if (instance->m_AllFonts[E_MENU_CATEGORIES::MainMenu][0].mEnabled)
        {

			// Joseph Updated the main menu ui to match to input keys
			// Changed 'Space', 'Q', and 'E' to 'A', 'S', and 'D'
                if (GCoreInput::GetKeyState(KeyCode::A) == KeyState::Down &&
                    GCoreInput::GetKeyState(KeyCode::S) == KeyState::Down &&
                    GCoreInput::GetKeyState(KeyCode::D) == KeyState::Down)
                {
                        instance->MainTilteUnpause();
                }
        }
        else
        {
                if (GCoreInput::GetKeyState(KeyCode::Esc) == KeyState::DownFirst)
                {
                        if (GEngine::Get()->GetGamePaused() == false)
                        {
                                instance->Pause();
                        }
                        else
                        {
                                instance->Unpause();
                        }
                }
        }

        for (auto& it : instance->m_AllSprites)
                for (auto& sprite : it.second)
                {
                        if (sprite.mEnabled)
                        {
                                if (GCoreInput::GetMouseState(MouseCode::LeftClick) == KeyState::Release)
                                {
                                        XMFLOAT2 cursorCoords = {(float)GCoreInput::GetMouseWindowPosX(),
                                                                 (float)GCoreInput::GetMouseWindowPosY()};
                                        XMVECTOR point        = UI::ConvertScreenPosToNDC(cursorCoords, instance->m_ScreenSize);
                                        if (UI::PointInRect(sprite.mRectangle, point))
                                        {
                                                // Button Was Pressed
                                                UIMouseEvent e;

                                                e.mouseX = (float)GCoreInput::GetMouseWindowPosX();
                                                e.mouseY = (float)GCoreInput::GetMouseWindowPosY();
                                                e.sprite = &sprite;
                                                sprite.OnMouseDown.Invoke(&e);
                                        }
                                }

                                instance->m_SpriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred,
                                                               instance->m_States->NonPremultiplied());

                                XMVECTOR position = XMVectorSet(sprite.mScreenOffset.x * instance->m_ScreenSize.x,
                                                                sprite.mScreenOffset.y * instance->m_ScreenSize.y,
                                                                0.0f,
                                                                1.0f) +
                                                    instance->m_ScreenCenter;

                                XMVECTOR scale = XMVectorSet(sprite.mScaleX * instance->m_ScreenSize.x,
                                                             aspectRatio * sprite.mScaleY * instance->m_ScreenSize.y,
                                                             0.0f,
                                                             1.0f);

                                instance->m_SpriteBatch->Draw(
                                    sprite.mTexture, position, nullptr, DirectX::Colors::White, 0.0f, sprite.mOrigin, scale);


                                instance->m_SpriteBatch->End();
                        }
                }

        for (auto& it : instance->m_AllFonts)
                for (auto& font : it.second)
                {
                        if (font.mEnabled)
                        {
                                instance->m_SpriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred,
                                                               instance->m_States->NonPremultiplied());

                                XMVECTOR position = XMVectorSet(font.mScreenOffset.x * instance->m_ScreenSize.x,
                                                                font.mScreenOffset.y * instance->m_ScreenSize.y,
                                                                0.0f,
                                                                0.0f) +
                                                    instance->m_ScreenCenter;

                                XMVECTOR scale = XMVectorSet(font.mScaleX * instance->m_ScreenSize.x,
                                                             aspectRatio * font.mScaleY * instance->m_ScreenSize.y,
                                                             0.0f,
                                                             0.0f);

                                font.mSpriteFont->DrawString(instance->m_SpriteBatch.get(),
                                                             font.mTextDisplay.c_str(),
                                                             position,
                                                             DirectX::Colors::White,
                                                             0.0f,
                                                             font.mOrigin,
                                                             scale);

                                instance->m_SpriteBatch->End();
                        }
                }
}

void UIManager::Shutdown()
{
        instance->m_States.reset();
        instance->m_SpriteBatch.reset();
        instance->m_background.Reset();

        for (auto& it : instance->m_AllSprites)
                for (auto& sprite : it.second)
                {
                        SAFE_RELEASE(sprite.mTexture);
                }

        for (auto& it : instance->m_AllFonts)
                for (auto& font : it.second)
                {
                        SAFE_RELEASE(font.mTexture);
                        font.mSpriteFont.reset();
                }

        assert(instance);
        delete instance;
}