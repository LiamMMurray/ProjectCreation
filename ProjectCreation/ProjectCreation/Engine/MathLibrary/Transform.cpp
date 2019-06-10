#include "Transform.h"
#include "MathLibrary.h"

using namespace DirectX;
FTransform::FTransform()
{
        translation = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
        rotation    = FQuaternion();
        scale       = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
}

FTransform::FTransform(DirectX::XMVECTOR translationVec, DirectX::XMVECTOR scaleVec, FQuaternion Fquaternion)
{
        translation = translationVec;
        scale       = scaleVec;
        rotation    = Fquaternion;
}

DirectX::XMMATRIX FTransform::CreateMatrix() const
{
        return XMMatrixScalingFromVector(scale) * XMMatrixRotationQuaternion(rotation.data) *
               XMMatrixTranslationFromVector(translation);
}

void FTransform::SetMatrix(const DirectX::XMMATRIX& matrix)
{
        XMMatrixDecompose(&scale, &rotation.data, &translation, matrix);
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

float FTransform::GetRadius()
{
        XMFLOAT3 vec;

		XMStoreFloat3(&vec, scale);

        return std::max(vec.x, std::max(vec.y, vec.z));
}
