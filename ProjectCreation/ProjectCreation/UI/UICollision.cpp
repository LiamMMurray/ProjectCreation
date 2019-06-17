#include "UICollision.h"

bool UI::PointInRect(const UIRect& rect, const DirectX::XMVECTOR& point)
{
        using namespace DirectX;
        XMVECTOR clampedPos = XMVectorMax(XMVectorMin(point, rect.center + rect.extents), rect.center - rect.extents);

        return XMVectorGetX(XMVector2Length(point - clampedPos)) < 0.0001f;
}

DirectX::XMVECTOR UI::ConvertScreenPosToNDC(DirectX::XMFLOAT2 in, const DirectX::XMFLOAT2& res)
{
        in.x = in.x / res.x - 0.5f;
        in.y = in.y / res.y - 0.5f;
        return DirectX::XMLoadFloat2(&in);
}
