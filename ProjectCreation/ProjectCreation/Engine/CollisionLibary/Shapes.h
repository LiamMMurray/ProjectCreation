#pragma once
#include <DirectXMath.h>
#include <array>
namespace Shapes
{
        enum ECollisionObjectTypes
        {
                Sphere = 0,
                Aabb,   // = 1
                Plane,  // = 2
                Capsule // = 3
        };

        struct FCollisionShape
        {
            public:
                virtual ECollisionObjectTypes GetID() const = 0;
        };


        struct FSphere : FCollisionShape
        {
                DirectX::XMVECTOR center;
                float             radius;
                FSphere();
                FSphere(DirectX::XMVECTOR center, float radius);

                virtual ECollisionObjectTypes GetID() const override;
        };

        struct FAabb : FCollisionShape
        {
                DirectX::XMVECTOR center;  // min
                DirectX::XMVECTOR extents; // max
                FAabb();
                FAabb(DirectX::XMVECTOR center, DirectX::XMVECTOR extents);

                virtual ECollisionObjectTypes GetID() const override;
        };

        struct FPlane : FCollisionShape
        {
                DirectX::XMVECTOR normal;
                float             offset;

                virtual ECollisionObjectTypes GetID() const override;
        };

        using Frustum = std::array<FPlane, 6>;

        struct FCapsule : FCollisionShape
        {
                DirectX::XMVECTOR startPoint;
                DirectX::XMVECTOR endPoint;
                float             radius;
                FCapsule();
                FCapsule(DirectX::XMVECTOR startPoint, DirectX::XMVECTOR endPoint, float radius);
                float GetCapsuleDistance();

                virtual ECollisionObjectTypes GetID() const override;
        };

		struct FCircle
		{
                DirectX::XMFLOAT2 center;
                float             radius;

		};
        FSphere  CreateFSphere(DirectX::XMVECTOR center, float radius);
        FAabb    CreateFAabb(DirectX::XMVECTOR center, DirectX::XMVECTOR extents);
        FCapsule CreateFCapsule(DirectX::XMVECTOR startPoint, DirectX::XMVECTOR endPoint, float radius);
}; // namespace Shapes