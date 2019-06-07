#include "UIManager.h"
#include <iostream>
#include "../Engine/CoreInput/CoreInput.h"
#include "../Engine/GEngine.h"
#include "../Rendering/RenderingSystem.h"

#include "../Engine/Controller/ControllerManager.h"

#define WIN32_LEAN_AND_MEAN // Gets rid of bloat on Windows.h
#define NOMINMAX
#include <Windows.h>

#include "../Utility/Macros/DirectXMacros.h"

UIManager* UIManager::instance;

/*
    ---List of UI Elements---
    [0] -> [4] Pause Menu

[0] = Pause Menu Background Element
[1] = Pause Menu Resume Button
[2] = Pause Menu Main Menu Button
[3] = Pause Menu Options Button
[4] = Pause Menu Exit Button

[5] -> [8] Options Menu



    ---List of UI Text---
    [0] -> [4] Pause Menu

[0] = Pause Menu Resume Button
[1] = Pause Menu Main Menu Button
[2] = Pause Menu Options Button
[3] = Pause Menu Exit Button


    [4] -> [7] Options Menu

*/

// Adds a sprite to the vector of sprites
// Currently PositionX and PositionY are not used
void UIManager::AddSprite(ID3D11Device*        device,
                          ID3D11DeviceContext* deviceContext,
                          const wchar_t*       FileName,
                          float                PositionX,
                          float                PositionY,
                          float                scaleX,
                          float                scaleY)
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
        if (instance->mSprites.size() <= 0)
        {
                cSprite.mId = 1;
        }
        else
        {
                cSprite.mId = instance->mSprites[instance->mSprites.size() - 1].mId + 1;
        }
        // Set the Sprite to enabled
        cSprite.mEnabled = false;

        // Scale
        cSprite.mScaleX = scaleX;
        cSprite.mScaleY = scaleY;

        // Rectangle
        cSprite.MakeRectangle();


        // Push back to the vector
        instance->mSprites.push_back(cSprite);

        // Reset everything that needs to be
        Texture.Reset();
        resource.Reset();
}

void UIManager::RemoveSprite(int id)
{}

// Adds a sprite to the vector of text
// Currently PositionX and PositionY are not used
void UIManager::AddText(ID3D11Device*        device,
                        ID3D11DeviceContext* deviceContext,
                        const wchar_t*       FileName,
                        std::string          TextDisplay,
                        float                PositionX,
                        float                PositionY)
{
        FontComponent* createText = new FontComponent();

        createText->mSpriteFont  = std::make_unique<DirectX::SpriteFont>(device, FileName);
        createText->mTextDisplay = TextDisplay;

        // Set the Main Menu text to enabled
        createText->mEnabled = false;

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
        instance->mSpriteFonts.push_back(createText);

        instance->AddSprite(device, deviceContext, L"../Assets/2d/Sprite/Grey Box Test.dds", PositionX, PositionY, 0.4f, 0.11f);
}

void UIManager::RemoveText(int id)
{}


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

        instance->mSpriteBatch = std::make_unique<DirectX::SpriteBatch>(renderSystem->m_Context);
        instance->mStates      = std::make_unique<DirectX::CommonStates>(renderSystem->m_Device);
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

        instance->AddSprite(renderSystem->m_Device,
                            renderSystem->m_Context,
                            L"../Assets/2d/Sprite/Grey Box Test.dds",
                            650.0f,
                            300.0f,
                            0.8f,
                            1.2f);

        instance->AddText(renderSystem->m_Device,
                          renderSystem->m_Context,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Resume",
                          instance->mSprites[0].mScreenPos.x,
                          instance->mSprites[0].mScreenPos.y - 80.0f);

        instance->AddText(renderSystem->m_Device,
                          renderSystem->m_Context,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Main Menu",
                          instance->mSprites[0].mScreenPos.x,
                          instance->mSprites[0].mScreenPos.y - 20.0f);

        instance->AddText(renderSystem->m_Device,
                          renderSystem->m_Context,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Options",
                          instance->mSprites[0].mScreenPos.x,
                          instance->mSprites[0].mScreenPos.y + 40.0f);

        instance->AddText(renderSystem->m_Device,
                          renderSystem->m_Context,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Exit",
                          instance->mSprites[0].mScreenPos.x,
                          instance->mSprites[0].mScreenPos.y + 100.0f);

        // Events
        instance->mSprites[0].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                std::cout << "OnPress Event" << std::endl;
                std::cout << "Sprite id: " << e->sprite->mId << std::endl;
                std::cout << "X: " << e->mouseX << "\t\t" << e->mouseY << std::endl;
                std::cout << std::endl;
        });

        instance->mSprites[0].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                std::cout << "OnPress Event2" << std::endl;
                std::cout << "Sprite id: " << e->sprite->mId << std::endl;
                std::cout << "X: " << e->mouseX << "\t\t" << e->mouseY << std::endl;
                std::cout << std::endl;
        });

        /*
        instance->mSprites[0].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                for (int i = 0; i < instance->mSpriteFonts.size(); i++)
                {
                        if (instance->mSpriteFonts[i]->mEnabled == false)
                        {
                                instance->mSpriteFonts[i]->mEnabled = true;
                        }
                        else
                        {
                                instance->mSpriteFonts[i]->mEnabled = false;
                        }
                }
        });
        */

        instance->mSprites[1].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                // Change input from Control to Escape whenever implimented
                // Disable or enable all Sprites based off of input
                // Sprites
                for (int i = 0; i < instance->mSprites.size(); i++)
                {
                        instance->mSprites[i].mEnabled = false;
                }
                // Text

                for (int i = 0; i < instance->mSpriteFonts.size(); i++)
                {
                        instance->mSpriteFonts[i]->mEnabled = false;
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

        instance->mSprites[3].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                // Sprites
                for (int i = 0; i < instance->mSprites.size(); i++)
                {
                        if (instance->mSprites[i].mEnabled == true && i != 0)
                        {
                                instance->mSprites[i].mEnabled = false;
                        }
                        else
                        {
                                instance->mSprites[i].mEnabled = true;
                        }
                }
                // Text

                for (int i = 0; i < instance->mSpriteFonts.size(); i++)
                {
                        if (instance->mSpriteFonts[i]->mEnabled == false)
                        {
                                instance->mSpriteFonts[i]->mEnabled = true;
                        }
                        else
                        {
                                instance->mSpriteFonts[i]->mEnabled = false;
                        }
                }
        });

        instance->mSprites[4].OnMouseDown.AddEventListener([](UIMouseEvent* e) { exit(EXIT_SUCCESS); });
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


        if (GCoreInput::GetKeyState(KeyCode::Esc) == KeyState::DownFirst)
        {
                static POINT cursorPos;

                instance->m_InMenu                           = !instance->m_InMenu;
                if (instance->m_InMenu)
                {
                        while (ShowCursor(TRUE) < 0)
                                ;
                        SetCursorPos(cursorPos.x, cursorPos.y);
                }
                else
                {
                        GetCursorPos(&cursorPos);
                        while (ShowCursor(FALSE) >= 0)
                                ;
                }


                // Change input from Control to Escape whenever implimented
                // Disable or enable all Sprites based off of input
                // Sprites
                if (instance->mSprites[0].mEnabled == true)
                {
                        // Sprites
                        for (int i = 0; i < instance->mSprites.size(); i++)
                        {
                                instance->mSprites[i].mEnabled = false;
                        }
                }
                else
                {
                        // Sprites
                        for (int i = 0; i < instance->mSprites.size(); i++)
                        {
                                instance->mSprites[i].mEnabled = true;
                        }
                }

                if (instance->mSprites[0].mEnabled == true)
                {
                        // Text
                        for (int i = 0; i < instance->mSpriteFonts.size(); i++)
                        {
                                instance->mSpriteFonts[i]->mEnabled = true;
                        }
                }
                else
                {
                        // Text
                        for (int i = 0; i < instance->mSpriteFonts.size(); i++)
                        {
                                instance->mSpriteFonts[i]->mEnabled = false;
                        }
                }


                // Pause Game Afterwards
        }


        // Sprite Display
        for (int i = 0; i < instance->mSprites.size(); i++)
        {
                if (instance->mSprites[i].mEnabled == true)
                {
                        if (GCoreInput::GetMouseState(MouseCode::LeftClick) == KeyState::Release)
                        {
                                if (PtInRect(&instance->mSprites[i].mRectangle,
                                             {GCoreInput::GetMouseWindowPosX(), GCoreInput::GetMouseWindowPosY()}))
                                {
                                        // Button Was Pressed
                                        UIMouseEvent e;

                                        e.mouseX = (float)GCoreInput::GetMouseWindowPosX();
                                        e.mouseY = (float)GCoreInput::GetMouseWindowPosY();
                                        e.sprite = &instance->mSprites[0];
                                        instance->mSprites[i].OnMouseDown.Invoke(&e);
                                }
                        }

                        instance->mSpriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred,
                                                      instance->mStates->NonPremultiplied());


                        instance->mSpriteBatch->Draw(
                            instance->mSprites[i].mTexture,
                            DirectX::XMVECTOR{instance->mSprites[i].mScreenPos.x, instance->mSprites[i].mScreenPos.y},
                            nullptr,
                            DirectX::Colors::White,
                            0.0f,
                            instance->mSprites[i].mOrigin,
                            DirectX::XMVECTOR{instance->mSprites[i].mScaleX, instance->mSprites[i].mScaleY});


                        instance->mSpriteBatch->End();
                }
        }
        // Text Display
        for (int i = 0; i < instance->mSpriteFonts.size(); i++)
        {
                if (instance->mSpriteFonts[i]->mEnabled == true)
                {
                        instance->mSpriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred,
                                                      instance->mStates->NonPremultiplied());

                        instance->mSpriteFonts[i]->mSpriteFont->DrawString(instance->mSpriteBatch.get(),
                                                                           instance->mSpriteFonts[i]->mTextDisplay.c_str(),
                                                                           instance->mSpriteFonts[i]->mScreenPos,
                                                                           DirectX::Colors::White,
                                                                           0.0f,
                                                                           instance->mSpriteFonts[i]->mOrigin);

                        instance->mSpriteBatch->End();
                }
        }
}

void UIManager::Shutdown()
{
        instance->mStates.reset();

        // Release Sprites
        for (int i = 0; i < instance->mSprites.size(); i++)
        {
                SAFE_RELEASE(instance->mSprites[i].mTexture);
        }
        // Release Fonts
        for (int i = 0; i < instance->mSpriteFonts.size(); i++)
        {
                SAFE_RELEASE(instance->mSpriteFonts[i]->mTexture);
                instance->mSpriteFonts[i]->mSpriteFont.reset();
        }


        assert(instance);
        delete instance;
}
