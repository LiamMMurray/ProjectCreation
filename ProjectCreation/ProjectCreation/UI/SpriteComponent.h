#pragma once

#include <d3d11.h>
#include "SimpleMath.h"
#include "SpriteFont.h"

class SpriteComponent
{
    public:
        DirectX::SimpleMath::Vector2          mScreenPos;
        DirectX::SimpleMath::Vector2          mOrigin;
        ID3D11ShaderResourceView*             mTexture;
        RECT							      mRectangle;
        bool                                  enabled;
        unsigned int                          id;

		void Initialize();
        void SetPosition(float x, float y);
        void TransformPosition(float x, float y);
		void ScaleSprite(float scale);
		void RotateSprite(float degree);
};