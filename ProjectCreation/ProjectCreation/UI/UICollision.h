#pragma once
#include <DirectXMath.h>


namespace UI
{
        struct UIRect
        {
                DirectX::XMVECTOR center;
                DirectX::XMVECTOR extents;
        };

        bool PointInRect(const UIRect& rect, const DirectX::XMVECTOR& point);
} // namespace UI