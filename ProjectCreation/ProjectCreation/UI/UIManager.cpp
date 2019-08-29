#include "UIManager.h"
#include <Windows.h>
#include <iostream>
#include "../Engine/CoreInput/InputActions.h"

#include "../Engine/Controller/ControllerSystem.h"
#include "../Engine/CoreInput/CoreInput.h"
#include "../Engine/GEngine.h"
#include "../Rendering/RenderingSystem.h"
#include "../Utility/Hashing/PairHash.h"
#include "../Utility/Macros/DirectXMacros.h"
#include "../Utility/MemoryLeakDetection.h"

#include "../Engine/ConsoleWindow/ConsoleWindow.h"
#include "../Engine/Levels/TutorialLevel.h"
class TutorialLevel;

#define WIN32_LEAN_AND_MEAN // Gets rid of bloat on Windows.h
#define NOMINMAX
UIManager* UIManager::instance;
using namespace DirectX;
// Adds Sprites to the vector of Sprites
int UIManager::AddSprite(ID3D11Device*        device,
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
        cSprite.mScaleX = scaleX / cSprite.mWidth;
        cSprite.mScaleY = scaleY / cSprite.mHeight;

        // Add the origin to the sprite
        cSprite.mOrigin.x = (float)(cSprite.mWidth * 0.5f);
        cSprite.mOrigin.y = (float)(cSprite.mHeight * 0.5f);

        // Sprite Screen Position
        cSprite.SetPosition(PositionX, PositionY);

        // Set the Sprite to enabled
        cSprite.mEnabled = enabled;

        cSprite.mRectangle.center  = XMVectorSet(PositionX, PositionY, 0.0f, 1.0f);
        cSprite.mRectangle.extents = 0.5f * XMVectorSet(scaleX, scaleY, 0.0f, 1.0f);
        // Push back to the vector
        m_AllSprites[category].emplace_back(cSprite);

        // Reset everything that needs to be
        Texture.Reset();
        resource.Reset();

        return (int)(m_AllSprites[category].size() - 1);

        // m_AllSprites[category][index].desiredAlpha = 1.0f;
}

// Adds text to the vector of text
void UIManager::AddText(ID3D11Device*        device,
                        ID3D11DeviceContext* deviceContext,
                        int                  category,
                        int                  fontType,
                        std::string          TextDisplay,
                        float                scaleX,
                        float                scaleY,
                        float                PositionX,
                        float                PositionY,
                        bool                 enabled,
                        bool                 AddButton,
                        bool                 bOverrideButtonDimensions,
                        float                buttonwidth,
                        float                buttonheight)
{
        FontComponent cFont;

        cFont.mFontType    = fontType;
        cFont.mTextDisplay = TextDisplay;

        // Set the Main Menu text to enabled
        cFont.mEnabled = enabled;

        // Create Dimensions
        XMFLOAT2 dimensions;
        XMStoreFloat2(&dimensions, instance->m_FontTypes[fontType]->MeasureString(TextDisplay.c_str()));
        float aspectRatio = dimensions.x / dimensions.y;

        cFont.mOrigin = XMVectorSet(dimensions.x * 0.5f, dimensions.y * 0.5f, 0.0f, 1.0f);
        cFont.mScaleX = aspectRatio * scaleX * 1.0f / (dimensions.x);
        cFont.mScaleY = scaleY * 1.0f / (dimensions.y);
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

        scaleX *= aspectRatio;
        scaleY = scaleY;

        if (bOverrideButtonDimensions)
        {
                scaleX = buttonwidth;
                scaleY = buttonheight;
        }
        if (AddButton)
        {
                instance->AddSprite(device,
                                    deviceContext,
                                    category,
                                    L"../Assets/2d/Sprite/Grey Box Test.dds",
                                    PositionX,
                                    PositionY,
                                    scaleX,
                                    scaleY,
                                    enabled);
        }
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

void UIManager::DrawSprites(float deltaTime)
{
        for (auto& it : instance->m_AllSprites)
                for (auto& sprite : it.second)
                {
                        if (sprite.mEnabled)
                        {
                                instance->m_SpriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred,
                                                               instance->m_States->NonPremultiplied());

                                XMVECTOR position = XMVectorSet(sprite.mScreenOffset.x * instance->m_ScreenSize.x / 2.0f,
                                                                sprite.mScreenOffset.y * instance->m_ScreenSize.y / 2.0f,
                                                                0.0f,
                                                                0.0f) +
                                                    instance->m_ScreenCenter;

                                XMVECTOR scale = XMVectorSet(sprite.mScaleX * instance->m_ScreenSize.x / 2.0f,
                                                             sprite.mScaleY * instance->m_ScreenSize.y / 2.0f,
                                                             0.0f,
                                                             1.0f);


                                sprite.currColor = MathLibrary::MoveVectorColorTowards(
                                    sprite.currColor, sprite.desiredColor, deltaTime * 1.0f);


                                instance->m_SpriteBatch->Draw(
                                    sprite.mTexture, position, nullptr, sprite.currColor, 0.0f, sprite.mOrigin, scale);


                                instance->m_SpriteBatch->End();
                        }
                }
}

void UIManager::Present()
{
        m_RenderSystem->Present();
}

std::pair<unsigned int, unsigned int> UIManager::GetHighestSupportedResolution()
{
        DXGI_MODE_DESC desc = instance->resDescriptors.back();
        return std::make_pair((unsigned int)desc.Width, (unsigned int)desc.Height);
}

void UIManager::CatchWinProcSetFullscreen()
{
        if (m_RenderSystem)
        {
                if (m_RenderSystem->m_Swapchain)
                {
                        BOOL fullscreen;
                        m_RenderSystem->GetSwapChain()->GetFullscreenState(&fullscreen, nullptr);
                        if ((instance->CSettings.m_IsFullscreen != (bool)fullscreen) && fullscreen == 0)
                        {
                                instance->CSettings.m_IsFullscreen = false;
                                instance->PSettings.m_IsFullscreen = false;
                                SetFullscreen(false);

                                if (instance->m_AllSprites[E_MENU_CATEGORIES::OptionsSubmenu][0].mEnabled == true)
                                {
                                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][0].mEnabled = false; // Off
                                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][1].mEnabled = true;  // On
                                }
                        }
                }
        }
}

void UIManager::SetFullscreen(bool val)
{
        instance->m_RenderSystem->SetFullscreen(val);
        // instance->StartupResAdjust((HWND)m_WindowHandle);
        // instance->UpdateResolutionText();
        instance->m_RenderSystem->InitDrawOnBackBufferPipeline();
}

void UIManager::UpdateResolutionText()
{
        for (int i = (int)(instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu].size() - instance->resDescriptors.size());
             i < instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu].size();
             i++)
        {
                instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][i].mEnabled = false;
        }
        instance
            ->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu]
                        [instance->CSettings.m_Resolution + instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu].size() -
                         instance->resDescriptors.size()]
            .mEnabled = true;
}

void UIManager::GameplayUpdate(float deltaTime)
{
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

        if (instance->m_AllFonts[E_MENU_CATEGORIES::MainMenu][0].mEnabled == true)
        {
                if (InputActions::EnterAction() == KeyState::DownFirst)
                {
                        instance->MainTitleUnpause();
                }
        }

        else
        {
                if (InputActions::PauseAction() == KeyState::DownFirst)
                {
                        // Left Click Image
                        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][0].mEnabled = false;
                        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][4].mEnabled = false;

                        for (int i = 0; i < instance->m_AllFonts[E_MENU_CATEGORIES::Demo].size(); i++)
                        {
                                instance->m_AllFonts[E_MENU_CATEGORIES::Demo][i].mEnabled = false;
                        }
                        for (int i = 0; i < instance->m_AllSprites[E_MENU_CATEGORIES::Demo].size(); i++)
                        {
                                instance->m_AllFonts[E_MENU_CATEGORIES::Demo][i].mEnabled = false;
                        }

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

        // Left Click
        if (GCoreInput::GetMouseState(MouseCode::LeftClick) == KeyState::Down)
        {
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][0].desiredColor = {1, 1, 1, 0};
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][4].desiredColor = {1, 1, 1, 0};
        }
        else if (TutorialLevel::currPhase == TutorialLevel::E_TUTORIAL_PHASE::PHASE_1)
        {
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][0].desiredColor = {1, 1, 1, 1};
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][4].desiredColor = {1, 1, 1, 1};
        }

        if (GCoreInput::GetKeyState(KeyCode::A) == KeyState::Down)
        {
                auto [r, g, b]                                                      = RedIconColor;
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][1].desiredColor = {r, g, b, 0};
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][5].desiredColor = {r, g, b, 0};
        }
        else if (TutorialLevel::currPhase == TutorialLevel::E_TUTORIAL_PHASE::PHASE_2)
        {
                auto [r, g, b]                                                      = RedIconColor;
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][1].desiredColor = {r, g, b, 1};
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][5].desiredColor = {r, g, b, 1};
        }

        if (GCoreInput::GetKeyState(KeyCode::S) == KeyState::Down)
        {
                auto [r, g, b]                                                      = GreenIconColor;
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][2].desiredColor = {r, g, b, 0};
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][6].desiredColor = {r, g, b, 0};
        }
        else if (TutorialLevel::currPhase == TutorialLevel::E_TUTORIAL_PHASE::PHASE_3)
        {
                auto [r, g, b]                                                      = GreenIconColor;
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][2].desiredColor = {r, g, b, 1};
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][6].desiredColor = {r, g, b, 1};
        }

        if (GCoreInput::GetKeyState(KeyCode::D) == KeyState::Down)
        {
                auto [r, g, b]                                                      = BlueIconColor;
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][3].desiredColor = {r, g, b, 0};
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][7].desiredColor = {r, g, b, 0};
        }
        else if (TutorialLevel::currPhase == TutorialLevel::E_TUTORIAL_PHASE::PHASE_4)
        {
                auto [r, g, b]                                                      = BlueIconColor;
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][3].desiredColor = {r, g, b, 1};
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][7].desiredColor = {r, g, b, 1};
        }

        /*BEGIN CONTROLLER REFACTORING*/

        // Original code below
        /*
         UIMouseEvent e;
         e.mouseX = (float)GCoreInput::GetMouseWindowPosX();
         e.mouseY = (float)GCoreInput::GetMouseWindowPosY();
         std::vector<SpriteComponent*> clickedSprites;

         RECT rect;
         GetWindowRect((HWND)(instance->m_RenderSystem->m_WindowHandle), &rect);
         XMUINT4 xmRect = XMUINT4(rect.top, rect.left, rect.bottom, rect.right);

         POINT cursorPoint;
         GetCursorPos(&cursorPoint);
         XMFLOAT2 cursorCoords = {(float)cursorPoint.x, (float)cursorPoint.y};
         XMVECTOR point        = UI::ConvertScreenPosToNDC(cursorCoords, instance->m_ScreenSize, xmRect);
         */

        UIMouseEvent e;

        RECT rect;
        GetWindowRect((HWND)(instance->m_RenderSystem->m_WindowHandle), &rect);
        XMUINT4 xmRect = XMUINT4(rect.top, rect.left, rect.bottom, rect.right);

        POINT cursorPoint;
        GetCursorPos(&cursorPoint);

        if (GamePad::Get()->CheckConnection() == true)
        {
                for (int i = 0; i < instance->resDescriptors.size(); i++)
                {
                        int XPos = int(GamePad::Get()->leftStickX * 10.0f);
                        int YPos = int(GamePad::Get()->leftStickY * 10.0f);

                        SetCursorPos(cursorPoint.x + XPos, cursorPoint.y - YPos);
                }
        }

        GetCursorPos(&cursorPoint);
        e.mouseX = (float)cursorPoint.x;
        e.mouseY = (float)cursorPoint.y;

        std::vector<SpriteComponent*> clickedSprites;
        XMFLOAT2                      cursorCoords = {(float)cursorPoint.x, (float)cursorPoint.y};
        XMVECTOR                      point        = UI::ConvertScreenPosToNDC(cursorCoords, instance->m_ScreenSize, xmRect);

        instance->DrawSprites(deltaTime);

        for (auto& it : instance->m_AllSprites)
                for (auto& sprite : it.second)
                {
                        if (sprite.mEnabled)
                        {
                                if (InputActions::MouseClickAction() == KeyState::Release)
                                {

                                        if (UI::PointInRect(sprite.mRectangle, point))
                                        {
                                                // Button Was Pressed
                                                clickedSprites.push_back(&sprite);
                                        }
                                }
                        }
                }

        for (auto& sprite : clickedSprites)
        {
                e.sprite = sprite;
                sprite->OnMouseDown.Invoke(&e);
        }
}

// Splash Screen
void UIManager::Splash_FullSail()
{
        instance->m_AllSprites[E_MENU_CATEGORIES::SplashScreen][0].mEnabled  = true;
        instance->m_AllSprites[E_MENU_CATEGORIES::SplashScreen][0].currColor = {1, 1, 1, .0f};
        // instance->m_AllSprites[E_MENU_CATEGORIES::SplashScreen][1].mEnabled  = false;
        // instance->m_AllSprites[E_MENU_CATEGORIES::SplashScreen][2].mEnabled  = false;
}

void UIManager::Splash_GPGames()
{
        instance->m_AllSprites[E_MENU_CATEGORIES::SplashScreen][1].mEnabled  = true;
        instance->m_AllSprites[E_MENU_CATEGORIES::SplashScreen][1].currColor = {1, 1, 1, .0f};
        // instance->m_AllSprites[E_MENU_CATEGORIES::SplashScreen][0].mEnabled  = false;
        // instance->m_AllSprites[E_MENU_CATEGORIES::SplashScreen][2].mEnabled  = false;
}

void UIManager::Splash_Team()
{
        instance->m_AllSprites[E_MENU_CATEGORIES::SplashScreen][2].mEnabled  = true;
        instance->m_AllSprites[E_MENU_CATEGORIES::SplashScreen][2].currColor = {1, 1, 1, .0f};
        // instance->m_AllSprites[E_MENU_CATEGORIES::SplashScreen][0].mEnabled  = false;
        // instance->m_AllSprites[E_MENU_CATEGORIES::SplashScreen][1].mEnabled  = false;
}

void UIManager::SplashUpdate(float globalTimer, float deltaTime)
{
        if (GCoreInput::GetKeyState(KeyCode::Esc) == KeyState::DownFirst)
        {
                instance->m_BreakSplash = true;
                UIManager::instance->Splash_End();
                timed_functions.clear();
        }
        // else
        //{
        //        if (globalTimer < 5.0f)
        //        {
        //                static auto dbg_test = 0;
        //                dbg_test++;
        //                // Full Sail Logo
        //                UIManager::instance->Splash_FullSail();
        //        }
        //        else if (globalTimer >= 5.0f && globalTimer < 10.0f)
        //        {
        //                // GP Games Logo
        //                UIManager::instance->Splash_GPGames();
        //        }
        //        else if (globalTimer >= 10.0f && globalTimer < 15.0f)
        //        {
        //                // Deep!deep Logo
        //                UIManager::instance->Splash_Team();
        //        }
        //        if (globalTimer >= 15.0f)
        //        {
        //                UIManager::instance->Splash_End();
        //        }
        //}
}

void UIManager::Splash_End()
{
        instance->m_AllSprites[E_MENU_CATEGORIES::SplashScreen][0].mEnabled = false;
        instance->m_AllSprites[E_MENU_CATEGORIES::SplashScreen][1].mEnabled = false;
        instance->m_AllSprites[E_MENU_CATEGORIES::SplashScreen][2].mEnabled = false;


        instance->m_AllFonts[E_MENU_CATEGORIES::MainMenu][0].mEnabled = true;

        if (GamePad::Get()->CheckConnection() == true)
        {
                instance->m_AllFonts[E_MENU_CATEGORIES::MainMenu][2].mEnabled = true;
        }

        else
        {
                instance->m_AllFonts[E_MENU_CATEGORIES::MainMenu][1].mEnabled = true;
        }
        instance->m_AllFonts[E_MENU_CATEGORIES::MainMenu][0].currColor = {1, 1, 1, 0};
        instance->m_AllFonts[E_MENU_CATEGORIES::MainMenu][1].currColor = {1, 1, 1, 0};
}


// UI Transitions
void UIManager::WhiteOrbCollected()
{
        // If the controller is connected, it will turn on the Controller UI element
        // Otherwise it will turn on the keyboard UI element
        // It will always disable both UI elements in case the controller is unplugged
        m_currentTutorialIcon = {1, 5};

        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][0].desiredColor = {1, 1, 1, 0};
        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][4].desiredColor = {1, 1, 1, 0};

        TimedFunction disableIndicatorDelayed;
        disableIndicatorDelayed.delay = .3f;
        disableIndicatorDelayed.func  = []() {
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][0].mEnabled = false;
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][4].mEnabled = false;

                auto [redR, redG, redB] = RedIconColor;
                if (GamePad::Get()->CheckConnection() == true)
                {
                        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][5].mEnabled  = true;
                        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][5].currColor = {redR, redG, redB, 0.0f};
                }
                else
                {
                        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][1].mEnabled  = true;
                        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][1].currColor = {redR, redG, redB, .0f};
                }
        };
        instance->timed_functions.push_back(disableIndicatorDelayed);
}

void UIManager::RedOrbCollected()
{
        // instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][1].desiredColor = {1, 1, 1, 0};
        // instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][5].desiredColor = {1, 1, 1, 0};
        // if (GamePad::Get()->CheckConnection() == true)
        //{
        //        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][6].mEnabled  = true;
        //        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][6].currColor = {1, 1, 1, .0f};
        //}
        // else
        //{
        //        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][2].mEnabled  = true;
        //        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][2].currColor = {1, 1, 1, .0f};
        //}
        m_currentTutorialIcon = {2, 6};

        TimedFunction disableIndicatorDelayed;
        disableIndicatorDelayed.delay = .3f;
        disableIndicatorDelayed.func  = []() {
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][1].mEnabled = false;
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][5].mEnabled = false;
                auto [greenR, greenG, greenB]                                   = GreenIconColor;
                if (GamePad::Get()->CheckConnection() == true)
                {
                        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][6].mEnabled  = true;
                        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][6].currColor = {greenR, greenG, greenB, .0f};
                }
                else
                {
                        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][2].mEnabled  = true;
                        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][2].currColor = {greenR, greenG, greenB, .0f};
                }
        };
        instance->timed_functions.push_back(disableIndicatorDelayed);
}

void UIManager::GreenOrbCollected()
{
        /*       instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][2].desiredColor = {1, 1, 1, 0};
               instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][6].desiredColor = {1, 1, 1, 0};
               if (GamePad::Get()->CheckConnection() == true)
               {
                       instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][7].mEnabled  = true;
                       instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][7].currColor = {1, 1, 1, .0f};
               }
               else
               {
                       instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][3].mEnabled  = true;
                       instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][3].currColor = {1, 1, 1, .0f};
               }
       */
        m_currentTutorialIcon = {3, 7};

        TimedFunction disableIndicatorDelayed;
        disableIndicatorDelayed.delay = .3f;
        disableIndicatorDelayed.func  = []() {
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][2].mEnabled = false;
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][6].mEnabled = false;

                auto [blueR, blueG, blueB] = BlueIconColor;
                if (GamePad::Get()->CheckConnection() == true)
                {
                        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][7].mEnabled  = true;
                        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][7].currColor = {blueR, blueG, blueB, 0.0f};
                }
                else
                {
                        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][3].mEnabled  = true;
                        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][3].currColor = {blueR, blueG, blueB, .0f};
                }
        };
        instance->timed_functions.push_back(disableIndicatorDelayed);
}

void UIManager::BlueOrbCollected()
{
        m_currentTutorialIcon = {-1, -1};

        // Only needs to turn off the elements. There are none to turn on
        auto [r, g, b]                                                      = BlueIconColor;
        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][3].desiredColor = {r, g, b, 0};
        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][7].desiredColor = {r, g, b, 0};
}


// Pausing and Unpausing
void UIManager::MainTitleUnpause()
{
        GEngine::Get()->SetGamePaused(false);
        for (int i = 0; i < instance->m_AllFonts[E_MENU_CATEGORIES::MainMenu].size(); i++)
        {
                instance->m_AllFonts[E_MENU_CATEGORIES::MainMenu][i].mEnabled = false;
        }

        m_currentTutorialIcon = {0, 4};
        // Left Click Image
        if (GamePad::Get()->CheckConnection() == true)
        {

                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][4].mEnabled  = true;
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][4].currColor = {1, 1, 1, .0f};
        }
        else
        {
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][0].mEnabled  = true;
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][0].currColor = {1, 1, 1, .0f};
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
        for (auto& itr : instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu])
        {
                itr.mEnabled = false;
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

        auto [currIconMouse, currIconController] = m_currentTutorialIcon;

        if (currIconMouse != -1)
        {
                auto spriteColor = instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][currIconController].currColor;
                if (GamePad::Get()->CheckConnection() == true)
                {

                        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][currIconController].mEnabled  = true;
                        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][currIconController].currColor = {
                            spriteColor.m128_f32[0], spriteColor.m128_f32[1], spriteColor.m128_f32[2], 0};
                        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][currIconController].desiredColor = {
                            spriteColor.m128_f32[0], spriteColor.m128_f32[1], spriteColor.m128_f32[2], 0};
                }
                else
                {
                        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][currIconMouse].mEnabled  = true;
                        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][currIconMouse].currColor = {
                            spriteColor.m128_f32[0], spriteColor.m128_f32[1], spriteColor.m128_f32[2], 0};
                        instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][currIconMouse].desiredColor = {
                            spriteColor.m128_f32[0], spriteColor.m128_f32[1], spriteColor.m128_f32[2], 0};
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
        // if (instance->m_AdjustedScreen == false)
        {
                instance->m_AdjustedScreen = true;
                instance->m_window         = window;

                // Resizes the window
                SupportedResolutions();
                DXGI_MODE_DESC desc              = instance->resDescriptors.back();
                instance->CSettings.m_Resolution = (int)(instance->resDescriptors.size() - 1);
                instance->PSettings              = instance->CSettings;
                AdjustResolution(window, desc.Width, desc.Height);
        }
}

void UIManager::AdjustResolution(HWND window, int wWidth, int wHeight)
{
        if (instance->CSettings.m_IsFullscreen == true)
        {
                // instance->instance->m_RenderSystem->OnWindowResize(wWidth, wHeight, true);
                m_RenderSystem->OnWindowResize(wWidth, wHeight, true);
                m_RenderSystem->InitDrawOnBackBufferPipeline();

                return;
        }

        RECT wr = {0, 0, wWidth, wHeight};                 // set the size
        AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE); // adjust the size

        DXGI_MODE_DESC& desc = instance->resDescriptors.back();

        int posX = GetSystemMetrics(SM_CXSCREEN) / 2 - (wWidth) / 2;
        int posY = GetSystemMetrics(SM_CYSCREEN) / 2 - (wHeight) / 2;

        ::SetWindowPos(window, 0, posX, posY, wr.right - wr.left, wr.bottom - wr.top, WS_OVERLAPPEDWINDOW);
        MoveWindow(window, posX, posY, wWidth, wHeight, true);

        m_RenderSystem->OnWindowResize(wWidth, wHeight, false);
        m_RenderSystem->InitDrawOnBackBufferPipeline();
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

        std::unordered_set<std::pair<UINT, UINT>, hash_pair> checkedResolutions;

        size_t size = instance->resDescriptors.size();
        for (int i = (int)size - 1; i >= 0; i--)
        {
                std::pair<UINT, UINT> newRes =
                    std::make_pair(instance->resDescriptors[i].Width, instance->resDescriptors[i].Height);
                if (checkedResolutions.find(newRes) != checkedResolutions.end() || newRes.first < 640)
                {
                        instance->resDescriptors.erase(instance->resDescriptors.begin() + i);
                }
                else
                {
                        checkedResolutions.insert(newRes);
                }
        }
}

void UIManager::DemoEnd()
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

        for (int i = 0; i < instance->m_AllFonts[E_MENU_CATEGORIES::Demo].size(); i++)
        {
                instance->m_AllFonts[E_MENU_CATEGORIES::Demo][i].mEnabled = true;
        }
        for (int i = 0; i < instance->m_AllSprites[E_MENU_CATEGORIES::Demo].size(); i++)
        {
                instance->m_AllSprites[E_MENU_CATEGORIES::Demo][i].mEnabled = true;
        }
}


// Core Function
void UIManager::Initialize(native_handle_type hwnd)
{
        assert(!instance);
        instance = DBG_NEW UIManager;

        instance->m_RenderSystem = GEngine::Get()->GetSystemManager()->GetSystem<RenderSystem>();

        instance->m_ScreenSize =
            XMFLOAT2{instance->m_RenderSystem->m_BackBufferWidth, instance->m_RenderSystem->m_BackBufferHeight};
        instance->m_ScreenCenter = XMVectorSet(instance->m_ScreenSize.x * 0.5f, instance->m_ScreenSize.y * 0.5f, 0.0f, 1.0f);
        instance->m_TexelSize    = XMFLOAT2{1.0f / instance->m_ScreenSize.x, 1.0f / instance->m_ScreenSize.y};

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
        instance->m_BreakSplash  = false;

        // Create supported resolutions
        instance->SupportedResolutions();
        const float ScaleXRatio = 1.2f;
        const float ScaleYRatio = 2.0f;

        const float PosXRatio = 1.2f;
        const float PosYRatio = 2.0f;

        constexpr float pauseButtonWidth  = 0.25f;
        constexpr float pauseButtonHeight = 0.05f;

        instance->m_FontTypes[E_FONT_TYPE::Angel] =
            new DirectX::SpriteFont(instance->m_RenderSystem->m_Device, L"../Assets/2d/Text/angel.spritefont");
        instance->m_FontTypes[E_FONT_TYPE::Calibri] =
            new DirectX::SpriteFont(instance->m_RenderSystem->m_Device, L"../Assets/2d/Text/calibri.spritefont");
        instance->m_FontTypes[E_FONT_TYPE::CourierNew] =
            new DirectX::SpriteFont(instance->m_RenderSystem->m_Device, L"../Assets/2d/Text/couriernew.spritefont");

        // Splash Screen
        {
                instance->AddSprite(instance->m_RenderSystem->m_Device,
                                    instance->m_RenderSystem->m_Context,
                                    E_MENU_CATEGORIES::SplashScreen,
                                    L"../Assets/2d/Sprite/Full_Sail_Logo.dds",
                                    0.0f,
                                    0.0f,
                                    2.0f,
                                    2.0f,
                                    false);

                instance->AddSprite(instance->m_RenderSystem->m_Device,
                                    instance->m_RenderSystem->m_Context,
                                    E_MENU_CATEGORIES::SplashScreen,
                                    L"../Assets/2d/Sprite/GPGlogo_solid.dds",
                                    0.0f,
                                    0.0f,
                                    2.0f,
                                    2.0f,
                                    false);

                instance->AddSprite(instance->m_RenderSystem->m_Device,
                                    instance->m_RenderSystem->m_Context,
                                    E_MENU_CATEGORIES::SplashScreen,
                                    L"../Assets/2d/Sprite/Deep!deep_Logo.dds",
                                    0.0f,
                                    0.0f,
                                    2.0f,
                                    2.0f,
                                    false);
        }
        // Main Menu
        {
                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::MainMenu,
                                  E_FONT_TYPE::Angel,
                                  "INANIS",
                                  0.06f * ScaleXRatio,
                                  0.06f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  -0.25f * PosYRatio,
                                  false,
                                  false);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::MainMenu,
                                  E_FONT_TYPE::Calibri,
                                  "PRESS ENTER TO CONTINUE...",
                                  0.05f * ScaleXRatio,
                                  0.05f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  0.1f * PosYRatio,
                                  false,
                                  false);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::MainMenu,
                                  E_FONT_TYPE::CourierNew,
                                  "Press The Back Button To Continue...",
                                  0.05f * ScaleXRatio,
                                  0.05f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  0.1f * PosYRatio,
                                  false,
                                  false);

                instance->AddSprite(instance->m_RenderSystem->m_Device,
                                    instance->m_RenderSystem->m_Context,
                                    E_MENU_CATEGORIES::MainMenu,
                                    L"../Assets/2d/Sprite/Mouse_Key.dds",
                                    0.0f * PosXRatio,
                                    0.3f * PosYRatio,
                                    0.1f * ScaleXRatio,
                                    0.1f * ScaleYRatio,
                                    false);

                instance->AddSprite(instance->m_RenderSystem->m_Device,
                                    instance->m_RenderSystem->m_Context,
                                    E_MENU_CATEGORIES::MainMenu,
                                    L"../Assets/2d/Sprite/A_Key.dds",
                                    0.0f * PosXRatio,
                                    0.3f * PosYRatio,
                                    0.2f * ScaleXRatio,
                                    0.2f * ScaleYRatio,
                                    false);

                auto [redR, redG, redB] = RedIconColor;

                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][1].currColor    = {redR, redG, redB, 1};
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][1].desiredColor = {redR, redG, redB, 1};

                instance->AddSprite(instance->m_RenderSystem->m_Device,
                                    instance->m_RenderSystem->m_Context,
                                    E_MENU_CATEGORIES::MainMenu,
                                    L"../Assets/2d/Sprite/S_Key.dds",
                                    0.0f * PosXRatio,
                                    0.3f * PosYRatio,
                                    0.2f * ScaleXRatio,
                                    0.2f * ScaleYRatio,
                                    false);

                auto [greenR, greenG, greenB] = GreenIconColor;

                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][2].currColor    = {greenR, greenG, greenB, 1};
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][2].desiredColor = {greenR, greenG, greenB, 1};

                instance->AddSprite(instance->m_RenderSystem->m_Device,
                                    instance->m_RenderSystem->m_Context,
                                    E_MENU_CATEGORIES::MainMenu,
                                    L"../Assets/2d/Sprite/D_Key.dds",
                                    0.0f * PosXRatio,
                                    0.3f * PosYRatio,
                                    0.2f * ScaleXRatio,
                                    0.2f * ScaleYRatio,
                                    false);

                auto [blueR, blueG, blueB] = GreenIconColor;

                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][3].currColor    = {blueR, blueG, blueB, 1};
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][3].desiredColor = {blueR, blueG, blueB, 1};

                // Controller Icons
                instance->AddSprite(instance->m_RenderSystem->m_Device,
                                    instance->m_RenderSystem->m_Context,
                                    E_MENU_CATEGORIES::MainMenu,
                                    L"../Assets/2d/Sprite/RightTrigger.dds",
                                    0.0f * PosXRatio,
                                    0.3f * PosYRatio,
                                    0.1f * ScaleXRatio,
                                    0.1f * ScaleYRatio,
                                    false);

                instance->AddSprite(instance->m_RenderSystem->m_Device,
                                    instance->m_RenderSystem->m_Context,
                                    E_MENU_CATEGORIES::MainMenu,
                                    L"../Assets/2d/Sprite/B_Button.dds",
                                    0.0f * PosXRatio,
                                    0.3f * PosYRatio,
                                    0.2f * ScaleXRatio,
                                    0.2f * ScaleYRatio,
                                    false);
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][5].currColor    = {redR, redG, redB, 1};
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][5].desiredColor = {redR, redG, redB, 1};

                instance->AddSprite(instance->m_RenderSystem->m_Device,
                                    instance->m_RenderSystem->m_Context,
                                    E_MENU_CATEGORIES::MainMenu,
                                    L"../Assets/2d/Sprite/A_key.dds",
                                    0.0f * PosXRatio,
                                    0.3f * PosYRatio,
                                    0.2f * ScaleXRatio,
                                    0.2f * ScaleYRatio,
                                    false);
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][6].currColor    = {greenR, greenG, greenB, 1};
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][6].desiredColor = {greenR, greenG, greenB, 1};

                instance->AddSprite(instance->m_RenderSystem->m_Device,
                                    instance->m_RenderSystem->m_Context,
                                    E_MENU_CATEGORIES::MainMenu,
                                    L"../Assets/2d/Sprite/X_Button.dds",
                                    0.0f * PosXRatio,
                                    0.3f * PosYRatio,
                                    0.2f * ScaleXRatio,
                                    0.2f * ScaleYRatio,
                                    false);
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][7].currColor    = {blueR, blueG, blueB, 1};
                instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu][7].desiredColor = {blueR, blueG, blueB, 1};
        }
        // Pause Menu
        {
                instance->AddSprite(instance->m_RenderSystem->m_Device,
                                    instance->m_RenderSystem->m_Context,
                                    E_MENU_CATEGORIES::PauseMenu,
                                    L"../Assets/2d/Sprite/Grey Box Test.dds",
                                    0.0f * PosXRatio,
                                    0.0f * PosYRatio,
                                    0.4f * ScaleXRatio,
                                    0.6f * ScaleYRatio,
                                    false);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::PauseMenu,
                                  E_FONT_TYPE::Angel,
                                  "INANIS",
                                  0.06f * ScaleXRatio,
                                  0.06f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  -0.25f * PosYRatio,
                                  false,
                                  false);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::PauseMenu,
                                  E_FONT_TYPE::Calibri,
                                  "RESUME",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  -0.13f * PosYRatio,
                                  false,
                                  true,
                                  true,
                                  pauseButtonWidth * ScaleXRatio,
                                  pauseButtonHeight * ScaleYRatio);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::PauseMenu,
                                  E_FONT_TYPE::Calibri,
                                  "LEVELS",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  -0.06f * PosYRatio,
                                  false,
                                  true,
                                  true,
                                  pauseButtonWidth * ScaleXRatio,
                                  pauseButtonHeight * ScaleYRatio);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::PauseMenu,
                                  E_FONT_TYPE::Calibri,
                                  "OPTIONS",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  0.01f * PosYRatio,
                                  false,
                                  true,
                                  true,
                                  pauseButtonWidth * ScaleXRatio,
                                  pauseButtonHeight * ScaleYRatio);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::PauseMenu,
                                  E_FONT_TYPE::Calibri,
                                  "CONTROLS",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  0.08f * PosYRatio,
                                  false,
                                  true,
                                  true,
                                  pauseButtonWidth * ScaleXRatio,
                                  pauseButtonHeight * ScaleYRatio);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::PauseMenu,
                                  E_FONT_TYPE::Calibri,
                                  "EXIT",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  0.15f * PosYRatio,
                                  false,
                                  true,
                                  true,
                                  pauseButtonWidth * ScaleXRatio,
                                  pauseButtonHeight * ScaleYRatio);
        }
        // Options Menu
        {
                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::OptionsMenu,
                                  E_FONT_TYPE::Calibri,
                                  "BACK",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  -0.20f * PosYRatio,
                                  false,
                                  true,
                                  true,
                                  pauseButtonWidth * ScaleXRatio,
                                  pauseButtonHeight * ScaleYRatio);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::OptionsMenu,
                                  E_FONT_TYPE::Calibri,
                                  "WINDOWED",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  0.07f * PosYRatio,
                                  false,
                                  true,
                                  true,
                                  pauseButtonWidth * ScaleXRatio,
                                  pauseButtonHeight * ScaleYRatio);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::OptionsMenu,
                                  E_FONT_TYPE::Calibri,
                                  "RESOLUTION",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  0.17f * PosYRatio,
                                  false,
                                  true,
                                  true,
                                  pauseButtonWidth * ScaleXRatio,
                                  pauseButtonHeight * ScaleYRatio);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::OptionsMenu,
                                  E_FONT_TYPE::Calibri,
                                  "APPLY",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  -0.26f * PosYRatio,
                                  false,
                                  true,
                                  true,
                                  pauseButtonWidth * ScaleXRatio,
                                  pauseButtonHeight * ScaleYRatio);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::OptionsMenu,
                                  E_FONT_TYPE::Calibri,
                                  "MASTER VOLUME",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  -0.13f * PosYRatio,
                                  false,
                                  true,
                                  true,
                                  pauseButtonWidth * ScaleXRatio,
                                  pauseButtonHeight * ScaleYRatio);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::OptionsMenu,
                                  E_FONT_TYPE::Calibri,
                                  "SENSITIVITY",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  -0.03f * PosYRatio,
                                  false,
                                  true,
                                  true,
                                  pauseButtonWidth * ScaleXRatio,
                                  pauseButtonHeight * ScaleYRatio);
        }
        // Options Submenu
        {
                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::OptionsSubmenu,
                                  E_FONT_TYPE::Calibri,
                                  "OFF",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  0.11f * PosYRatio,
                                  false,
                                  false);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::OptionsSubmenu,
                                  E_FONT_TYPE::Calibri,
                                  "ON",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  0.11f * PosYRatio,
                                  false,
                                  false);
                // Resolutions
                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::OptionsSubmenu,
                                  E_FONT_TYPE::Calibri,
                                  "<",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  -0.12f * PosXRatio,
                                  0.22f * PosYRatio,
                                  false,
                                  true);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::OptionsSubmenu,
                                  E_FONT_TYPE::Calibri,
                                  ">",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.12f * PosXRatio,
                                  0.22f * PosYRatio,
                                  false,
                                  true);
                // Sensitivity
                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::OptionsSubmenu,
                                  E_FONT_TYPE::Calibri,
                                  "<",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  -0.12f * PosXRatio,
                                  0.02f * PosYRatio,
                                  false,
                                  true);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::OptionsSubmenu,
                                  E_FONT_TYPE::Calibri,
                                  ">",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.12f * PosXRatio,
                                  0.02f * PosYRatio,
                                  false,
                                  true);

                // Volume
                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::OptionsSubmenu,
                                  E_FONT_TYPE::Calibri,
                                  "<",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  -0.12f * PosXRatio,
                                  -0.08f * PosYRatio,
                                  false,
                                  true);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::OptionsSubmenu,
                                  E_FONT_TYPE::Calibri,
                                  ">",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.12f * PosXRatio,
                                  -0.08f * PosYRatio,
                                  false,
                                  true);

                // Text for Sensitivity
                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::OptionsSubmenu,
                                  E_FONT_TYPE::Calibri,
                                  "Low",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  0.02f * PosYRatio,
                                  false,
                                  false);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::OptionsSubmenu,
                                  E_FONT_TYPE::Calibri,
                                  "Medium",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  0.02f * PosYRatio,
                                  false,
                                  false);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::OptionsSubmenu,
                                  E_FONT_TYPE::Calibri,
                                  "High",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  0.02f * PosYRatio,
                                  false,
                                  false);

                // Volumes
                for (auto i = 0; i <= 100; i += 10)
                {
                        instance->AddText(instance->m_RenderSystem->m_Device,
                                          instance->m_RenderSystem->m_Context,
                                          E_MENU_CATEGORIES::OptionsSubmenu,
                                          E_FONT_TYPE::Calibri,
                                          std::to_string(i),
                                          0.035f * ScaleXRatio,
                                          0.035f * ScaleYRatio,
                                          0.0f * PosXRatio,
                                          -0.08f * PosYRatio,
                                          false,
                                          false);
                }


                // Any new options submenu should be put above this
                for (auto i = 0; i < instance->resDescriptors.size(); i++)
                {
                        instance->AddText(instance->m_RenderSystem->m_Device,
                                          instance->m_RenderSystem->m_Context,
                                          E_MENU_CATEGORIES::OptionsSubmenu,
                                          E_FONT_TYPE::Calibri,
                                          std::to_string(instance->resDescriptors[i].Width) + "x" +
                                              std::to_string(instance->resDescriptors[i].Height),
                                          0.035f * ScaleXRatio,
                                          0.035f * ScaleYRatio,
                                          0.0f * PosXRatio,
                                          0.22f * PosYRatio,
                                          false,
                                          false);
                }
        }
        // Level Menu
        {
                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::LevelMenu,
                                  E_FONT_TYPE::Calibri,
                                  "BACK",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  -0.20f * PosYRatio,
                                  false,
                                  true,
                                  true,
                                  pauseButtonWidth * ScaleXRatio,
                                  pauseButtonHeight * ScaleYRatio);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::LevelMenu,
                                  E_FONT_TYPE::Calibri,
                                  "TUTORIAL",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  -0.10f * PosYRatio,
                                  false,
                                  true,
                                  true,
                                  pauseButtonWidth * ScaleXRatio,
                                  pauseButtonHeight * ScaleYRatio);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::LevelMenu,
                                  E_FONT_TYPE::Calibri,
                                  "LEVEL 1",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  -0.03f * PosYRatio,
                                  false,
                                  true,
                                  true,
                                  pauseButtonWidth * ScaleXRatio,
                                  pauseButtonHeight * ScaleYRatio);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::LevelMenu,
                                  E_FONT_TYPE::Calibri,
                                  "LEVEL 2",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  0.04f * PosYRatio,
                                  false,
                                  true,
                                  true,
                                  pauseButtonWidth * ScaleXRatio,
                                  pauseButtonHeight * ScaleYRatio);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::LevelMenu,
                                  E_FONT_TYPE::Calibri,
                                  "LEVEL 3",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  0.11f * PosYRatio,
                                  false,
                                  true,
                                  true,
                                  pauseButtonWidth * ScaleXRatio,
                                  pauseButtonHeight * ScaleYRatio);
        }
        // Controls Menu
        {
                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::ControlsMenu,
                                  E_FONT_TYPE::Calibri,
                                  "BACK",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  -0.20f * PosYRatio,
                                  false,
                                  true,
                                  true,
                                  pauseButtonWidth * ScaleXRatio,
                                  pauseButtonHeight * ScaleYRatio);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::ControlsMenu,
                                  E_FONT_TYPE::Calibri,
                                  "COLLECT RED LIGHT: A",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  -0.1f * PosYRatio,
                                  false,
                                  false);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::ControlsMenu,
                                  E_FONT_TYPE::Calibri,
                                  "COLLECT GREEN LIGHT: S",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  0.0f * PosYRatio,
                                  false,
                                  false);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::ControlsMenu,
                                  E_FONT_TYPE::Calibri,
                                  "COLLECT BLUE LIGHT: D",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  0.1f * PosYRatio,
                                  false,
                                  false);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::ControlsMenu,
                                  E_FONT_TYPE::Calibri,
                                  "MOVEMENT: Left Mouse ",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  0.2f * PosYRatio,
                                  false,
                                  false);
        }
        // Demo
        {
                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::Demo,
                                  E_FONT_TYPE::Calibri,
                                  "Thank you for playing!",
                                  0.05f * ScaleXRatio,
                                  0.05f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  -0.25f * PosYRatio,
                                  false,
                                  false);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::Demo,
                                  E_FONT_TYPE::Calibri,
                                  "CREDITS",
                                  0.05f * ScaleXRatio,
                                  0.05f * ScaleYRatio,
                                  0.0f * PosXRatio,
                                  -0.2f * PosYRatio,
                                  false,
                                  false);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::Demo,
                                  E_FONT_TYPE::Calibri,
                                  "Jose Villarroel:",
                                  0.05f * ScaleXRatio,
                                  0.05f * ScaleYRatio,
                                  -0.3f * PosXRatio,
                                  -0.1f * PosYRatio,
                                  false,
                                  false);
                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::Demo,
                                  E_FONT_TYPE::Calibri,
                                  "Graphics Programmer",
                                  0.05f * ScaleXRatio,
                                  0.05f * ScaleYRatio,
                                  0.3f * PosXRatio,
                                  -0.1f * PosYRatio,
                                  false,
                                  false);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::Demo,
                                  E_FONT_TYPE::Calibri,
                                  "Liam Murray:",
                                  0.05f * ScaleXRatio,
                                  0.05f * ScaleYRatio,
                                  -0.3f * PosXRatio,
                                  -0.05f * PosYRatio,
                                  false,
                                  false);
                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::Demo,
                                  E_FONT_TYPE::Calibri,
                                  "Engine Programmer",
                                  0.05f * ScaleXRatio,
                                  0.05f * ScaleYRatio,
                                  0.3f * PosXRatio,
                                  -0.05f * PosYRatio,
                                  false,
                                  false);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::Demo,
                                  E_FONT_TYPE::Calibri,
                                  "Joseph Gill:",
                                  0.05f * ScaleXRatio,
                                  0.05f * ScaleYRatio,
                                  -0.3f * PosXRatio,
                                  0.0f * PosYRatio,
                                  false,
                                  false);
                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::Demo,
                                  E_FONT_TYPE::Calibri,
                                  "Gameplay Programmer",
                                  0.05f * ScaleXRatio,
                                  0.05f * ScaleYRatio,
                                  0.3f * PosXRatio,
                                  0.0f * PosYRatio,
                                  false,
                                  false);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::Demo,
                                  E_FONT_TYPE::Calibri,
                                  "Victoria Kiang:",
                                  0.05f * ScaleXRatio,
                                  0.05f * ScaleYRatio,
                                  -0.3f * PosXRatio,
                                  0.05f * PosYRatio,
                                  false,
                                  false);
                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::Demo,
                                  E_FONT_TYPE::Calibri,
                                  "Math and Collision Engineer",
                                  0.05f * ScaleXRatio,
                                  0.05f * ScaleYRatio,
                                  0.3f * PosXRatio,
                                  0.05f * PosYRatio,
                                  false,
                                  false);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::Demo,
                                  E_FONT_TYPE::Calibri,
                                  "Nolan Bys:",
                                  0.05f * ScaleXRatio,
                                  0.05f * ScaleYRatio,
                                  -0.3f * PosXRatio,
                                  0.1f * PosYRatio,
                                  false,
                                  false);
                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::Demo,
                                  E_FONT_TYPE::Calibri,
                                  "User Interface Programmer",
                                  0.05f * ScaleXRatio,
                                  0.05f * ScaleYRatio,
                                  0.3f * PosXRatio,
                                  0.1f * PosYRatio,
                                  false,
                                  false);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::Demo,
                                  E_FONT_TYPE::Calibri,
                                  "John Joseph Beckmeyer:",
                                  0.05f * ScaleXRatio,
                                  0.05f * ScaleYRatio,
                                  -0.3f * PosXRatio,
                                  0.15f * PosYRatio,
                                  false,
                                  false);
                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::Demo,
                                  E_FONT_TYPE::Calibri,
                                  "Audio Developer",
                                  0.05f * ScaleXRatio,
                                  0.05f * ScaleYRatio,
                                  0.3f * PosXRatio,
                                  0.15f * PosYRatio,
                                  false,
                                  false);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::Demo,
                                  E_FONT_TYPE::Calibri,
                                  "CONTINUE",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  -0.15f * PosXRatio,
                                  0.3f * PosYRatio,
                                  false,
                                  true,
                                  true,
                                  pauseButtonWidth * ScaleXRatio,
                                  pauseButtonHeight * ScaleYRatio);

                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::Demo,
                                  E_FONT_TYPE::Calibri,
                                  "EXIT",
                                  0.035f * ScaleXRatio,
                                  0.035f * ScaleYRatio,
                                  0.15f * PosXRatio,
                                  0.3f * PosYRatio,
                                  false,
                                  true,
                                  true,
                                  pauseButtonWidth * ScaleXRatio,
                                  pauseButtonHeight * ScaleYRatio);
        }
        //{
        //        if (globalTimer < 5.0f)
        //        {
        //                static auto dbg_test = 0;
        //                dbg_test++;
        //                // Full Sail Logo
        //                UIManager::instance->Splash_FullSail();
        //        }
        //        else if (globalTimer >= 5.0f && globalTimer < 10.0f)
        //        {
        //                // GP Games Logo
        //                UIManager::instance->Splash_GPGames();
        //        }
        //        else if (globalTimer >= 10.0f && globalTimer < 15.0f)
        //        {
        //                // Deep!deep Logo
        //                UIManager::instance->Splash_Team();
        //        }
        //        if (globalTimer >= 15.0f)
        //        {
        //                UIManager::instance->Splash_End();
        //        }
        //}
        TimedFunction fadeInSplashScreen0;
        fadeInSplashScreen0.delay = 0;
        fadeInSplashScreen0.func  = []() { UIManager::instance->Splash_FullSail(); };
        TimedFunction fadeOutSplashScreen0;
        fadeOutSplashScreen0.delay = 3.5;
        fadeOutSplashScreen0.func  = []() {
                UIManager::instance->m_AllSprites[E_MENU_CATEGORIES::SplashScreen][0].desiredColor = {1, 1, 1, 0};
        };
        TimedFunction fadeInSplashScreen1;
        fadeInSplashScreen1.delay = 5;
        fadeInSplashScreen1.func  = []() { UIManager::instance->Splash_GPGames(); };
        TimedFunction fadeOutSplashScreen1;
        fadeOutSplashScreen1.delay = 8.5;
        fadeOutSplashScreen1.func  = []() {
                UIManager::instance->m_AllSprites[E_MENU_CATEGORIES::SplashScreen][1].desiredColor = {1, 1, 1, 0};
        };
        TimedFunction fadeInSplashScreen2;
        fadeInSplashScreen2.delay = 10;
        fadeInSplashScreen2.func  = []() { UIManager::instance->Splash_Team(); };
        TimedFunction fadeOutSplashScreen2;
        fadeOutSplashScreen2.delay = 13.5;
        fadeOutSplashScreen2.func  = []() {
                UIManager::instance->m_AllSprites[E_MENU_CATEGORIES::SplashScreen][2].desiredColor = {1, 1, 1, 0};
        };
        TimedFunction splashEndDelayd;
        splashEndDelayd.delay = 15;
        splashEndDelayd.func  = []() { UIManager::instance->Splash_End(); };

        instance->timed_functions.push_back(fadeInSplashScreen0);
        instance->timed_functions.push_back(fadeInSplashScreen1);
        instance->timed_functions.push_back(fadeInSplashScreen2);
        instance->timed_functions.push_back(splashEndDelayd);
        instance->timed_functions.push_back(fadeOutSplashScreen0);
        instance->timed_functions.push_back(fadeOutSplashScreen1);
        instance->timed_functions.push_back(fadeOutSplashScreen2);
        // Pause Menu

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

                // Disable other Sensitivities
                if (instance->CSettings.m_Sensitivity == 0)
                {
                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][8].mEnabled = true;

                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][9].mEnabled  = false;
                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][10].mEnabled = false;
                }
                else if (instance->CSettings.m_Sensitivity == 1)
                {

                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][9].mEnabled = true;

                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][8].mEnabled  = false;
                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][10].mEnabled = false;
                }
                else
                {
                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][10].mEnabled = true;

                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][8].mEnabled = false;
                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][9].mEnabled = false;
                }

                // Fullscreen on/off Check
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

                int ResBegin =
                    (int)(instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu].size() - instance->resDescriptors.size());
                int ResEnd = (int)(instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu].size());
                for (int i = ResBegin; i < ResEnd; i++)
                {
                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][i].mEnabled = false;
                }
                instance
                    ->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu]
                                [instance->CSettings.m_Resolution +
                                 instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu].size() -
                                 instance->resDescriptors.size()]
                    .mEnabled = true;


                int VolBegin = (int)(instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu].size() -
                                     instance->resDescriptors.size() - 11);
                int VolEnd =
                    (int)(instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu].size() - instance->resDescriptors.size());

                for (int i = VolBegin; i < VolEnd; i++)
                {
                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][i].mEnabled = false;
                }
                instance
                    ->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu]
                                [(size_t)(instance->CSettings.m_Volume * 0.1f - 11) +
                                 instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu].size() -
                                 instance->resDescriptors.size()]
                    .mEnabled = true;
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


                instance->CSettings = instance->PSettings;

                instance->SetFullscreen(instance->PSettings.m_IsFullscreen);
                instance->AdjustResolution(instance->m_window,
                                           instance->resDescriptors[instance->PSettings.m_Resolution].Width,
                                           instance->resDescriptors[instance->PSettings.m_Resolution].Height);
        });

        // Window Mode
        instance->m_AllSprites[E_MENU_CATEGORIES::OptionsMenu][1].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                if (instance->CSettings.m_IsFullscreen == false)
                {
                        instance->CSettings.m_IsFullscreen                                  = true;
                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][0].mEnabled = true;  // Off
                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][1].mEnabled = false; // On
                        // instance->CSettings.m_Resolution                                    = 8;
                }
                else
                {
                        instance->CSettings.m_IsFullscreen                                  = false;
                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][0].mEnabled = false; // Off
                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][1].mEnabled = true;  // On
                }
                instance->SetFullscreen(instance->CSettings.m_IsFullscreen);
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


                // instance->AdjustResolution(instance->m_window,
                //                           instance->resDescriptors[instance->CSettings.m_Resolution].Width,
                //                           instance->resDescriptors[instance->CSettings.m_Resolution].Height);

                // instance->SetFullscreen(instance->CSettings.m_IsFullscreen);
                AudioManager::Get()->SetMasterVolume(0.1f * instance->CSettings.m_Volume);

                instance->PSettings = instance->CSettings;
        });

        // Left Resolution Button
        instance->m_AllSprites[E_MENU_CATEGORIES::OptionsSubmenu][0].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                if (instance->CSettings.m_Resolution - 1 <= -1)
                {
                        instance->CSettings.m_Resolution = int(instance->resDescriptors.size() - 1);
                }
                else
                {
                        instance->CSettings.m_Resolution--;
                }

                instance->UpdateResolutionText();
                // Change Resolution HERE
                instance->AdjustResolution(instance->m_window,
                                           instance->resDescriptors[instance->CSettings.m_Resolution].Width,
                                           instance->resDescriptors[instance->CSettings.m_Resolution].Height);
        });

        // Right Resolution Button
        instance->m_AllSprites[E_MENU_CATEGORIES::OptionsSubmenu][1].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                if (instance->CSettings.m_Resolution + 1 >= instance->resDescriptors.size())
                {
                        instance->CSettings.m_Resolution = 0;
                }
                else
                {
                        instance->CSettings.m_Resolution++;
                }

                instance->UpdateResolutionText();

                // Change Resolution HERE
                instance->AdjustResolution(instance->m_window,
                                           instance->resDescriptors[instance->CSettings.m_Resolution].Width,
                                           instance->resDescriptors[instance->CSettings.m_Resolution].Height);
        });

        // Left Sensitivity Button
        instance->m_AllSprites[E_MENU_CATEGORIES::OptionsSubmenu][2].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                if (instance->CSettings.m_Sensitivity - 1 <= -1)
                {
                        instance->CSettings.m_Sensitivity = 2;
                }
                else
                {
                        instance->CSettings.m_Sensitivity--;
                }

                // Disable other Sensitivities
                if (instance->CSettings.m_Sensitivity == 0)
                {
                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][8].mEnabled = true;

                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][9].mEnabled  = false;
                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][10].mEnabled = false;
                }
                else if (instance->CSettings.m_Sensitivity == 1)
                {

                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][9].mEnabled = true;

                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][8].mEnabled  = false;
                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][10].mEnabled = false;
                }
                else
                {
                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][10].mEnabled = true;

                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][8].mEnabled = false;
                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][9].mEnabled = false;
                }
                // Change Sensitivity HERE

                ControllerSystem* controllerSys      = SYSTEM_MANAGER->GetSystem<ControllerSystem>();
                PlayerController* m_PlayerController = static_cast<PlayerController*>(controllerSys->GetCurrentController());
                m_PlayerController->SetSensitivity(instance->CSettings.m_Sensitivity);
        });

        // Right Sensitivity Button
        instance->m_AllSprites[E_MENU_CATEGORIES::OptionsSubmenu][3].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                if (instance->CSettings.m_Sensitivity + 1 >= 3)
                {
                        instance->CSettings.m_Sensitivity = 0;
                }
                else
                {
                        instance->CSettings.m_Sensitivity++;
                }

                // Disable other Sensitivities
                if (instance->CSettings.m_Sensitivity == 0)
                {
                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][8].mEnabled = true;

                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][9].mEnabled  = false;
                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][10].mEnabled = false;
                }
                else if (instance->CSettings.m_Sensitivity == 1)
                {

                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][9].mEnabled = true;

                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][8].mEnabled  = false;
                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][10].mEnabled = false;
                }
                else
                {
                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][10].mEnabled = true;

                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][8].mEnabled = false;
                        instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][9].mEnabled = false;
                }
                // Change Sensitivity HERE

                ControllerSystem* controllerSys      = SYSTEM_MANAGER->GetSystem<ControllerSystem>();
                PlayerController* m_PlayerController = static_cast<PlayerController*>(controllerSys->GetCurrentController());
                m_PlayerController->SetSensitivity(instance->CSettings.m_Sensitivity);
        });

        // Left Volume Button
        instance->m_AllSprites[E_MENU_CATEGORIES::OptionsSubmenu][4].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                if (instance->CSettings.m_Volume * 0.1f > 0)
                {
                        instance->CSettings.m_Volume -= 10;
                        int VolBegin = (int)(instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu].size() -
                                             instance->resDescriptors.size() - 11);
                        int VolEnd   = (int)(instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu].size() -
                                           instance->resDescriptors.size());
                        for (int i = VolBegin; i < VolEnd; i++)
                        {
                                instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][i].mEnabled = false;
                        }
                        instance
                            ->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu]
                                        [(size_t)(instance->CSettings.m_Volume * 0.1f - 11) +
                                         instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu].size() -
                                         instance->resDescriptors.size()]
                            .mEnabled = true;
                        // Change Volume HERE
                        AudioManager::Get()->SetMasterVolume(0.1f * instance->CSettings.m_Volume);
                }
        });

        // Right Volume Button
        instance->m_AllSprites[E_MENU_CATEGORIES::OptionsSubmenu][5].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                if (instance->CSettings.m_Volume * 0.1f < 10)
                {
                        instance->CSettings.m_Volume += 10;

                        int VolBegin = (int)(instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu].size() -
                                             instance->resDescriptors.size() - 11);
                        int VolEnd   = (int)(instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu].size() -
                                           instance->resDescriptors.size());
                        for (int i = VolBegin; i < VolEnd; i++)
                        {
                                instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu][i].mEnabled = false;
                        }
                        instance
                            ->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu]
                                        [(size_t)(instance->CSettings.m_Volume * 0.1f - 11) +
                                         instance->m_AllFonts[E_MENU_CATEGORIES::OptionsSubmenu].size() -
                                         instance->resDescriptors.size()]
                            .mEnabled = true;
                        // Change Volume HERE
                        AudioManager::Get()->SetMasterVolume(0.1f * instance->CSettings.m_Volume);
                }
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

        // Tutorial Button
        instance->m_AllSprites[E_MENU_CATEGORIES::LevelMenu][1].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                for (auto& itr : instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu])
                {
                        auto currColor   = itr.desiredColor;
                        itr.desiredColor = {currColor.m128_f32[0], currColor.m128_f32[1], currColor.m128_f32[2], 0};
                        itr.currColor    = {currColor.m128_f32[0], currColor.m128_f32[1], currColor.m128_f32[2], 0};
                }
                instance->m_currentTutorialIcon = {0, 4};
                if (GamePad::Get()->CheckConnection() == true)
                {
                        instance->m_AllSprites[E_MENU_CATEGORIES::LevelMenu][0].currColor    = {1.0f, 1.0f, 1.0f, 1.0f};
                        instance->m_AllSprites[E_MENU_CATEGORIES::LevelMenu][0].desiredColor = {1.0f, 1.0f, 1.0f, 1.0f};
                        instance->m_AllSprites[E_MENU_CATEGORIES::LevelMenu][0].mEnabled     = true;
                }
                else
                {
                        instance->m_AllSprites[E_MENU_CATEGORIES::LevelMenu][4].currColor    = {1.0f, 1.0f, 1.0f, 1.0f};
                        instance->m_AllSprites[E_MENU_CATEGORIES::LevelMenu][4].desiredColor = {1.0f, 1.0f, 1.0f, 1.0f};
                        instance->m_AllSprites[E_MENU_CATEGORIES::LevelMenu][4].mEnabled     = true;
                }

                instance->Unpause();
                // Load Level Function
                auto curLevel = GEngine::Get()->GetLevelStateManager()->GetCurrentLevelState();

                if (curLevel->GetLevelType() == TUTORIAL_LEVEL)
                {
                        GEngine::Get()->GetLevelStateManager()->ForceLoadState(
                            E_LevelStateEvents::TUTORIAL_LEVEL_TO_TUTORIAL_LEVEL);
                }

                else if (curLevel->GetLevelType() == LEVEL_01)
                {
                        GEngine::Get()->GetLevelStateManager()->ForceLoadState(E_LevelStateEvents::LEVEL_01_TO_TUTORIAL_LEVEL);
                }

                else if (curLevel->GetLevelType() == LEVEL_02)
                {
                        GEngine::Get()->GetLevelStateManager()->ForceLoadState(E_LevelStateEvents::LEVEL_02_TO_TUTORIAL_LEVEL);
                }

                else if (curLevel->GetLevelType() == LEVEL_03)
                {
                        GEngine::Get()->GetLevelStateManager()->ForceLoadState(E_LevelStateEvents::LEVEL_03_TO_TUTORIAL_LEVEL);
                }

                else if (curLevel->GetLevelType() == LEVEL_04)
                {
                        GEngine::Get()->GetLevelStateManager()->ForceLoadState(E_LevelStateEvents::LEVEL_04_TO_TUTORIAL_LEVEL);
                }
        });

        // Level 1 Button
        instance->m_AllSprites[E_MENU_CATEGORIES::LevelMenu][2].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                instance->Unpause();
                // Load Level Function
                auto curLevel = GEngine::Get()->GetLevelStateManager()->GetCurrentLevelState();

                if (curLevel->GetLevelType() == TUTORIAL_LEVEL)
                {
                        GEngine::Get()->GetLevelStateManager()->ForceLoadState(E_LevelStateEvents::TUTORIAL_LEVEL_TO_LEVEL_01);
                }

                else if (curLevel->GetLevelType() == LEVEL_01)
                {
                        GEngine::Get()->GetLevelStateManager()->ForceLoadState(E_LevelStateEvents::LEVEL_01_TO_LEVEL_01);
                }

                else if (curLevel->GetLevelType() == LEVEL_02)
                {
                        GEngine::Get()->GetLevelStateManager()->ForceLoadState(E_LevelStateEvents::LEVEL_02_TO_LEVEL_01);
                }

                else if (curLevel->GetLevelType() == LEVEL_03)
                {
                        GEngine::Get()->GetLevelStateManager()->ForceLoadState(E_LevelStateEvents::LEVEL_03_TO_LEVEL_01);
                }

                else if (curLevel->GetLevelType() == LEVEL_04)
                {
                        GEngine::Get()->GetLevelStateManager()->ForceLoadState(E_LevelStateEvents::LEVEL_04_TO_LEVEL_01);
                }

                for (auto& itr : instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu])
                {
                        itr.mEnabled = false;
                }
                instance->m_currentTutorialIcon = {-1, -1};
        });

        // Level 2 Button
        instance->m_AllSprites[E_MENU_CATEGORIES::LevelMenu][3].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                instance->Unpause();
                // Load Level Function
                auto curLevel = GEngine::Get()->GetLevelStateManager()->GetCurrentLevelState();

                if (curLevel->GetLevelType() == TUTORIAL_LEVEL)
                {
                        GEngine::Get()->GetLevelStateManager()->ForceLoadState(E_LevelStateEvents::TUTORIAL_LEVEL_TO_LEVEL_02);
                }

                else if (curLevel->GetLevelType() == LEVEL_01)
                {
                        GEngine::Get()->GetLevelStateManager()->ForceLoadState(E_LevelStateEvents::LEVEL_01_TO_LEVEL_02);
                }

                else if (curLevel->GetLevelType() == LEVEL_02)
                {
                        GEngine::Get()->GetLevelStateManager()->ForceLoadState(E_LevelStateEvents::LEVEL_02_TO_LEVEL_02);
                }

                else if (curLevel->GetLevelType() == LEVEL_03)
                {
                        GEngine::Get()->GetLevelStateManager()->ForceLoadState(E_LevelStateEvents::LEVEL_03_TO_LEVEL_02);
                }

                else if (curLevel->GetLevelType() == LEVEL_04)
                {
                        GEngine::Get()->GetLevelStateManager()->ForceLoadState(E_LevelStateEvents::LEVEL_04_TO_LEVEL_02);
                }
                for (auto& itr : instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu])
                {
                        itr.mEnabled = false;
                }
                instance->m_currentTutorialIcon = {-1, -1};
        });

        // Level 3 Button
        instance->m_AllSprites[E_MENU_CATEGORIES::LevelMenu][4].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                instance->Unpause();
                // Load Level Function
                auto curLevel = GEngine::Get()->GetLevelStateManager()->GetCurrentLevelState();

                if (curLevel->GetLevelType() == TUTORIAL_LEVEL)
                {
                        GEngine::Get()->GetLevelStateManager()->ForceLoadState(E_LevelStateEvents::TUTORIAL_LEVEL_TO_LEVEL_03);
                }

                else if (curLevel->GetLevelType() == LEVEL_01)
                {
                        GEngine::Get()->GetLevelStateManager()->ForceLoadState(E_LevelStateEvents::LEVEL_01_TO_LEVEL_03);
                }

                else if (curLevel->GetLevelType() == LEVEL_02)
                {
                        GEngine::Get()->GetLevelStateManager()->ForceLoadState(E_LevelStateEvents::LEVEL_02_TO_LEVEL_03);
                }

                else if (curLevel->GetLevelType() == LEVEL_03)
                {
                        GEngine::Get()->GetLevelStateManager()->ForceLoadState(E_LevelStateEvents::LEVEL_03_TO_LEVEL_03);
                }

                else if (curLevel->GetLevelType() == LEVEL_04)
                {
                        GEngine::Get()->GetLevelStateManager()->ForceLoadState(E_LevelStateEvents::LEVEL_04_TO_LEVEL_03);
                }

                for (auto& itr : instance->m_AllSprites[E_MENU_CATEGORIES::MainMenu])
                {
                        itr.mEnabled = false;
                }
                instance->m_currentTutorialIcon = {-1, -1};
        });


        // Controls Select

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


        // Demo

        // Continue
        instance->m_AllSprites[E_MENU_CATEGORIES::Demo][0].OnMouseDown.AddEventListener(
            [](UIMouseEvent* e) { instance->Unpause(); });

        // Exit
        instance->m_AllSprites[E_MENU_CATEGORIES::Demo][1].OnMouseDown.AddEventListener(
            [](UIMouseEvent* e) { GEngine::Get()->RequestGameExit(); });
}

void UIManager::Update(float deltaTime)
{
        GamePad::Get()->Refresh();
        GEngine::Get()->m_MainThreadProfilingContext.Begin("UIManager", "UIManager");
        using namespace DirectX;

        instance->m_ScreenSize =
            XMFLOAT2{instance->m_RenderSystem->m_BackBufferWidth, instance->m_RenderSystem->m_BackBufferHeight};
        instance->m_ScreenCenter = XMVectorSet(instance->m_ScreenSize.x * 0.5f, instance->m_ScreenSize.y * 0.5f, 0.0f, 1.0f);
        instance->m_TexelSize    = XMFLOAT2{1.0f / instance->m_ScreenSize.x, 1.0f / instance->m_ScreenSize.y};

        static float GlobalTimer = 0.0f;

        if (GlobalTimer < 15.1f && !instance->m_BreakSplash)
        {
                instance->SplashUpdate(GlobalTimer, deltaTime);
                GlobalTimer += GEngine::Get()->GetDeltaTime();
        }
        else
        {
                instance->GameplayUpdate(deltaTime);
        }

        instance->DrawSprites(deltaTime);
        for (auto& it : instance->m_AllFonts)
                for (auto& font : it.second)
                {
                        if (font.mEnabled)
                        {
                                instance->m_SpriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred,
                                                               instance->m_States->NonPremultiplied());

                                XMVECTOR position = XMVectorSet(font.mScreenOffset.x * instance->m_ScreenSize.x / 2.0f,
                                                                font.mScreenOffset.y * instance->m_ScreenSize.y / 2.0f,
                                                                0.0f,
                                                                0.0f) +
                                                    instance->m_ScreenCenter;

                                XMVECTOR scale = XMVectorSet(font.mScaleX * instance->m_ScreenSize.x / 2.0f,
                                                             font.mScaleY * instance->m_ScreenSize.y / 2.0f,
                                                             0.0f,
                                                             0.0f);

                                font.currColor =
                                    MathLibrary::MoveVectorColorTowards(font.currColor, font.desiredColor, deltaTime * 1.0f);

                                instance->m_FontTypes[font.mFontType]->DrawString(instance->m_SpriteBatch.get(),
                                                                                  font.mTextDisplay.c_str(),
                                                                                  position,
                                                                                  font.currColor,
                                                                                  0.0f,
                                                                                  font.mOrigin,
                                                                                  scale);

                                instance->m_SpriteBatch->End();
                        }
                }
        GEngine::Get()->m_MainThreadProfilingContext.End();

        for (int i = 0; i < instance->timed_functions.size(); i++)
        {
                instance->timed_functions[i].delay -= deltaTime;
                if (instance->timed_functions[i].delay <= 0)
                {
                        instance->timed_functions[i].func();
                        instance->timed_functions.erase(instance->timed_functions.begin() + i);
                        i--;
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
                }

        for (int i = 0; i < E_FONT_TYPE::COUNT; ++i)
        {
                delete instance->m_FontTypes[i];
        }
        assert(instance);
        delete instance;
}