#include "UIManager.h"
#include "../Engine/CoreInput/CoreInput.h"
#include "../Engine/GEngine.h"
#include "../Rendering/RenderingSystem.h"


UIManager* UIManager::instance;

void UIManager::CreateSprite(SpriteComponent& createComp, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
        // Setup
        instance->mSpriteBatch = std::make_unique<DirectX::SpriteBatch>(deviceContext);
        instance->mStates      = std::make_unique<DirectX::CommonStates>(device);

        Microsoft::WRL::ComPtr<ID3D11Resource> resource;
        HRESULT hr = DirectX::CreateDDSTextureFromFile(device, L"../Assets/2d/Sprite/GuyMoon.dds", resource.GetAddressOf(), &createComp.mTexture);
        if (FAILED(hr))
        {
                exit(-1);
        }

        Microsoft::WRL::ComPtr<ID3D11Texture2D> GuyMoon;
        resource.As(&GuyMoon);

        CD3D11_TEXTURE2D_DESC GuyMoonDesc;
        GuyMoon->GetDesc(&GuyMoonDesc);

        createComp.mOrigin.x = float(GuyMoonDesc.Width * 0.5);
        createComp.mOrigin.y = float(GuyMoonDesc.Height * 0.5);

		//Set the Main menu to enabled
        createComp.enabled = true;

		//Sprite Screen Position
		createComp.SetPosition(GuyMoonDesc.Width * 0.5, 0);
					

		// Rectangle
        createComp.mRectangle.x      = createComp.mOrigin.x;
        createComp.mRectangle.y      = createComp.mOrigin.y;
        createComp.mRectangle.width  = GuyMoonDesc.Width;
        createComp.mRectangle.height = GuyMoonDesc.Height;

        instance->mSprites.push_back(createComp);


        GuyMoon.Reset();
        resource.Reset();
}

void UIManager::CreateText(SpriteComponent& baseSprite, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
        FontComponent* createText = new FontComponent();
        createText->mSpriteFont   = std::make_unique<DirectX::SpriteFont>(device, L"../Assets/2d/Text/myfile.spritefont");
        createText->mTextDisplay  = "Hello World!";

		//Set the Main Menu text to enabled
        createText->enabled = true;

        // Text Screen Position
        createText->mScreenPos.x = baseSprite.mOrigin.x * 0.5;
        createText->mScreenPos.y = 0;

        // Error C2280
        // attempting to reference a deleted function
        // C:\Program Files(x86)\Microsoft Visual Studio\2019\Preview\VC\Tools\MSVC\14.20.27508\include\xmemory0 819
        // FIXED
        // Making the vector array an array of pointers fixed this issue
        instance->mSpriteFonts.push_back(createText);
}

void UIManager::Initialize()
{
        assert(!instance);
        instance = new UIManager;

        auto renderSystem = GEngine::Get()->GetSystemManager()->GetSystem<RenderSystem>();

        SpriteComponent temp;
        instance->CreateSprite(temp, renderSystem->m_Device, renderSystem->m_Context);
        instance->CreateText(instance->mSprites[0], renderSystem->m_Device, renderSystem->m_Context);
}

void UIManager::Update()
{
        if (GCoreInput::GetKeyState(KeyCode::Control) == KeyState::DownFirst) 
		{
					//Change input from Control to Escape whenever implimented
				//Disable or enable all Sprites based off of input
				//Sprites
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
				//Text
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
		}
        
		//Sprite Display
		for (int i = 0; i < instance->mSprites.size(); i++)
        {
                if (instance->mSprites[i].enabled == true)
                {
                        instance->mSpriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred,
                                                      instance->mStates->NonPremultiplied());

                        instance->mSpriteBatch->Draw(
                            instance->mSprites[i].mTexture,
                            DirectX::XMVECTOR{instance->mSprites[i].mScreenPos.x, instance->mSprites[i].mScreenPos.y});

                        instance->mSpriteBatch->End();
                }
        }
        //Text Display
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
