#include "FontComponent.h"

void FontComponent::MakeRectangle()
{
        mRectangle.top  = 0;
        mRectangle.left = mScreenOffset.x;

        mRectangle.bottom = 0 + mHeight;
        mRectangle.right  = mScreenOffset.x + mWidth;
}

void FontComponent::SetPosition(float x, float y)
{
        mScreenOffset.x = x;
        mScreenOffset.y = y;
}
