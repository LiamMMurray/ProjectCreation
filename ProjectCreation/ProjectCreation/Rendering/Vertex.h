#pragma once
#include <DirectXMath.h>
struct FVertex
{
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT2 uv;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT3 tangent;
        DirectX::XMFLOAT3 binnormal;

        FVertex() = default;
        FVertex(DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 color);
        inline bool operator==(const FVertex& rhs) const
        {
                const FVertex& lhs = *this;

                bool out = (fabsf(lhs.position.x - rhs.position.x) < MathConstants::kEpsilon) &&
                           (fabsf(lhs.position.y - rhs.position.y) < MathConstants::kEpsilon) &&
                           (fabsf(lhs.position.z - rhs.position.z) < MathConstants::kEpsilon) &&
                           (fabsf(lhs.normal.x - rhs.normal.x) < MathConstants::kEpsilon) &&
                           (fabsf(lhs.normal.y - rhs.normal.y) < MathConstants::kEpsilon) &&
                           (fabsf(lhs.normal.z - rhs.normal.z) < MathConstants::kEpsilon) &&
                           (fabsf(lhs.uv.x - rhs.uv.x) < MathConstants::kEpsilon) &&
                           (fabsf(lhs.uv.y - rhs.uv.y) < MathConstants::kEpsilon) &&
                           (fabsf(lhs.color.x - rhs.color.x) < MathConstants::kEpsilon) &&
                           (fabsf(lhs.color.y - rhs.color.y) < MathConstants::kEpsilon) &&
                           (fabsf(lhs.color.z - rhs.color.z) < MathConstants::kEpsilon) &&
                           (fabsf(lhs.color.w - rhs.color.w) < MathConstants::kEpsilon);
                return out;
        };
};
struct SkinnedVertex
{};