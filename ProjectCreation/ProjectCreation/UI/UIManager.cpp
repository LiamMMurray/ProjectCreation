#include "UIManager.h"
#include "../Engine/CoreInput/CoreInput.h"
#include "../Engine/GEngine.h"
#include "../Rendering/RenderingSystem.h"

UIManager* UIManager::instance;

//Adds a sprite to the vector of sprites
//Currently PositionX and PositionY are not used
void       UIManager::AddSprite(ID3D11Device*        device,
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

        cSprite.mOrigin.x = float(TextureDesc.Width * 0.5);
        cSprite.mOrigin.y = float(TextureDesc.Height * 0.5);

        // Set the Id of the Sprite for the Main Menu
        if (instance->mSprites.size() <= 0) 
		{
                cSprite.id = 1;
		}
        else
        {
                cSprite.id = instance->mSprites[instance->mSprites.size() - 1].id + 1;
        }
        // Set the Sprite to enabled
        cSprite.enabled = false;

        // Sprite Screen Position
        cSprite.SetPosition(TextureDesc.Width * 0.5, 0);


        // Rectangle
        cSprite.mRectangle.top  = cSprite.mOrigin.y;
        cSprite.mRectangle.left = cSprite.mOrigin.x;

        cSprite.mRectangle.bottom = cSprite.mOrigin.y + TextureDesc.Height;
        cSprite.mRectangle.right  = cSprite.mOrigin.x + TextureDesc.Width;

        instance->mSprites.push_back(cSprite);


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

        createText->mSpriteFont = std::make_unique<DirectX::SpriteFont>(device, FileName);
		createText->mTextDisplay = TextDisplay;

		
			//Microsoft::WRL::ComPtr<ID3D11Resource> resource;
			//HRESULT hr = DirectX::CreateDDSTextureFromFile(device, FileName, resource.GetAddressOf(), &createText->mTexture);
			//if (FAILED(hr))
			//{
			//        exit(-1);
			//}
			//Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture;
			//resource.As(&Texture);
			//
			//CD3D11_TEXTURE2D_DESC TextureDesc;
			//Texture->GetDesc(&TextureDesc);

        // Set the Main Menu text to enabled
        createText->enabled = false;

        // Text Screen Position
        createText->mScreenPos.x = instance->mSprites[0].mOrigin.x;
        createText->mScreenPos.y = 0;

			////Rectangle
			//createText->mRectangle.top  = createText->mScreenPos.y;
			//createText->mRectangle.left = createText->mScreenPos.x;
			//
			//createText->mRectangle.bottom = createText->mScreenPos.y + TextureDesc.Height;
			//createText->mRectangle.right  = createText->mScreenPos.x + TextureDesc.Width;


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

        instance->AddSprite(renderSystem->m_Device,
                            renderSystem->m_Context,
                            L"../Assets/2d/Sprite/GuyMoon.dds",
                            0.0f,
                            0.0f);
        instance->AddText(renderSystem->m_Device,
                          renderSystem->m_Context,
                          L"../Assets/2d/Text/myfile.spritefont",
                          "New Game",
                            0.0f,
                            0.0f);
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
                        if (instance->mSprites[i].enabled == true)
                        {
                                instance->mSprites[i].enabled = false;
                        }
                        else
                        {
                                instance->mSprites[i].enabled = true;
                        }
                }
                // Text
                for (int i = 0; i < instance->mSpriteFonts.size(); i++)
                {
                        if (instance->mSpriteFonts[i]->enabled == true)
                        {
                                instance->mSpriteFonts[i]->enabled = false;
                        }
                        else
                        {
                                instance->mSpriteFonts[i]->enabled = true;
                        }
                }
                // Pause Game Afterwards
        }

        // Sprite Display
        for (int i = 0; i < instance->mSprites.size(); i++)
        {
                if (instance->mSprites[i].enabled == true)
                {
                        if (GCoreInput::GetMouseState(MouseCode::LeftClick) == KeyState::Release)
                        {
                                // Setup mouse cursor input here
                                // No current way to get mouse position

                                instance->mCursor.x = GCoreInput::GetMouseX();
                                instance->mCursor.y = GCoreInput::GetMouseY();

                                if (PtInRect(&instance->mSprites[i].mRectangle, instance->mCursor))
                                {
                                       //Button Was Pressed


                                        instance->mSpriteFonts[0]->enabled = false;
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
                if (instance->mSpriteFonts[i]->enabled == true)
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
