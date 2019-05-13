#include "Quaternion.h"
using namespace DirectX;

FQuaternion::FQuaternion()
{
        rotation = XMQuaternionIdentity();
}

FQuaternion::FQuaternion(DirectX::XMVECTOR vector)
{
        rotation = vector;
}

DirectX::XMVECTOR FQuaternion::GetForward() const
{
        return XMVector3Rotate(VectorConstants::Forward, rotation);
}

DirectX::XMVECTOR FQuaternion::GetRight() const
{
        return XMVector3Rotate(VectorConstants::Right, rotation);
}

DirectX::XMVECTOR FQuaternion::GetUp() const
{
        return XMVector3Rotate(VectorConstants::Up, rotation);
}

FQuaternion FQuaternion::RotateAxisAngle(DirectX::XMVECTOR& axis, float angle)
{
        return FQuaternion();
}
