#include "UIManager.h"
#include <iostream>
#include "../Engine/CoreInput/CoreInput.h"
#include "../Engine/GEngine.h"
#include "../Rendering/RenderingSystem.h"
UIManager* UIManager::instance;

// Adds a sprite to the vector of sprites
// Currently PositionX and PositionY are not used
void UIManager::AddSprite(ID3D11Device*        device,
                          ID3D11DeviceContext* deviceContext,
                          const wchar_t*       FileName,
                          float                PositionX,
                          float                PositionY)
{
        SpriteComponent cSprite;

        instance->mSpriteBatch = std::make_unique<DirectX::SpriteBatch>(deviceContext);
        instance->mStates      = std::make_unique<DirectX::CommonStates>(device);

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
        cSprite.mOrigin.x = float(TextureDesc.Width * 0.5);
        cSprite.mOrigin.y = float(TextureDesc.Height * 0.5);

        // Add the width and height to the sprite
        cSprite.mWidth  = TextureDesc.Width;
        cSprite.mHeight = TextureDesc.Height;

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

        // Sprite Screen Position
        cSprite.SetPosition(cSprite.mOrigin.x, PositionY);

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

        //Set the Main Menu text to enabled
        createText->mEnabled = false;

        //Text Screen Position
        createText->mScreenPos.x = PositionX;
        createText->mScreenPos.y = PositionY;

		//Create Dimensions
		const char* tempText = createText->mTextDisplay.c_str();
        DirectX::XMVECTOR tDimensions = createText->mSpriteFont->MeasureString(tempText);

		createText->MakeRectangle();

        // Error C2280
        // attempting to reference a deleted function
        // C:\Program Files(x86)\Microsoft Visual Studio\2019\Preview\VC\Tools\MSVC\14.20.27508\include\xmemory0 819
        // FIXED
        // Making the vector array an array of pointers fixed this issue
        instance->mSpriteFonts.push_back(createText);
}

void UIManager::RemoveText(int id)
{}

void UIManager::Initialize()
{
        assert(!instance);
        instance = new UIManager;

        auto renderSystem = GEngine::Get()->GetSystemManager()->GetSystem<RenderSystem>();

        instance->AddSprite(renderSystem->m_Device, renderSystem->m_Context, L"../Assets/2d/Sprite/Grey Box Test.dds", 0.0f, 0.0f);
        instance->AddSprite(renderSystem->m_Device, renderSystem->m_Context, L"../Assets/2d/Sprite/cat.dds", 0.0f, 0.0f);

        instance->AddText(renderSystem->m_Device,
                          renderSystem->m_Context,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "New Game",
                          instance->mSprites[0].mOrigin.x,
                          15.0f);
        instance->AddText(renderSystem->m_Device,
                          renderSystem->m_Context,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Load Game",
                          instance->mSprites[0].mOrigin.x,
                          65.0f);
        instance->AddText(renderSystem->m_Device,
                          renderSystem->m_Context,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Options",
                          instance->mSprites[0].mOrigin.x,
                          115.0f);
        instance->AddText(renderSystem->m_Device,
                          renderSystem->m_Context,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "Exit",
                          instance->mSprites[0].mOrigin.x,
                          165.0f);

		//Events
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

		instance->mSprites[1].OnMouseDown.AddEventListener([](UIMouseEvent* e) {
                exit(1);
        });
}

void UIManager::Update()
{
        if (GCoreInput::GetKeyState(KeyCode::Esc) == KeyState::DownFirst)
        {
                // Change input from Control to Escape whenever implimented
                // Disable or enable all Sprites based off of input
                // Sprites
                for (int i = 0; i < instance->mSprites.size(); i++)
                {
                        if (instance->mSprites[i].mEnabled == true)
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
                        if (instance->mSpriteFonts[i]->mEnabled == true)
                        {
                                instance->mSpriteFonts[i]->mEnabled = false;
                        }
                        else
                        {
                                instance->mSpriteFonts[i]->mEnabled = true;
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
										
                                        e.mouseX = GCoreInput::GetMouseWindowPosX();
                                        e.mouseY = GCoreInput::GetMouseWindowPosY();
                                        e.sprite = &instance->mSprites[0];
                                        instance->mSprites[i].OnMouseDown.Invoke(&e);
                                        

                                        //if (instance->mSprites[i].mId == 2)
                                        //{
                                        //        instance->mSpriteFonts[0]->mEnabled = false;
                                        //}
                                        //else
                                        //{
                                        //        instance->mSpriteFonts[0]->mEnabled = true;
										//}

                                        // exit(1);
                                }
                        }
                        instance->mSpriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred,
                                                      instance->mStates->NonPremultiplied());

                        instance->mSpriteBatch->Draw(
                            instance->mSprites[i].mTexture,
                            DirectX::XMVECTOR{instance->mSprites[i].mScreenPos.x, instance->mSprites[i].mScreenPos.y});

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
                                                                           instance->mSpriteFonts[i]->mScreenPos);

                        instance->mSpriteBatch->End();
                }
        }
}

void UIManager::Shutdown()
{
        instance->mStates.reset();

        for (int i = 0; i < instance->mSprites.size(); i++)
        {
                instance->mSprites[i].mTexture->Release();
        }
        for (int i = 0; i < instance->mSpriteFonts.size(); i++)
        {
                instance->mSpriteFonts[i]->mSpriteFont.reset();
        }
        assert(instance);
        delete instance;
}
