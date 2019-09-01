#pragma once

#include <d3d11.h>
#include <string>
#include <Component.h>
#include "SimpleMath.h"
#include "SpriteFont.h"

class FontComponent
{
    public:
        int                          mFontType;
        std::string                  mTextDisplay;
        DirectX::SimpleMath::Vector2 mScreenOffset;
        unsigned int                 mWidth;
        unsigned int                 mHeight;
        DirectX::XMVECTOR            mOrigin;
        float                        mScaleX;
        float                        mScaleY;

        DirectX::XMVECTOR currColor    = {1, 1, 1, 1};
        DirectX::XMVECTOR desiredColor = {1, 1, 1, 1};


        ID3D11ShaderResourceView* mTexture;

        bool mEnabled;
        int  mID;

        void SetPosition(float x, float y);

        virtual ~FontComponent()            = default;
        FontComponent()                     = default;
        FontComponent(const FontComponent&) = delete;
        FontComponent(FontComponent&&)      = default;
        FontComponent& operator=(const FontComponent&) = delete;
};
