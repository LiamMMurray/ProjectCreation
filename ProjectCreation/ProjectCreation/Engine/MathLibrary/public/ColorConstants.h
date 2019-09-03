#pragma once

#include <DirectXMath.h>

namespace ColorConstants
{
        constexpr DirectX::XMFLOAT4 White   = {1.0f, 1.0f, 1.0f, 1.0f};
        constexpr DirectX::XMFLOAT4 Black   = {0.0f, 0.0f, 0.0f, 1.0f};
        constexpr DirectX::XMFLOAT4 Red     = {1.0f, 0.0f, 0.0f, 1.0f};
        constexpr DirectX::XMFLOAT4 Green   = {0.0f, 1.0f, 0.0f, 1.0f};
        constexpr DirectX::XMFLOAT4 Blue    = {0.0f, 0.0f, 1.0f, 1.0f};
        constexpr DirectX::XMFLOAT4 Cyan    = {0.0f, 1.0f, 1.0f, 1.0f};
        constexpr DirectX::XMFLOAT4 Magenta = {1.0f, 0.0f, 1.0f, 1.0f};
        constexpr DirectX::XMFLOAT4 Yellow  = {1.0f, 1.0f, 0.0f, 1.0f};
} // namespace ColorConstants