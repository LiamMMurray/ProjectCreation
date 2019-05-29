#pragma once

#include <d3d11.h>
#include "../ECS/Component.h"
#include "SimpleMath.h"
#include "SpriteFont.h"

class SpriteComponent : public Component<SpriteComponent>
{
    public:
        DirectX::SimpleMath::Vector2          mScreenPos;
        DirectX::SimpleMath::Vector2          mOrigin;
        unsigned int                          mHeight;
        unsigned int                          mWidth;
        ID3D11ShaderResourceView*             mTexture;
        RECT							      mRectangle;
        bool                                  mEnabled;
        unsigned int                          mId;

        void MakeRectangle();
        void SetPosition(float x, float y);
        void TransformPosition(float x, float y);
		void ScaleSprite(float scale);
		void RotateSprite(float degree);
};