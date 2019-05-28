#pragma once
#include <DirectXMath.h>
#include <array>
namespace Shapes
{
        struct FSphere
        {
                DirectX::XMVECTOR center;
                float             radius;
                FSphere();
                FSphere(DirectX::XMVECTOR center, float radius);
        };

        struct FAabb
        {
                DirectX::XMVECTOR center;  // min
                DirectX::XMVECTOR extents; // max
                FAabb();
                FAabb(DirectX::XMVECTOR center, DirectX::XMVECTOR extents);
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
                FCapsule();
                FCapsule(DirectX::XMVECTOR startPoint, DirectX::XMVECTOR endPoint, float radius);
                float GetCapsuleDistance();
        };


        FSphere  CreateFSphere(DirectX::XMVECTOR center, float radius);
        FAabb    CreateFAabb(DirectX::XMVECTOR center, DirectX::XMVECTOR extents);
        FCapsule CreateFCapsule(DirectX::XMVECTOR startPoint, DirectX::XMVECTOR endPoint, float radius);

		extern int BvhIndex;
}; // namespace Shapes