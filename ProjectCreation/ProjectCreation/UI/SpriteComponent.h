#pragma once

#include <d3d11.h>
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"

struct Font
{
        //long thing;
};

class SpriteComponent
{
    public:
        DirectX::SimpleMath::Vector2          m_screenPos;
        DirectX::SimpleMath::Vector2          m_origin;
        ID3D11ShaderResourceView*             m_texture;

		void ScaleSprite();
		void RotateSprite();
};