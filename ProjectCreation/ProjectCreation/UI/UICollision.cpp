#include "UICollision.h"

bool UI::PointInRect(const UIRect& rect, const DirectX::XMVECTOR& point)
{
        using namespace DirectX;
        XMVECTOR clampedPos = XMVectorMax(XMVectorMin(point, rect.center + rect.extents), rect.center - rect.extents);

        return XMVectorGetX(XMVector2Length(point - clampedPos)) < 0.0001f;
}

DirectX::XMVECTOR UI::ConvertScreenPosToNDC(DirectX::XMFLOAT2 in, const DirectX::XMFLOAT2& res, const DirectX::XMUINT4& rect)
{
        float centerX = 0.5f * (rect.y + rect.w);
        float centerY = 0.5f * (rect.x + rect.z);

        float width  = float(rect.w - rect.y);
        float height = float(rect.z - rect.x);

        in.x -= centerX;
        in.y -= centerY;

        in.x = 2.0f * (in.x / width);
        in.y = 2.0f * (in.y / height);
        return DirectX::XMLoadFloat2(&in);
}
