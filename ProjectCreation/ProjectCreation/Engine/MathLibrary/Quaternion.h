#pragma once
#include "Vector.h"
struct FQuaternion
{
        DirectX::XMVECTOR rotation;

        FQuaternion();
        FQuaternion(DirectX::XMVECTOR vector);
        inline FQuaternion operator*(const FQuaternion& other) const
        {
                return FQuaternion(DirectX::XMQuaternionMultiply(rotation, other.rotation));
        }
        DirectX::XMVECTOR  GetForward() const;
        DirectX::XMVECTOR  GetRight() const;
        DirectX::XMVECTOR  GetUp() const;
        static FQuaternion RotateAxisAngle(DirectX::XMVECTOR& axis, float angle);
        static FQuaternion Lerp(const FQuaternion& lhs, const FQuaternion& rhs, float ratio);
        static FQuaternion LookAt(DirectX::XMVECTOR forward, DirectX::XMVECTOR up);
};