#include "Transform.h"
using namespace DirectX;
FTransform::FTransform()
{}

DirectX::XMMATRIX FTransform::CreateMatrix() const
{
        return XMMatrixScalingFromVector(scale) * XMMatrixRotationQuaternion(rotation.rotation) *
               XMMatrixTranslationFromVector(translation);
}

void FTransform::SetMatrix(const DirectX::XMMATRIX& matrix)
{

}

DirectX::XMVECTOR FTransform::GetForward()
{
        return DirectX::XMVECTOR();
}

DirectX::XMVECTOR FTransform::GetRight()
{
        return DirectX::XMVECTOR();
}

DirectX::XMVECTOR FTransform::GetUp()
{
        return DirectX::XMVECTOR();
}

void FTransform::RotateAxisAngle(DirectX::XMVECTOR& axis, float angle)
{}
