#include "Quaternion.h"
#include "MathLibrary.h"
using namespace DirectX;

FQuaternion::FQuaternion()
{
        data = XMQuaternionIdentity();
}

FQuaternion::FQuaternion(DirectX::XMVECTOR vector)
{
        data = vector;
}

DirectX::XMVECTOR FQuaternion::GetForward() const
{
        return XMVector3Rotate(VectorConstants::Forward, data);
}

DirectX::XMVECTOR FQuaternion::GetRight() const
{
        return XMVector3Rotate(VectorConstants::Right, data);
}

DirectX::XMVECTOR FQuaternion::GetUp() const
{
        return XMVector3Rotate(VectorConstants::Up, data);
}

FQuaternion FQuaternion::RotateAxisAngle(DirectX::XMVECTOR& axis, float angle)
{
        return FQuaternion(XMQuaternionRotationAxis(axis, XMConvertToRadians(angle)));
}

FQuaternion FQuaternion::Lerp(const FQuaternion& lhs, const FQuaternion& rhs, float ratio)
{
        return FQuaternion(XMQuaternionSlerp(lhs.data, rhs.data, ratio));
}

FQuaternion FQuaternion::LookAt(DirectX::XMVECTOR forward, DirectX::XMVECTOR up)
{
        MathLibrary::OrthoNormalize(forward, up);

        XMVECTOR    right = XMVector3Cross(up, forward);
        FQuaternion output;
        output.data.m128_f32[3] = sqrtf(1.0f + right.m128_f32[0] + up.m128_f32[1] + forward.m128_f32[2]) * 0.5f;

        float reciprocal = 1.0f / (4.0f * output.data.m128_f32[3]);

        output.data.m128_f32[0] = (up.m128_f32[2] - forward.m128_f32[1]) * reciprocal;
        output.data.m128_f32[1] = (forward.m128_f32[0] - right.m128_f32[2]) * reciprocal;
        output.data.m128_f32[2] = (right.m128_f32[1] - up.m128_f32[0]) * reciprocal;

        return output;
}

FQuaternion FQuaternion::FromEulerAngles(DirectX::XMFLOAT3 val)
{
        return FromEulerAngles(val.x, val.y, val.z);
}

FQuaternion FQuaternion::FromEulerAngles(float x, float y, float z)
{
        return XMQuaternionRotationRollPitchYaw(x, y, z);
}

DirectX::XMFLOAT3 FQuaternion::ToEulerAngles()
{
        XMFLOAT3 output;
        XMFLOAT4 quat;
        XMStoreFloat4(&quat, data);

        double sqx = (double)quat.x * quat.x;
        double sqy = (double)quat.y * quat.y;
        double sqz = (double)quat.z * quat.z;
        double sqw = (double)quat.w * quat.w;

        output.x = (float)atan2(2.0 * ((double)quat.y * quat.z + (double)quat.x * quat.w), (-sqx - sqy + sqz + sqw));
        output.y = (float)asin(-2.0 * ((double)quat.x * quat.z - (double)quat.y * quat.w));
        output.z = (float)atan2(2.0 * ((double)quat.x * quat.y + (double)quat.z * quat.w), (sqx - sqy - sqz + sqw));

        return output;
}
