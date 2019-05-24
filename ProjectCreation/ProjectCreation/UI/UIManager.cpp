#include "UIManager.h"
#include "../Engine/GEngine.h"
#include "../Rendering/RenderingSystem.h"


UIManager* UIManager::instance;

void UIManager::CreateSprite(SpriteComponent&     createComp,
                             ID3D11Device*             device,
                             ID3D11DeviceContext*      deviceContext)
{
        instance->m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(deviceContext);

		instance->m_states = std::make_unique<DirectX::CommonStates>(device);

        Microsoft::WRL::ComPtr<ID3D11Resource> resource;
        //HRESULT hr = DirectX::CreateWICTextureFromFile(device, L"../Assets/2d/GuyMoon.jpg", resource.GetAddressOf(), &createComp.m_texture);
        HRESULT hr = DirectX::CreateDDSTextureFromFile(device, L"../Assets/2d/cat.dds", resource.GetAddressOf(), &createComp.m_texture);
        if (FAILED(hr)) 
		{
			exit(-1);
		}

        Microsoft::WRL::ComPtr<ID3D11Texture2D> GuyMoon;
        resource.As(&GuyMoon);

        CD3D11_TEXTURE2D_DESC GuyMoonDesc;
        GuyMoon->GetDesc(&GuyMoonDesc);

        createComp.m_origin.x = float(GuyMoonDesc.Width / 2);
        createComp.m_origin.y = float(GuyMoonDesc.Height / 2);

		GuyMoon.Reset();
		resource.Reset();
}

void UIManager::Initialize()
{
        assert(!instance);
        instance       = new UIManager;

        auto renderSystem = GEngine::Get()->GetSystemManager()->GetSystem<RenderSystem>();

        instance->CreateSprite(instance->m_Sprite, renderSystem->m_Device, renderSystem->m_Context);
}

void UIManager::Update()
{
        instance->m_spriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred, instance->m_states->NonPremultiplied());
        //instance->m_spriteBatch->Begin();
        
		instance->m_spriteBatch->Draw(instance->m_Sprite.m_texture, DirectX::XMVECTOR{instance->m_Sprite.m_screenPos.x, instance->m_Sprite.m_screenPos.y});

		instance->m_spriteBatch->End();
}

void UIManager::Shutdown()
{
        instance->m_states.reset();

		instance->m_Sprite.m_texture->Release();
        assert(instance);
        delete instance;
}
