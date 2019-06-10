#include "FontComponent.h"

void FontComponent::MakeRectangle()
{
        mRectangle.top  = 0;
        mRectangle.left = mScreenPos.x;

        mRectangle.bottom = 0 + mHeight;
        mRectangle.right  = mScreenPos.x + mWidth;
}

void FontComponent::SetPosition(float x, float y)
{
        mScreenPos.x = x;
        mScreenPos.y = y;
}
