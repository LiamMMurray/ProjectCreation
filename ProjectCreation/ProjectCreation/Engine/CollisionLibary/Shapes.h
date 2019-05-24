#pragma once
#include<DirectXMath.h>
#include<array>
namespace Shapes
{
        struct FSphere
        {
                DirectX::XMVECTOR center;
                float             radius;
        };

        struct FAabb
        {
                DirectX::XMVECTOR center;  // min
                DirectX::XMVECTOR extents; // max
        };

        struct FPlane
        {
                DirectX::XMVECTOR normal;
                float             offset;
        };

        using Frustum = std::array<FPlane, 6>;

        struct FCapsule
        {
                DirectX::XMVECTOR startPoint;
                DirectX::XMVECTOR endPoint;
                float             radius;

                float GetCapsuleDistance();
        };
};