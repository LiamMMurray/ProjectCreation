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
        DirectX::XMVECTOR ConvertScreenPosToNDC(DirectX::XMFLOAT2 in, const DirectX::XMFLOAT2& res);
} // namespace UI