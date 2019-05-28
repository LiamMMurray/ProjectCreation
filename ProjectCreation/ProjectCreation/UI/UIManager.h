#pragma once

#include <memory>
#include <wrl/client.h>

#include <DDSTextureLoader.h>
#include "WICTextureLoader.h"

#include "SpriteComponent.h"
#include "CommonStates.h"
#include "SpriteFont.h"
#include <vector>
#include <string>


struct FontComponent
{
        std::unique_ptr<DirectX::SpriteFont> mSpriteFont;
        std::string                          mTextDisplay;
        DirectX::SimpleMath::Vector2         mScreenPos;
        bool                                 enabled;
};

class UIManager
{
    public:
        static void Initialize();
        static void Update();
        static void Shutdown();

        void CreateSprite(SpriteComponent&           createComp,
                          ID3D11Device*             device,
                          ID3D11DeviceContext*      deviceContext);

		void CreateText(SpriteComponent& baseSprite, ID3D11Device* device, ID3D11DeviceContext* deviceContext);

        
    private:
        static UIManager* instance;

        std::unique_ptr<DirectX::SpriteBatch>	mSpriteBatch;
        std::unique_ptr<DirectX::CommonStates>  mStates;
        LPPOINT                                 mCursor;

        std::vector<SpriteComponent> mSprites;
        std::vector<FontComponent*>   mSpriteFonts;
};
