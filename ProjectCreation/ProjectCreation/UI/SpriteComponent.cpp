#include "SpriteComponent.h"

void SpriteComponent::MakeRectangle()
{
        mRectangle.top  = 0;
        mRectangle.left = mOrigin.x;

        mRectangle.bottom = 0 + mHeight;
        mRectangle.right  = mOrigin.x + mWidth;
}

void SpriteComponent::SetPosition(float x, float y)
{
        mScreenPos.x = x;
        mScreenPos.y = y;
}

void SpriteComponent::TransformPosition(float x, float y)
{

}

void SpriteComponent::RotateSprite(float deltaTime)
{
        //float time = float(m_timer.GetTotalSeconds
        
        //m_spriteBatch->Begin();
        
        //m_spriteBatch->Draw(m_texture.Get(), rotateComp.m_screenPos, nullptr, Colors::White, cosf(time) * 4.f, m_origin);
        
        //m_spriteBatch->End();
}