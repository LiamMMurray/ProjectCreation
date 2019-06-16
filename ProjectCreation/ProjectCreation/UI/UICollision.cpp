#include "UICollision.h"

bool UI::PointInRect(const UIRect& rect, const DirectX::XMVECTOR& point)
{
        using namespace DirectX;
        XMVECTOR clampedPos = XMVectorMax(XMVectorMin(point, rect.center + rect.extents), rect.center - rect.extents);

        return XMVectorGetX(XMVector2Length(point - clampedPos)) < 0.0001f;
}
