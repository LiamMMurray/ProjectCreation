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
        rotation = rotation.RotateAxisAngle(axis, angle);
}
