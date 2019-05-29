#pragma once

#include <wrl/client.h>
#include <memory>

#include <DDSTextureLoader.h>
#include "WICTextureLoader.h"

#include <string>
#include <vector>
#include "CommonStates.h"
#include "SpriteComponent.h"
#include "FontComponent.h"

class UIManager
{
    public:
        static void Initialize();
        static void Update(float deltaTime);
        static void Shutdown();

        void AddSprite(ID3D11Device*        device,
                       ID3D11DeviceContext* deviceContext,
                       const wchar_t*       FileName,
                       float                PositionX,
                       float                PositionY);
        void RemoveSprite(int id);

        void AddText(ID3D11Device*        device,
                     ID3D11DeviceContext* deviceContext,
                     const wchar_t*       FileName,
                     std::string          TextDisplay,
                     float                PositionX,
                     float                PositionY);
        void RemoveText(int id);
        void ScaleSprite(float deltaTime, SpriteComponent sprite);

        static UIManager* instance;
    private:

        std::unique_ptr<DirectX::SpriteBatch>  mSpriteBatch;
        std::unique_ptr<DirectX::CommonStates> mStates;
       POINT                                  mCursor;

        std::vector<SpriteComponent> mSprites;
        std::vector<FontComponent*>  mSpriteFonts;
};
