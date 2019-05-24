#include "Transform.h"
using namespace DirectX;
FTransform::FTransform()
{
        translation = XMVectorSet(0.0f, 0.0f, 0.0f,1.0f);
        rotation    = FQuaternion();
        scale       = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
}

DirectX::XMMATRIX FTransform::CreateMatrix() const
{
        return XMMatrixScalingFromVector(scale) * XMMatrixRotationQuaternion(rotation.rotation) *
               XMMatrixTranslationFromVector(translation);
}

void FTransform::SetMatrix(const DirectX::XMMATRIX& matrix)
{
        XMMatrixDecompose(&scale, &rotation.rotation, &translation, matrix);
}

DirectX::XMVECTOR FTransform::GetForward()
{
        return rotation.GetForward();
}

DirectX::XMVECTOR FTransform::GetRight()
{
        return rotation.GetRight();
}

DirectX::XMVECTOR FTransform::GetUp()
{
        return rotation.GetUp();
}

void FTransform::RotateAxisAngle(DirectX::XMVECTOR& axis, float angle)
{
        rotation.RotateAxisAngle(axis, angle);
}

FTransform FTransform::Lerp(const FTransform& lhs, const FTransform& rhs, float ratio)
{
        FTransform output;
        output.translation = XMVectorLerp(lhs.translation, rhs.translation, ratio);
        output.rotation    = FQuaternion::Lerp(lhs.rotation, rhs.rotation, ratio);
        output.scale       = XMVectorLerp(lhs.scale, rhs.scale, ratio);
        return output;
}