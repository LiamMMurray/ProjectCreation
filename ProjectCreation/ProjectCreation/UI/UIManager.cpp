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

// Adds a sprite to the vector of text

// Adds a sprite to the vector of sprites
// Currently PositionX and PositionY are not used
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

        // Set the Id of the Sprite for the Main Menu
        if (instance->m_PauseSprites.size() <= 0)
        {
                cSprite.mId = 1;
        }
        else
        {
                cSprite.mId = instance->m_PauseSprites[instance->m_PauseSprites.size() - 1].mId + 1;
        }
        // Set the Sprite to enabled
        cSprite.mEnabled = enabled;

        // Scale
        cSprite.mScaleX = scaleX;
        cSprite.mScaleY = scaleY;

        // Rectangle
        cSprite.MakeRectangle();


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
        createText->mScreenPos.x = PositionX;
        createText->mScreenPos.y = PositionY;

        createText->MakeRectangle();

        // Error C2280
        // attempting to reference a deleted function
        // C:\Program Files(x86)\Microsoft Visual Studio\2019\Preview\VC\Tools\MSVC\14.20.27508\include\xmemory0 819
        // FIXED
        // Making the vector array an array of pointers fixed this issue
        FontVector.push_back(createText);

        if (AddButton == true)
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
        m_fullscreenRect.right  = ScreenWidth;
        m_fullscreenRect.bottom = ScreenHeight;
}

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


void UIManager::Initialize(native_handle_type hwnd)
{
        assert(!instance);

        instance = new UIManager;

        auto renderSystem = GEngine::Get()->GetSystemManager()->GetSystem<RenderSystem>();

        instance->m_SpriteBatch = std::make_unique<DirectX::SpriteBatch>(renderSystem->m_Context);
        instance->m_States      = std::make_unique<DirectX::CommonStates>(renderSystem->m_Device);
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
        instance->AddText(renderSystem->m_Device,
                          renderSystem->m_Context,
                          instance->m_MainSpriteFonts,
                          instance->m_MainSprites,
                          L"../Assets/2d/Text/angel.spritefont",
                          "INANIS",
                          renderSystem->m_BackBufferWidth * 0.5f,
                          renderSystem->m_BackBufferHeight * 0.1f,
                          false,
                          true);

        instance->AddText(renderSystem->m_Device,
                          renderSystem->m_Context,
                          instance->m_MainSpriteFonts,
                          instance->m_MainSprites,
                          L"../Assets/2d/Text/calibri.spritefont",
                          "Press Spacebar to continue. . .",
                          renderSystem->m_BackBufferWidth * 0.5f,
                          renderSystem->m_BackBufferHeight * 0.5f,
                          false,
                          true);


        // Pause Menu
        instance->AddSprite(renderSystem->m_Device,
                            renderSystem->m_Context,
                            instance->m_PauseSprites,
                            L"../Assets/2d/Sprite/Grey Box Test.dds",
                            renderSystem->m_BackBufferWidth * 0.5f,
                            renderSystem->m_BackBufferHeight * 0.5f,
                            0.8f,
                            1.2f,
                            false);

        instance->AddText(renderSystem->m_Device,
                          renderSystem->m_Context,
                          instance->m_PauseSpriteFonts,
                          instance->m_PauseSprites,
                          L"../Assets/2d/Text/angel.spritefont",
                          "INANIS",
                          renderSystem->m_BackBufferWidth * 0.5f,
                          renderSystem->m_BackBufferHeight * 0.1f,
                          false,
                          false);

        instance->AddText(renderSystem->m_Device,
                          renderSystem->m_Context,
                          instance->m_PauseSpriteFonts,
                          instance->m_PauseSprites,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Resume",
                          renderSystem->m_BackBufferWidth * 0.5f,
                          renderSystem->m_BackBufferHeight * 0.5f - 80.0f,
                          true,
                          false);

        instance->AddText(renderSystem->m_Device,
                          renderSystem->m_Context,
                          instance->m_PauseSpriteFonts,
                          instance->m_PauseSprites,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Levels",
                          renderSystem->m_BackBufferWidth * 0.5f,
                          renderSystem->m_BackBufferHeight * 0.5f - 20.0f,
                          true,
                          false);

        instance->AddText(renderSystem->m_Device,
                          renderSystem->m_Context,
                          instance->m_PauseSpriteFonts,
                          instance->m_PauseSprites,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Options",
                          renderSystem->m_BackBufferWidth * 0.5f,
                          renderSystem->m_BackBufferHeight * 0.5f + 40.0f,
                          true,
                          false);

        instance->AddText(renderSystem->m_Device,
                          renderSystem->m_Context,
                          instance->m_PauseSpriteFonts,
                          instance->m_PauseSprites,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Exit",
                          renderSystem->m_BackBufferWidth * 0.5f,
                          renderSystem->m_BackBufferHeight * 0.5f + 100.0f,
                          true,
                          false);

        // Options Menu
        instance->AddText(renderSystem->m_Device,
                          renderSystem->m_Context,
                          instance->m_OptionsSpriteFonts,
                          instance->m_OptionsSprites,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Back",
                          renderSystem->m_BackBufferWidth * 0.5f,
                          renderSystem->m_BackBufferHeight * 0.5f - 140.0f,
                          true,
                          false);

        // Level Menu
        instance->AddText(renderSystem->m_Device,
                          renderSystem->m_Context,
                          instance->m_LevelSpriteFonts,
                          instance->m_LevelSprites,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Back",
                          renderSystem->m_BackBufferWidth * 0.5f,
                          renderSystem->m_BackBufferHeight * 0.5f - 140.0f,
                          true,
                          false);

        // Events
        // Background Image
        instance->m_PauseSprites[0].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                std::cout << "OnPress Event" << std::endl;
                std::cout << "Sprite id: " << e->sprite->mId << std::endl;
                std::cout << "X: " << e->mouseX << "\t\t" << e->mouseY << std::endl;
                std::cout << std::endl;
        });

        instance->m_PauseSprites[0].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                std::cout << "OnPress Event2" << std::endl;
                std::cout << "Sprite id: " << e->sprite->mId << std::endl;
                std::cout << "X: " << e->mouseX << "\t\t" << e->mouseY << std::endl;
                std::cout << std::endl;
        });

        instance->m_PauseSprites[1].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                // Sprites
                for (int i = 0; i < instance->m_PauseSprites.size(); i++)
                {
                        instance->m_PauseSprites[i].mEnabled = false;
                }

                // Text
                for (int i = 0; i < instance->m_PauseSpriteFonts.size(); i++)
                {
                        instance->m_PauseSpriteFonts[i]->mEnabled = false;
                }

                // Joe's code for unpausing the game here
                instance->m_InMenu = !instance->m_InMenu;
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
        });

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

        instance->m_PauseSprites[4].OnMouseDown.AddEventListener([](UIMouseEvent* e) { GEngine::Get()->RequestGameExit(); });

        // Options
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

        // Level
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
        if (instance->m_InMenu)
        {
                ClipCursor(nullptr);
        }
        else
        {
                instance->UIClipCursor();
        }

        if (instance->m_MainSpriteFonts[0]->mEnabled == true)
        {
                // ControllerManager::Get()->m_togglePauseInput = true;
                if (GCoreInput::GetKeyState(KeyCode::Space) == KeyState::DownFirst)
                {
                        // ControllerManager::Get()->m_togglePauseInput = !ControllerManager::Get()->m_togglePauseInput;
                        GEngine::Get()->SetGamePaused(false);
                        for (int i = 0; i < instance->m_MainSpriteFonts.size(); i++)
                        {
                                instance->m_MainSpriteFonts[i]->mEnabled = false;
                        }
                }
        }
        else
        {
                if (GCoreInput::GetKeyState(KeyCode::Esc) == KeyState::DownFirst)
                {
                        // ControllerSystem::Get()->m_togglePauseInput = !ControllerSystem::Get()->m_togglePauseInput;
                        instance->m_InMenu = !instance->m_InMenu;
                        GEngine::Get()->SetGamePaused(instance->m_InMenu);
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


                        // Change input from Control to Escape whenever implimented
                        // Disable or enable all Sprites based off of input
                        // Pause Menu
                        if (instance->m_PauseSprites[0].mEnabled == true)
                        {
                                // Sprites
                                for (int i = 0; i < instance->m_PauseSprites.size(); i++)
                                {
                                        instance->m_PauseSprites[i].mEnabled = false;
                                }
                        }
                        else
                        {
                                // Sprites
                                for (int i = 0; i < instance->m_PauseSprites.size(); i++)
                                {
                                        instance->m_PauseSprites[i].mEnabled = true;
                                }
                        }
                        if (instance->m_PauseSprites[0].mEnabled == true)
                        {
                                // Text
                                for (int i = 0; i < instance->m_PauseSpriteFonts.size(); i++)
                                {
                                        instance->m_PauseSpriteFonts[i]->mEnabled = true;
                                }
                        }
                        else
                        {
                                // Text
                                for (int i = 0; i < instance->m_PauseSpriteFonts.size(); i++)
                                {
                                        instance->m_PauseSpriteFonts[i]->mEnabled = false;
                                }
                        }

                        // Options Menu
                        // Sprites
                        for (int i = 0; i < instance->m_OptionsSprites.size(); i++)
                        {
                                instance->m_OptionsSprites[i].mEnabled = false;
                        }
                        // Text
                        for (int i = 0; i < instance->m_OptionsSpriteFonts.size(); i++)
                        {
                                instance->m_OptionsSpriteFonts[i]->mEnabled = false;
                        }
                }
        }

        // Main Menu
        for (int i = 0; i < instance->m_MainSprites.size(); i++)
        {
                if (instance->m_MainSprites[i].mEnabled == true)
                {
                        if (GCoreInput::GetMouseState(MouseCode::LeftClick) == KeyState::Release)
                        {
                                if (PtInRect(&instance->m_MainSprites[i].mRectangle,
                                             {GCoreInput::GetMouseWindowPosX(), GCoreInput::GetMouseWindowPosY()}))
                                {
                                        // Button Was Pressed
                                        UIMouseEvent e;


                                        e.mouseX = GCoreInput::GetMouseWindowPosX();
                                        e.mouseY = GCoreInput::GetMouseWindowPosY();
                                        e.sprite = &instance->m_PauseSprites[0];
                                        instance->m_MainSprites[i].OnMouseDown.Invoke(&e);
                                }
                        }

                        instance->m_SpriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred,
                                                       instance->m_States->NonPremultiplied());

                        instance->m_SpriteBatch->Draw(
                            instance->m_MainSprites[i].mTexture,
                            DirectX::XMVECTOR{instance->m_MainSprites[i].mScreenPos.x, instance->m_MainSprites[i].mScreenPos.y},
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
                if (instance->m_MainSpriteFonts[i]->mEnabled == true)
                {
                        instance->m_SpriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred,
                                                       instance->m_States->NonPremultiplied());

                        instance->m_MainSpriteFonts[i]->mSpriteFont->DrawString(
                            instance->m_SpriteBatch.get(),
                            instance->m_MainSpriteFonts[i]->mTextDisplay.c_str(),
                            instance->m_MainSpriteFonts[i]->mScreenPos,
                            DirectX::Colors::White,
                            0.0f,
                            instance->m_MainSpriteFonts[i]->mOrigin);

                        instance->m_SpriteBatch->End();
                }
        }

        // Pause Menu
        for (int i = 0; i < instance->m_PauseSprites.size(); i++)
        {
                if (instance->m_PauseSprites[i].mEnabled == true)
                {
                        if (GCoreInput::GetMouseState(MouseCode::LeftClick) == KeyState::Release)
                        {
                                if (PtInRect(&instance->m_PauseSprites[i].mRectangle,
                                             {GCoreInput::GetMouseWindowPosX(), GCoreInput::GetMouseWindowPosY()}))
                                {
                                        // Button Was Pressed
                                        UIMouseEvent e;


                                        e.mouseX = GCoreInput::GetMouseWindowPosX();
                                        e.mouseY = GCoreInput::GetMouseWindowPosY();
                                        e.sprite = &instance->m_PauseSprites[0];
                                        instance->m_PauseSprites[i].OnMouseDown.Invoke(&e);
                                }
                        }

                        instance->m_SpriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred,
                                                       instance->m_States->NonPremultiplied());

                        instance->m_SpriteBatch->Draw(
                            instance->m_PauseSprites[i].mTexture,
                            DirectX::XMVECTOR{instance->m_PauseSprites[i].mScreenPos.x,
                                              instance->m_PauseSprites[i].mScreenPos.y},
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
                if (instance->m_PauseSpriteFonts[i]->mEnabled == true)
                {
                        instance->m_SpriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred,
                                                       instance->m_States->NonPremultiplied());

                        instance->m_PauseSpriteFonts[i]->mSpriteFont->DrawString(
                            instance->m_SpriteBatch.get(),
                            instance->m_PauseSpriteFonts[i]->mTextDisplay.c_str(),
                            instance->m_PauseSpriteFonts[i]->mScreenPos,
                            DirectX::Colors::White,
                            0.0f,
                            instance->m_PauseSpriteFonts[i]->mOrigin);

                        instance->m_SpriteBatch->End();
                }
        }

        // Options Menu
        for (int i = 0; i < instance->m_OptionsSprites.size(); i++)
        {
                if (instance->m_OptionsSprites[i].mEnabled == true)
                {
                        if (GCoreInput::GetMouseState(MouseCode::LeftClick) == KeyState::Release)
                        {
                                if (PtInRect(&instance->m_OptionsSprites[i].mRectangle,
                                             {GCoreInput::GetMouseWindowPosX(), GCoreInput::GetMouseWindowPosY()}))
                                {
                                        // Button Was Pressed
                                        UIMouseEvent e;

                                        e.mouseX = GCoreInput::GetMouseWindowPosX();
                                        e.mouseY = GCoreInput::GetMouseWindowPosY();
                                        e.sprite = &instance->m_PauseSprites[0];
                                        instance->m_OptionsSprites[i].OnMouseDown.Invoke(&e);
                                }
                        }

                        instance->m_SpriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred,
                                                       instance->m_States->NonPremultiplied());

                        instance->m_SpriteBatch->Draw(
                            instance->m_OptionsSprites[i].mTexture,
                            DirectX::XMVECTOR{instance->m_OptionsSprites[i].mScreenPos.x,
                                              instance->m_OptionsSprites[i].mScreenPos.y},
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
                if (instance->m_OptionsSpriteFonts[i]->mEnabled == true)
                {
                        instance->m_SpriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred,
                                                       instance->m_States->NonPremultiplied());

                        instance->m_OptionsSpriteFonts[i]->mSpriteFont->DrawString(
                            instance->m_SpriteBatch.get(),
                            instance->m_OptionsSpriteFonts[i]->mTextDisplay.c_str(),
                            instance->m_OptionsSpriteFonts[i]->mScreenPos,
                            DirectX::Colors::White,
                            0.0f,
                            instance->m_OptionsSpriteFonts[i]->mOrigin);

                        instance->m_SpriteBatch->End();
                }
        }

        // Level Menu
        for (int i = 0; i < instance->m_LevelSprites.size(); i++)
        {
                if (instance->m_LevelSprites[i].mEnabled == true)
                {
                        if (GCoreInput::GetMouseState(MouseCode::LeftClick) == KeyState::Release)
                        {
                                if (PtInRect(&instance->m_OptionsSprites[i].mRectangle,
                                             {GCoreInput::GetMouseWindowPosX(), GCoreInput::GetMouseWindowPosY()}))
                                {
                                        // Button Was Pressed
                                        UIMouseEvent e;

                                        e.mouseX = GCoreInput::GetMouseWindowPosX();
                                        e.mouseY = GCoreInput::GetMouseWindowPosY();
                                        e.sprite = &instance->m_PauseSprites[0];
                                        instance->m_LevelSprites[i].OnMouseDown.Invoke(&e);
                                }
                        }

                        instance->m_SpriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred,
                                                       instance->m_States->NonPremultiplied());

                        instance->m_SpriteBatch->Draw(
                            instance->m_LevelSprites[i].mTexture,
                            DirectX::XMVECTOR{instance->m_LevelSprites[i].mScreenPos.x,
                                              instance->m_LevelSprites[i].mScreenPos.y},
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
                if (instance->m_LevelSpriteFonts[i]->mEnabled == true)
                {
                        instance->m_SpriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred,
                                                       instance->m_States->NonPremultiplied());

                        instance->m_LevelSpriteFonts[i]->mSpriteFont->DrawString(
                            instance->m_SpriteBatch.get(),
                            instance->m_LevelSpriteFonts[i]->mTextDisplay.c_str(),
                            instance->m_LevelSpriteFonts[i]->mScreenPos,
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
