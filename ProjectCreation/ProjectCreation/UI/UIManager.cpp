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

UIManager* UIManager::instance;


//Adding UI
void UIManager::AddSprite(ID3D11Device*                 device,
                          ID3D11DeviceContext*          deviceContext,
                          std::vector<SpriteComponent>& SpriteVector,
                          const wchar_t*                FileName,
                          float                         PositionX,
                          float                         PositionY,
                          float                         scaleX,
                          float                         scaleY,
                          bool                          enabled)
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


        // Add the origin to the sprite
        cSprite.mOrigin.x = (float)(TextureDesc.Width * 0.5f);
        cSprite.mOrigin.y = (float)(TextureDesc.Height * 0.5f);

        // Add the width and height to the sprite
        cSprite.mWidth  = TextureDesc.Width;
        cSprite.mHeight = TextureDesc.Height;

        // Sprite Screen Position
        cSprite.SetPosition(PositionX, PositionY);

        // Set the Sprite to enabled
        cSprite.mEnabled = enabled;

        // Scale
        cSprite.mScaleX = scaleX;
        cSprite.mScaleY = scaleY;

        // Rectangle
        cSprite.mRectangle.center = cSprite.mOrigin;
        cSprite.mRectangle.extents = {cSprite.mOrigin.x - (TextureDesc.Width * 0.5f),
                                      cSprite.mOrigin.y + (TextureDesc.Height * 0.5f),
                                      cSprite.mOrigin.y + (TextureDesc.Height * 0.5f),
                                      cSprite.mOrigin.y - (TextureDesc.Height * 0.5f)};

        // Push back to the vector
        SpriteVector.push_back(cSprite);

        // Reset everything that needs to be
        Texture.Reset();
        resource.Reset();
}

// Adds a sprite to the vector of text
// Currently PositionX and PositionY are not used
void UIManager::AddText(ID3D11Device*                 device,
                        ID3D11DeviceContext*          deviceContext,
                        std::vector<FontComponent*>&  FontVector,
                        std::vector<SpriteComponent>& SpriteVector,
                        const wchar_t*                FileName,
                        std::string                   TextDisplay,
                        float                         PositionX,
                        float                         PositionY,
                        bool                          AddButton,
                        bool                          enabled)
{
        FontComponent* createText = new FontComponent();

        createText->mSpriteFont  = std::make_unique<DirectX::SpriteFont>(device, FileName);
        createText->mTextDisplay = TextDisplay;

        // Set the Main Menu text to enabled
        createText->mEnabled = enabled;

        // Create Dimensions
        createText->mOrigin = DirectX::XMVECTOR{createText->mSpriteFont->MeasureString(TextDisplay.c_str()).m128_f32[0] * 0.5f,
                                                createText->mSpriteFont->MeasureString(TextDisplay.c_str()).m128_f32[1] * 0.5f};

        // Text Screen Position
        createText->mScreenOffset.x = PositionX;
        createText->mScreenOffset.y = PositionY;

        createText->MakeRectangle();

        // Error C2280
        // attempting to reference a deleted function
        // C:\Program Files(x86)\Microsoft Visual Studio\2019\Preview\VC\Tools\MSVC\14.20.27508\include\xmemory0 819
        // FIXED
        // Making the vector array an array of pointers fixed this issue
        FontVector.push_back(createText);

        if (AddButton)
        {
                instance->AddSprite(device,
                                    deviceContext,
                                    SpriteVector,
                                    L"../Assets/2d/Sprite/Grey Box Test.dds",
                                    PositionX,
                                    PositionY,
                                    0.4f,
                                    0.11f,
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


//Helpful Functions
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
{

}


//Pausing and Unpausing
void UIManager::MainTilteUnpause()
{
        GEngine::Get()->SetGamePaused(false);
        for (int i = 0; i < instance->m_MainSpriteFonts.size(); i++)
        {
                instance->m_MainSpriteFonts[i]->mEnabled = false;
        }
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
        for (int i = 0; i < instance->m_PauseSprites.size(); i++)
        {
                instance->m_PauseSprites[i].mEnabled = true;
        }
        // Text
        for (int i = 0; i < instance->m_PauseSpriteFonts.size(); i++)
        {
                instance->m_PauseSpriteFonts[i]->mEnabled = true;
        }

        // Options Menu Sprites
        for (int i = 0; i < instance->m_OptionsSprites.size(); i++)
        {
                instance->m_OptionsSprites[i].mEnabled = false;
        }
        // Text
        for (int i = 0; i < instance->m_OptionsSpriteFonts.size(); i++)
        {
                instance->m_OptionsSpriteFonts[i]->mEnabled = false;
        }

        // Level Select Menu Sprites
        for (int i = 0; i < instance->m_LevelSprites.size(); i++)
        {
                instance->m_LevelSprites[i].mEnabled = false;
        }
        // Text
        for (int i = 0; i < instance->m_LevelSpriteFonts.size(); i++)
        {
                instance->m_LevelSpriteFonts[i]->mEnabled = false;
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
        for (int i = 0; i < instance->m_PauseSprites.size(); i++)
        {
                instance->m_PauseSprites[i].mEnabled = false;
        }
        // Text
        for (int i = 0; i < instance->m_PauseSpriteFonts.size(); i++)
        {
                instance->m_PauseSpriteFonts[i]->mEnabled = false;
        }

        // Options Menu Sprites
        for (int i = 0; i < instance->m_OptionsSprites.size(); i++)
        {
                instance->m_OptionsSprites[i].mEnabled = false;
        }
        // Text
        for (int i = 0; i < instance->m_OptionsSpriteFonts.size(); i++)
        {
                instance->m_OptionsSpriteFonts[i]->mEnabled = false;
        }

        // Level Select Menu Sprites
        for (int i = 0; i < instance->m_LevelSprites.size(); i++)
        {
                instance->m_LevelSprites[i].mEnabled = false;
        }
        // Text
        for (int i = 0; i < instance->m_LevelSpriteFonts.size(); i++)
        {
                instance->m_LevelSpriteFonts[i]->mEnabled = false;
        }
}


//Core Function
void UIManager::Initialize(native_handle_type hwnd)
{
        assert(!instance);

        instance = new UIManager;

        instance->m_RenderSystem = GEngine::Get()->GetSystemManager()->GetSystem<RenderSystem>();

        instance->m_SpriteBatch = std::make_unique<DirectX::SpriteBatch>(instance->m_RenderSystem->m_Context);
        instance->m_States      = std::make_unique<DirectX::CommonStates>(instance->m_RenderSystem->m_Device);
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
                          instance->m_MainSpriteFonts,
                          instance->m_MainSprites,
                          L"../Assets/2d/Text/angel.spritefont",
                          "INANIS",
                          0.0f,
                          -0.35f,
                          false,
                          true);

        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          instance->m_MainSpriteFonts,
                          instance->m_MainSprites,
                          L"../Assets/2d/Text/calibri.spritefont",
                          "Press Spacebar to continue. . .",
                          0.0f,
                          0.0f,
                          false,
                          true);


        // Pause Menu
        instance->AddSprite(instance->m_RenderSystem->m_Device,
                            instance->m_RenderSystem->m_Context,
                            instance->m_PauseSprites,
                            L"../Assets/2d/Sprite/Grey Box Test.dds",
                            0.0f,
                            0.0f,
                            0.8f,
                            1.2f,
                            false);

        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          instance->m_PauseSpriteFonts,
                          instance->m_PauseSprites,
                          L"../Assets/2d/Text/angel.spritefont",
                          "INANIS",
                          0.0f,
                          -0.35f,
                          false,
                          false);

        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          instance->m_PauseSpriteFonts,
                          instance->m_PauseSprites,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Resume",
                          0.0f,
                          -0.15f,
                          true,
                          false);

        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          instance->m_PauseSpriteFonts,
                          instance->m_PauseSprites,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Levels",
                          0.0f,
                          -0.05f,
                          true,
                          false);

        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          instance->m_PauseSpriteFonts,
                          instance->m_PauseSprites,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Options",
                          0.0f,
                          0.05f,
                          true,
                          false);

        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          instance->m_PauseSpriteFonts,
                          instance->m_PauseSprites,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Exit",
                          0.0f,
                          0.15f,
                          true,
                          false);

        // Options Menu
        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          instance->m_OptionsSpriteFonts,
                          instance->m_OptionsSprites,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Back",
                          0.0f,
                          -0.15f,
                          true,
                          false);

        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          instance->m_OptionsSpriteFonts,
                          instance->m_OptionsSprites,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Fullscreen: ",
                          0.1f,
                          -0.05f,
                          false,
                          false);

        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          instance->m_OptionsSpriteFonts,
                          instance->m_OptionsSprites,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Resolution: ",
                          0.1f,
                          0.05f,
                          false,
                          false);

        // Level Menu
        instance->AddText(instance->m_RenderSystem->m_Device,
                          instance->m_RenderSystem->m_Context,
                          instance->m_LevelSpriteFonts,
                          instance->m_LevelSprites,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Back",
                          0.0f,
                          0.2f,
                          true,
                          false);

        // Events
        // Pause
        // Background Image
        instance->m_PauseSprites[0].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                std::cout << "OnPress Event" << std::endl;
                std::cout << "X: " << e->mouseX << "\t\t" << e->mouseY << std::endl;
                std::cout << std::endl;
        });

        instance->m_PauseSprites[0].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                std::cout << "OnPress Event2" << std::endl;
                std::cout << "X: " << e->mouseX << "\t\t" << e->mouseY << std::endl;
                std::cout << std::endl;
        });

        // Resume Button
        instance->m_PauseSprites[1].OnMouseDown.AddEventListener([](UIMouseEvent* e) { instance->Unpause(); });

        // Level Select Button
        instance->m_PauseSprites[2].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                // Sprites
                for (int i = 0; i < instance->m_PauseSprites.size(); i++)
                {
                        if (i != 0)
                        {
                                instance->m_PauseSprites[i].mEnabled = false;
                        }
                }
                // Text
                for (int i = 0; i < instance->m_PauseSpriteFonts.size(); i++)
                {
                        instance->m_PauseSpriteFonts[i]->mEnabled = false;
                }

                // Sprites
                for (int i = 0; i < instance->m_LevelSprites.size(); i++)
                {
                        instance->m_LevelSprites[i].mEnabled = true;
                }
                // Text
                for (int i = 0; i < instance->m_LevelSpriteFonts.size(); i++)
                {
                        instance->m_LevelSpriteFonts[i]->mEnabled = true;
                }
        });

        // Options Button
        instance->m_PauseSprites[3].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                // Sprites
                for (int i = 0; i < instance->m_PauseSprites.size(); i++)
                {
                        if (i != 0)
                        {
                                instance->m_PauseSprites[i].mEnabled = false;
                        }
                }
                // Text
                for (int i = 0; i < instance->m_PauseSpriteFonts.size(); i++)
                {
                        instance->m_PauseSpriteFonts[i]->mEnabled = false;
                }

                // Sprites
                for (int i = 0; i < instance->m_OptionsSprites.size(); i++)
                {
                        instance->m_OptionsSprites[i].mEnabled = true;
                }
                // Text
                for (int i = 0; i < instance->m_OptionsSpriteFonts.size(); i++)
                {
                        instance->m_OptionsSpriteFonts[i]->mEnabled = true;
                }
        });

        // Exit Button
        instance->m_PauseSprites[4].OnMouseDown.AddEventListener([](UIMouseEvent* e) { GEngine::Get()->RequestGameExit(); });

        // Options
        // Back Button
        instance->m_OptionsSprites[0].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                // Back button to go from the options menu to the pause menu

                // Disable all sprites for the options
                for (int i = 0; i < instance->m_OptionsSprites.size(); i++)
                {
                        instance->m_OptionsSprites[i].mEnabled = false;
                }
                // Disable all text for the options
                for (int i = 0; i < instance->m_OptionsSpriteFonts.size(); i++)
                {
                        instance->m_OptionsSpriteFonts[i]->mEnabled = false;
                }

                // Enable all sprites for the background
                for (int i = 1; i < instance->m_PauseSprites.size(); i++)
                {
                        instance->m_PauseSprites[i].mEnabled = true;
                }
                // Enable all text for the background
                for (int i = 0; i < instance->m_PauseSpriteFonts.size(); i++)
                {
                        instance->m_PauseSpriteFonts[i]->mEnabled = true;
                }
        });

        // Level Select
        // Back Button
        instance->m_LevelSprites[0].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                // Back button to go from the options menu to the pause menu

                // Disable all sprites for the options
                for (int i = 0; i < instance->m_LevelSprites.size(); i++)
                {
                        instance->m_LevelSprites[i].mEnabled = false;
                }
                // Disable all text for the options
                for (int i = 0; i < instance->m_LevelSpriteFonts.size(); i++)
                {
                        instance->m_LevelSpriteFonts[i]->mEnabled = false;
                }

                // Enable all sprites for the background
                for (int i = 1; i < instance->m_PauseSprites.size(); i++)
                {
                        instance->m_PauseSprites[i].mEnabled = true;
                }
                // Enable all text for the background
                for (int i = 0; i < instance->m_PauseSpriteFonts.size(); i++)
                {
                        instance->m_PauseSpriteFonts[i]->mEnabled = true;
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

        if (instance->m_InMenu)
        {
                ClipCursor(nullptr);
        }
        else
        {
                instance->UIClipCursor();
        }

        // Pause / Unpause
        if (instance->m_MainSpriteFonts[0]->mEnabled)
        {
                if (GCoreInput::GetKeyState(KeyCode::Space) == KeyState::DownFirst)
                {
                        instance->MainTilteUnpause();
                }
        }
        else
        {
                if (GCoreInput::GetKeyState(KeyCode::Esc) == KeyState::DownFirst)
                {
                        if (!instance->m_EscPush)
                        {
                                instance->Pause();
                                instance->m_EscPush = true;
                        }
                        else
                        {
                                instance->Unpause();
                                instance->m_EscPush = false;
                        }
                }
        }

        // Main Menu
        for (int i = 0; i < instance->m_MainSprites.size(); i++)
        {
                if (instance->m_MainSprites[i].mEnabled)
                {
                        if (GCoreInput::GetMouseState(MouseCode::LeftClick) == KeyState::Release)
                        {
                                XMVECTOR point = {GCoreInput::GetMouseWindowPosX(), GCoreInput::GetMouseWindowPosY()};
                                if (UI::PointInRect(instance->m_MainSprites[i].mRectangle, point))
                                {
                                        // Button Was Pressed
                                        UIMouseEvent e;


                                        e.mouseX = (float)GCoreInput::GetMouseWindowPosX();
                                        e.mouseY = (float)GCoreInput::GetMouseWindowPosY();
                                        e.sprite = &instance->m_PauseSprites[0];
                                        instance->m_MainSprites[i].OnMouseDown.Invoke(&e);
                                }
                        }

                        instance->m_SpriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred,
                                                       instance->m_States->NonPremultiplied());

                        XMVECTOR position =
                            DirectX::XMVECTOR{instance->m_MainSprites[i].mScreenOffset.x * instance->m_ScreenSize.x,
                                              instance->m_MainSprites[i].mScreenOffset.y * instance->m_ScreenSize.y} +
                            instance->m_ScreenCenter;

                        instance->m_SpriteBatch->Draw(
                            instance->m_MainSprites[i].mTexture,
                            position,
                            nullptr,
                            DirectX::Colors::White,
                            0.0f,
                            instance->m_MainSprites[i].mOrigin,
                            DirectX::XMVECTOR{instance->m_MainSprites[i].mScaleX, instance->m_MainSprites[i].mScaleY});


                        instance->m_SpriteBatch->End();
                }
        }
        for (int i = 0; i < instance->m_MainSpriteFonts.size(); i++)
        {
                if (instance->m_MainSpriteFonts[i]->mEnabled)
                {
                        instance->m_SpriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred,
                                                       instance->m_States->NonPremultiplied());

                        XMVECTOR position =
                            DirectX::XMVECTOR{instance->m_MainSpriteFonts[i]->mScreenOffset.x * instance->m_ScreenSize.x,
                                              instance->m_MainSpriteFonts[i]->mScreenOffset.y * instance->m_ScreenSize.y} +
                            instance->m_ScreenCenter;

                        instance->m_MainSpriteFonts[i]->mSpriteFont->DrawString(
                            instance->m_SpriteBatch.get(),
                            instance->m_MainSpriteFonts[i]->mTextDisplay.c_str(),
                            position,
                            DirectX::Colors::White,
                            0.0f,
                            instance->m_MainSpriteFonts[i]->mOrigin);

                        instance->m_SpriteBatch->End();
                }
        }

        // Pause Menu
        for (int i = 0; i < instance->m_PauseSprites.size(); i++)
        {
                if (instance->m_PauseSprites[i].mEnabled)
                {
                        if (GCoreInput::GetMouseState(MouseCode::LeftClick) == KeyState::Release)
                        {
                                XMVECTOR point = {GCoreInput::GetMouseWindowPosX(), GCoreInput::GetMouseWindowPosY()};
                                if (UI::PointInRect(instance->m_PauseSprites[i].mRectangle, point))
                                {
                                        // Button Was Pressed
                                        UIMouseEvent e;


                                        e.mouseX = (float)GCoreInput::GetMouseWindowPosX();
                                        e.mouseY = (float)GCoreInput::GetMouseWindowPosY();
                                        e.sprite = &instance->m_PauseSprites[0];
                                        instance->m_PauseSprites[i].OnMouseDown.Invoke(&e);
                                }
                        }

                        instance->m_SpriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred,
                                                       instance->m_States->NonPremultiplied());

                        XMVECTOR position =
                            DirectX::XMVECTOR{instance->m_PauseSprites[i].mScreenOffset.x * instance->m_ScreenSize.x,
                                              instance->m_PauseSprites[i].mScreenOffset.y * instance->m_ScreenSize.y} +
                            instance->m_ScreenCenter;

                        instance->m_SpriteBatch->Draw(
                            instance->m_PauseSprites[i].mTexture,
                            position,
                            nullptr,
                            DirectX::Colors::White,
                            0.0f,
                            instance->m_PauseSprites[i].mOrigin,
                            DirectX::XMVECTOR{instance->m_PauseSprites[i].mScaleX, instance->m_PauseSprites[i].mScaleY});


                        instance->m_SpriteBatch->End();
                }
        }
        for (int i = 0; i < instance->m_PauseSpriteFonts.size(); i++)
        {
                if (instance->m_PauseSpriteFonts[i]->mEnabled)
                {
                        instance->m_SpriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred,
                                                       instance->m_States->NonPremultiplied());

                        XMVECTOR position =
                            DirectX::XMVECTOR{instance->m_PauseSpriteFonts[i]->mScreenOffset.x * instance->m_ScreenSize.x,
                                              instance->m_PauseSpriteFonts[i]->mScreenOffset.y * instance->m_ScreenSize.y} +
                            instance->m_ScreenCenter;

                        instance->m_PauseSpriteFonts[i]->mSpriteFont->DrawString(
                            instance->m_SpriteBatch.get(),
                            instance->m_PauseSpriteFonts[i]->mTextDisplay.c_str(),
                            position,
                            DirectX::Colors::White,
                            0.0f,
                            instance->m_PauseSpriteFonts[i]->mOrigin);

                        instance->m_SpriteBatch->End();
                }
        }

        // Options Menu
        for (int i = 0; i < instance->m_OptionsSprites.size(); i++)
        {
                if (instance->m_OptionsSprites[i].mEnabled)
                {
                        if (GCoreInput::GetMouseState(MouseCode::LeftClick) == KeyState::Release)
                        {
                                XMVECTOR point = {GCoreInput::GetMouseWindowPosX(), GCoreInput::GetMouseWindowPosY()};
                                if (UI::PointInRect(instance->m_OptionsSprites[i].mRectangle, point))
                                {
                                        // Button Was Pressed
                                        UIMouseEvent e;

                                        e.mouseX = (float)GCoreInput::GetMouseWindowPosX();
                                        e.mouseY = (float)GCoreInput::GetMouseWindowPosY();
                                        e.sprite = &instance->m_PauseSprites[0];
                                        instance->m_OptionsSprites[i].OnMouseDown.Invoke(&e);
                                }
                        }

                        instance->m_SpriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred,
                                                       instance->m_States->NonPremultiplied());

                        XMVECTOR position =
                            DirectX::XMVECTOR{instance->m_OptionsSprites[i].mScreenOffset.x * instance->m_ScreenSize.x,
                                              instance->m_OptionsSprites[i].mScreenOffset.y * instance->m_ScreenSize.y} +
                            instance->m_ScreenCenter;

                        instance->m_SpriteBatch->Draw(
                            instance->m_OptionsSprites[i].mTexture,
                            position,
                            nullptr,
                            DirectX::Colors::White,
                            0.0f,
                            instance->m_OptionsSprites[i].mOrigin,
                            DirectX::XMVECTOR{instance->m_OptionsSprites[i].mScaleX, instance->m_OptionsSprites[i].mScaleY});


                        instance->m_SpriteBatch->End();
                }
        }
        for (int i = 0; i < instance->m_OptionsSpriteFonts.size(); i++)
        {
                if (instance->m_OptionsSpriteFonts[i]->mEnabled)
                {
                        instance->m_SpriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred,
                                                       instance->m_States->NonPremultiplied());

                        XMVECTOR position =
                            DirectX::XMVECTOR{instance->m_OptionsSpriteFonts[i]->mScreenOffset.x * instance->m_ScreenSize.x,
                                              instance->m_OptionsSpriteFonts[i]->mScreenOffset.y * instance->m_ScreenSize.y} +
                            instance->m_ScreenCenter;

                        instance->m_OptionsSpriteFonts[i]->mSpriteFont->DrawString(
                            instance->m_SpriteBatch.get(),
                            instance->m_OptionsSpriteFonts[i]->mTextDisplay.c_str(),
                            position,
                            DirectX::Colors::White,
                            0.0f,
                            instance->m_OptionsSpriteFonts[i]->mOrigin);

                        instance->m_SpriteBatch->End();
                }
        }

        // Level Menu
        for (int i = 0; i < instance->m_LevelSprites.size(); i++)
        {
                if (instance->m_LevelSprites[i].mEnabled)
                {
                        if (GCoreInput::GetMouseState(MouseCode::LeftClick) == KeyState::Release)
                        {
                                XMVECTOR point = {GCoreInput::GetMouseWindowPosX(), GCoreInput::GetMouseWindowPosY()};
                                if (UI::PointInRect(instance->m_LevelSprites[i].mRectangle, point))
                                {
                                        // Button Was Pressed
                                        UIMouseEvent e;

                                        e.mouseX = (float)GCoreInput::GetMouseWindowPosX();
                                        e.mouseY = (float)GCoreInput::GetMouseWindowPosY();
                                        e.sprite = &instance->m_PauseSprites[0];
                                        instance->m_LevelSprites[i].OnMouseDown.Invoke(&e);
                                }
                        }

                        instance->m_SpriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred,
                                                       instance->m_States->NonPremultiplied());

                        XMVECTOR position =
                            DirectX::XMVECTOR{instance->m_LevelSprites[i].mScreenOffset.x * instance->m_ScreenSize.x,
                                              instance->m_LevelSprites[i].mScreenOffset.y * instance->m_ScreenSize.y} +
                            instance->m_ScreenCenter;

                        instance->m_SpriteBatch->Draw(
                            instance->m_LevelSprites[i].mTexture,
                            position,
                            nullptr,
                            DirectX::Colors::White,
                            0.0f,
                            instance->m_LevelSprites[i].mOrigin,
                            DirectX::XMVECTOR{instance->m_LevelSprites[i].mScaleX, instance->m_LevelSprites[i].mScaleY});


                        instance->m_SpriteBatch->End();
                }
        }
        for (int i = 0; i < instance->m_LevelSpriteFonts.size(); i++)
        {
                if (instance->m_LevelSpriteFonts[i]->mEnabled)
                {
                        instance->m_SpriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred,
                                                       instance->m_States->NonPremultiplied());

                        XMVECTOR position =
                            DirectX::XMVECTOR{instance->m_LevelSpriteFonts[i]->mScreenOffset.x * instance->m_ScreenSize.x,
                                              instance->m_LevelSpriteFonts[i]->mScreenOffset.y * instance->m_ScreenSize.y} +
                            instance->m_ScreenCenter;

                        instance->m_LevelSpriteFonts[i]->mSpriteFont->DrawString(
                            instance->m_SpriteBatch.get(),
                            instance->m_LevelSpriteFonts[i]->mTextDisplay.c_str(),
                            position,
                            DirectX::Colors::White,
                            0.0f,
                            instance->m_LevelSpriteFonts[i]->mOrigin);

                        instance->m_SpriteBatch->End();
                }
        }
}

void UIManager::Shutdown()
{
        instance->m_States.reset();
        instance->m_SpriteBatch.reset();
        instance->m_background.Reset();
        // Pause Manu
        // Release Sprites
        for (int i = 0; i < instance->m_PauseSprites.size(); i++)
        {
                SAFE_RELEASE(instance->m_PauseSprites[i].mTexture);
        }
        // Release Fonts
        for (int i = 0; i < instance->m_PauseSpriteFonts.size(); i++)
        {
                SAFE_RELEASE(instance->m_PauseSpriteFonts[i]->mTexture);
                instance->m_PauseSpriteFonts[i]->mSpriteFont.reset();
        }

        // Main Menu
        // Release Sprites
        for (int i = 0; i < instance->m_MainSprites.size(); i++)
        {
                SAFE_RELEASE(instance->m_MainSprites[i].mTexture);
        }
        // Release Fonts
        for (int i = 0; i < instance->m_MainSpriteFonts.size(); i++)
        {
                SAFE_RELEASE(instance->m_MainSpriteFonts[i]->mTexture);
                instance->m_MainSpriteFonts[i]->mSpriteFont.reset();
        }

        // Options Menu
        // Release Sprites
        for (int i = 0; i < instance->m_OptionsSprites.size(); i++)
        {
                SAFE_RELEASE(instance->m_OptionsSprites[i].mTexture);
        }
        // Release Fonts
        for (int i = 0; i < instance->m_OptionsSpriteFonts.size(); i++)
        {
                SAFE_RELEASE(instance->m_OptionsSpriteFonts[i]->mTexture);
                instance->m_OptionsSpriteFonts[i]->mSpriteFont.reset();
        }

        // Level Menu
        // Release Sprites
        for (int i = 0; i < instance->m_LevelSprites.size(); i++)
        {
                SAFE_RELEASE(instance->m_LevelSprites[i].mTexture);
        }
        // Release Fonts
        for (int i = 0; i < instance->m_LevelSpriteFonts.size(); i++)
        {
                SAFE_RELEASE(instance->m_LevelSpriteFonts[i]->mTexture);
                instance->m_LevelSpriteFonts[i]->mSpriteFont.reset();
        }


        assert(instance);
        delete instance;
}