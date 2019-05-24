#pragma once

#include <memory>
#include <wrl/client.h>

#include <DDSTextureLoader.h>
#include "WICTextureLoader.h"

#include "SpriteComponent.h"
#include "CommonStates.h"

class UIManager
{
    public:
        static void Initialize();
        static void Update();
        static void Shutdown();

        void CreateSprite(SpriteComponent&           createComp,
                          ID3D11Device*             device,
                          ID3D11DeviceContext*      deviceContext);

        
        SpriteComponent                       m_Sprite;
        std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
        std::unique_ptr<DirectX::CommonStates> m_states;
    private:
        static UIManager* instance;
};
