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

        instance->m_AllFonts[E_MENU_CATEGORIES::MainMenu][2].mEnabled = true;
}

void UIManager::Pause()
{
        instance->m_InMenu = true;
        GEngine::Get()->SetGamePaused(true);
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

        for (auto& it : instance->m_AllSprites)
        {
                for (auto& sprite : it.second)
                {
                        sprite.mEnabled = false;
                }
        }

        for (auto& it : instance->m_AllFonts)
        {
                for (auto& font : it.second)
                {
                        font.mEnabled = false;
                }
        }
}

void UIManager::CheckResolution()
{
        // Resizes the window
        RECT       desktop;
        const HWND hDesktop = GetDesktopWindow();
        GetWindowRect(hDesktop, &desktop); // set the size

        for (int i = 0; i < instance->resDescriptors.size(); i++)
        {
                if (instance->resDescriptors[i].Width == desktop.right && instance->resDescriptors[i].Height == desktop.bottom)
                {
                        instance->CSettings.m_Resolution = i;
                }
        }
}

void UIManager::StartupResAdjust(HWND window)
{
        if (instance->m_AdjustedScreen == false)
        {
                instance->m_AdjustedScreen = true;
                instance->m_window         = window;

                // Resizes the window
                RECT       desktop;
                const HWND hDesktop = GetDesktopWindow();
                GetWindowRect(hDesktop, &desktop); // set the size
                ::SetWindowPos(window, 0, 0, 0, desktop.right, desktop.bottom, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

                // Center the window
                MoveWindow(window, 0, 0, desktop.right, desktop.bottom, true);
        }
}

void UIManager::AdjustResolution(HWND window, int wWidth, int wHeight)
{
        ::SetWindowPos(window, 0, 0, 0, wWidth, wHeight, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

        // Center the window
        MoveWindow(window, 0, 0, wWidth, wHeight, true);
}

void UIManager::SupportedResolutions()
{
        IDXGIOutput* pOutput;
        UINT         num;
        instance->m_RenderSystem->m_Swapchain->GetContainingOutput(&pOutput);

        pOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, 0, &num, nullptr);

        instance->resDescriptors.resize(num);
        pOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, 0, &num, instance->resDescriptors.data());
        pOutput->Release();

        for (auto i = 0; i < instance->resDescriptors.size(); i++)
        {
                if (instance->resDescriptors[i].Scaling != DXGI_MODE_SCALING_UNSPECIFIED)
                {
                        instance->resDescriptors.erase(instance->resDescriptors.begin() + i);
                }
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

        // Create supported resolutions
        instance->SupportedResolutions();

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

        // "Press A, S, and D to continue. . . "
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

        // "Hold Left Click to Move. . . "
        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          E_MENU_CATEGORIES::MainMenu,
                          L"../Assets/2d/Text/calibri.spritefont",
                          "Hold Left Click to Move",
                          0.06f,
                          0.0f,
                          0.1f,
                          false,
                          false);

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
                          "Windowed",
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
                          0.2f,
                          false,
                          true,
                          true,
                          pauseButtonWidth,
                          pauseButtonHeight);

        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          E_MENU_CATEGORIES::OptionsMenu,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Apply",
                          0.05f,
                          0.0f,
                          -0.25f,
                          false,
                          true,
                          true,
                          pauseButtonWidth,
                          pauseButtonHeight);

        // Options Submenu
        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          E_MENU_CATEGORIES::OptionsSubmenu,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Off",
                          0.04f,
                          0.0f,
                          0.08f,
                          false,
                          false);

        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          E_MENU_CATEGORIES::OptionsSubmenu,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "On",
                          0.04f,
                          0.0f,
                          0.08f,
                          false,
                          false);

        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          E_MENU_CATEGORIES::OptionsSubmenu,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "<",
                          0.04f,
                          -0.12f,
                          0.3f,
                          false,
                          true);

        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          E_MENU_CATEGORIES::OptionsSubmenu,
                          L"../Assets/2d/Text/myfile.spritefont",
                          ">",
                          0.04f,
                          0.12f,
                          0.3f,
                          false,
                          true);

        for (auto i = 0; i < instance->resDescriptors.size(); i++)
        {
                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::OptionsSubmenu,
                                  L"../Assets/2d/Text/myfile.spritefont",
                                  std::to_string(instance->resDescriptors[i].Width) + "x" +
                                      std::to_string(instance->resDescriptors[i].Height),
                                  0.04f,
                                  0.0f,
                                  0.3f,
                                  false,
                                  false);
        }

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

        // Controls Menu
        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          E_MENU_CATEGORIES::ControlsMenu,
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
        // Pause Menu
        {
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

                        // Sprites
                        for (int i = 0; i < instance->m_AllSprites[E_MENU_CATEGORIES::OptionsSubmenu].size(); i++)
                        {
                                instance->m_AllSprites[E_MENU_CATEGORIES::OptionsSubmenu][i].mEnabled = true;
                        }
                        // Text
                        for (int i = 0; i < instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu].size(); i++)
                        {
                                instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][i].mEnabled = true;
                        }

                        if (instance->CSettings.m_IsFullscreen == false)
                        {
                                instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][0].mEnabled = false; // Off
                                instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][1].mEnabled = true;  // On
                        }
                        else
                        {
                                instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][0].mEnabled = true;  // Off
                                instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][1].mEnabled = false; // On
                        }

                        for (int i = 4; i < instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu].size(); i++)
                        {
                                instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][i].mEnabled = false;
                        }
                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][instance->CSettings.m_Resolution + 4].mEnabled =
                            true;
                });

                // Controls Button
                instance->m_AllSprites[E_MENU_CATEGORIES::PauseMenu][4].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
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
                        for (int i = 0; i < instance->m_AllSprites[E_MENU_CATEGORIES::ControlsMenu].size(); i++)
                        {
                                instance->m_AllSprites[E_MENU_CATEGORIES::ControlsMenu][i].mEnabled = true;
                        }
                        // Text
                        for (int i = 0; i < instance->m_AllFonts[E_MENU_CATEGORIES::ControlsMenu].size(); i++)
                        {
                                instance->m_AllFonts[E_MENU_CATEGORIES::ControlsMenu][i].mEnabled = true;
                        }
                });

                // Exit Button
                instance->m_AllSprites[E_MENU_CATEGORIES::PauseMenu][5].OnMouseDown.AddEventListener(
                    [](UIMouseEvent* e) { GEngine::Get()->RequestGameExit(); });
        }

        // Options
        {
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

                        // Disable all sprites for the options submenu
                        for (int i = 0; i < instance->m_AllSprites[E_MENU_CATEGORIES::OptionsSubmenu].size(); i++)
                        {
                                instance->m_AllSprites[E_MENU_CATEGORIES::OptionsSubmenu][i].mEnabled = false;
                        }
                        // Disable all text for the options submenu
                        for (int i = 0; i < instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu].size(); i++)
                        {
                                instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][i].mEnabled = false;
                        }


                        instance->CSettings.m_Resolution   = instance->PSettings.m_Resolution;
                        instance->CSettings.Volume         = instance->PSettings.Volume;
                        instance->CSettings.m_IsFullscreen = instance->PSettings.m_IsFullscreen;

                        instance->AdjustResolution(instance->m_window,
                                                   instance->resDescriptors[instance->PSettings.m_Resolution].Width,
                                                   instance->resDescriptors[instance->PSettings.m_Resolution].Height);

                        instance->m_RenderSystem->SetFullscreen(instance->PSettings.m_IsFullscreen);
                });

                // Window Mode
                instance->m_AllSprites[E_MENU_CATEGORIES::OptionsMenu][1].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                        if (instance->CSettings.m_IsFullscreen == false)
                        {
                                instance->CSettings.m_IsFullscreen                                  = true;
                                instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][0].mEnabled = true;  // Off
                                instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][1].mEnabled = false; // On
                                instance->CSettings.m_Resolution                                    = 8;
                        }
                        else
                        {
                                instance->CSettings.m_IsFullscreen                                  = false;
                                instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][0].mEnabled = false; // Off
                                instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][1].mEnabled = true;  // On
                        }
                        instance->m_RenderSystem->SetFullscreen(instance->CSettings.m_IsFullscreen);
                });

                // Apply Button
                instance->m_AllSprites[E_MENU_CATEGORIES::OptionsMenu][3].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
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

                        // Disable all sprites for the options submenu
                        for (int i = 0; i < instance->m_AllSprites[E_MENU_CATEGORIES::OptionsSubmenu].size(); i++)
                        {
                                instance->m_AllSprites[E_MENU_CATEGORIES::OptionsSubmenu][i].mEnabled = false;
                        }
                        // Disable all text for the options submenu
                        for (int i = 0; i < instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu].size(); i++)
                        {
                                instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][i].mEnabled = false;
                        }


                        instance->PSettings.m_Resolution   = instance->CSettings.m_Resolution;
                        instance->PSettings.Volume         = instance->CSettings.Volume;
                        instance->PSettings.m_IsFullscreen = instance->CSettings.m_IsFullscreen;

                        instance->AdjustResolution(instance->m_window,
                                                   instance->resDescriptors[instance->CSettings.m_Resolution].Width,
                                                   instance->resDescriptors[instance->CSettings.m_Resolution].Height);

                        instance->m_RenderSystem->SetFullscreen(instance->CSettings.m_IsFullscreen);
                });

                // Left Resolution Button
                instance->m_AllSprites[E_MENU_CATEGORIES::OptionsSubmenu][0].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                        if (instance->CSettings.m_Resolution - 1 <= -1)
                        {
                                instance->CSettings.m_Resolution = 8;
                        }
                        else
                        {
                                instance->CSettings.m_Resolution--;
                        }

                        for (int i = 4; i < instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu].size(); i++)
                        {
                                instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][i].mEnabled = false;
                        }
                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][instance->CSettings.m_Resolution + 4].mEnabled =
                            true;
                        // Change Resolution HERE
                        instance->AdjustResolution(instance->m_window,
                                                   instance->resDescriptors[instance->CSettings.m_Resolution].Width,
                                                   instance->resDescriptors[instance->CSettings.m_Resolution].Height);
                });

                // Right Resolution Button
                instance->m_AllSprites[E_MENU_CATEGORIES::OptionsSubmenu][1].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                        if (instance->CSettings.m_Resolution + 1 >= 9)
                        {
                                instance->CSettings.m_Resolution = 0;
                        }
                        else
                        {
                                instance->CSettings.m_Resolution++;
                        }

                        for (int i = 4; i < instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu].size(); i++)
                        {
                                instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][i].mEnabled = false;
                        }
                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][instance->CSettings.m_Resolution + 4].mEnabled =
                            true;
                        // Change Resolution HERE
                        instance->AdjustResolution(instance->m_window,
                                                   instance->resDescriptors[instance->CSettings.m_Resolution].Width,
                                                   instance->resDescriptors[instance->CSettings.m_Resolution].Height);
                });
        }

        // Level Select
        {
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

        // Controls Select
        {
                // Back Button
                instance->m_AllSprites[E_MENU_CATEGORIES::ControlsMenu][0].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                        // Back button to go from the options menu to the pause menu

                        // Disable all sprites for the options
                        for (int i = 0; i < instance->m_AllSprites[E_MENU_CATEGORIES::ControlsMenu].size(); i++)
                        {
                                instance->m_AllSprites[E_MENU_CATEGORIES::ControlsMenu][i].mEnabled = false;
                        }
                        // Disable all text for the options
                        for (int i = 0; i < instance->m_AllFonts[E_MENU_CATEGORIES::ControlsMenu].size(); i++)
                        {
                                instance->m_AllFonts[E_MENU_CATEGORIES::ControlsMenu][i].mEnabled = false;
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

        if (instance->m_AllFonts[E_MENU_CATEGORIES::MainMenu][2].mEnabled == true)
        {
                if (GCoreInput::GetMouseState(MouseCode::LeftClick) == KeyState::Down)
                {
                        instance->m_AllFonts[E_MENU_CATEGORIES::MainMenu][2].mEnabled = false;
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