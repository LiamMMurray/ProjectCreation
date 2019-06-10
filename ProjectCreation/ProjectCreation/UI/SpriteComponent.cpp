#include "SpriteComponent.h"

void SpriteComponent::MakeRectangle()
{
        mRectangle.top  = mScreenPos.y -( mOrigin.y * mScaleY);
        mRectangle.left = mScreenPos.x -( mOrigin.x * mScaleX);
									   
        mRectangle.bottom = mScreenPos.y + (mOrigin.y * mScaleY);
        mRectangle.right  = mScreenPos.x + (mOrigin.x * mScaleX);
}

void SpriteComponent::Scale()
{
        mWidth = (float)mWidth * mScaleX;
        mHeight = (float)mHeight* mScaleY;

        mRectangle.top  = (mScreenPos.y - mOrigin.y) * mScaleY;
        mRectangle.left = (mScreenPos.x - mOrigin.x) * mScaleX;
        mRectangle.bottom = (mScreenPos.y + mOrigin.y) * mScaleY;
        mRectangle.right  = (mScreenPos.x + mOrigin.x) * mScaleX;
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