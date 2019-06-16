#include "SpriteComponent.h"

void SpriteComponent::Scale()
{
        mWidth  = (float)mWidth * mScaleX;
        mHeight = (float)mHeight * mScaleY;
}

void SpriteComponent::SetPosition(float x, float y)
{
        mScreenOffset.x = x;
        mScreenOffset.y = y;
}