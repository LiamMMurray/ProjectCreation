#pragma once
#include "Vector.h"
struct FQuaternion
{
        DirectX::XMVECTOR data;

        FQuaternion();
        FQuaternion(DirectX::XMVECTOR vector);
        inline FQuaternion operator*(const FQuaternion& other) const
        {
                return FQuaternion(DirectX::XMQuaternionMultiply(data, other.data));
        }
        DirectX::XMVECTOR  GetForward() const;
        DirectX::XMVECTOR  GetForward2D() const;
        DirectX::XMVECTOR  GetRight() const;
        DirectX::XMVECTOR  GetUp() const;
        static FQuaternion RotateAxisAngle(const DirectX::XMVECTOR& axis, float angle);
        static FQuaternion Lerp(const FQuaternion& lhs, const FQuaternion& rhs, float ratio);
        static FQuaternion LookAt(DirectX::XMVECTOR forward, DirectX::XMVECTOR up = DirectX::XMVECTORF32{0.0f, 1.0f, 0.0f, 0.0f});

        static FQuaternion FromEulerAngles(DirectX::XMFLOAT3 val);
        static FQuaternion FromEulerAngles(float x, float y, float z);

		static FQuaternion LookAtWithRoll(DirectX::XMVECTOR sourcePoint, DirectX::XMVECTOR destPoint);

        DirectX::XMFLOAT3  ToEulerAngles();
};