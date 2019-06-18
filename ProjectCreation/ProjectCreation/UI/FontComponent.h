#pragma once

#include <d3d11.h>
#include <string>
#include "../ECS/Component.h"
#include "SimpleMath.h"
#include "SpriteFont.h"

class FontComponent : public Component<FontComponent>
{
    public:
        std::unique_ptr<DirectX::SpriteFont> mSpriteFont;
        std::string                          mTextDisplay;
        DirectX::SimpleMath::Vector2         mScreenOffset;
        unsigned int                         mWidth;
        unsigned int                         mHeight;
        DirectX::XMVECTOR                    mOrigin;

        float mScaleX;
        float mScaleY;

        ID3D11ShaderResourceView* mTexture;

        bool mEnabled;
        int  mID;

        void SetPosition(float x, float y);

        FontComponent()                     = default;
        FontComponent(const FontComponent&) = delete;
        FontComponent(FontComponent&&) = default;
        FontComponent& operator=(const FontComponent&) = delete;
};
