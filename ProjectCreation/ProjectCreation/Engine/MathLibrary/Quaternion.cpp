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

DirectX::XMVECTOR FQuaternion::GetForward2D() const
{
        XMVECTOR fw       = GetForward();
        float    distance = MathLibrary::CalulateDistance(fw, VectorConstants::Up);

        if (distance < 0.001f)
                return -GetUp();

        return XMVector3Normalize(XMVectorSetY(fw, 0.0f));
}

DirectX::XMVECTOR FQuaternion::GetRight() const
{
        return XMVector3Rotate(VectorConstants::Right, data);
}

DirectX::XMVECTOR FQuaternion::GetUp() const
{
        return XMVector3Rotate(VectorConstants::Up, data);
}

FQuaternion FQuaternion::RotateAxisAngle(const DirectX::XMVECTOR& axis, float angle)
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

FQuaternion FQuaternion::LookAtWithRoll(DirectX::XMVECTOR sourcePoint, DirectX::XMVECTOR destPoint)
{
        XMVECTOR forwardVector = XMVector3Normalize(destPoint - sourcePoint);

        XMVECTOR vRotAxis = XMVector3Cross(VectorConstants::Forward, forwardVector);
        float    dot      = MathLibrary::VectorDotProduct(VectorConstants::Forward, forwardVector);

        XMFLOAT3 rotAxis;

        XMStoreFloat3(&rotAxis, vRotAxis);

        XMFLOAT4 q;
        q.x = rotAxis.x;
        q.y = rotAxis.y;
        q.z = rotAxis.z;
        q.w = dot + 1;

        return XMQuaternionNormalize(XMLoadFloat4(&q));
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

        constexpr float singularityThreshold = 0.4999995f;

        const float singularityTest = quat.x * quat.y + quat.z * quat.w;

        float yawY = 2.0f * (quat.w * quat.y + quat.x * quat.z);
        float yawX = float(1.0f - 2.0f * (sqz + sqy));


        if (singularityTest < -singularityThreshold)
        {
                output.x = -XM_PIDIV2;
                output.y = atan2f(yawY, yawX);
                output.z = MathLibrary::Warprange(-output.y - (2.0f * atan2f(quat.x, quat.w)), -XM_PI, XM_PI);
        }
        else if (singularityTest > singularityThreshold)
        {
                output.x = XM_PIDIV2;
                output.y = atan2f(yawY, yawX);
                output.z = MathLibrary::Warprange(output.y - (2.0f * atan2f(quat.x, quat.w)), -XM_PI, XM_PI);
        }
        else
        {
                output.x = asinf(2.0f * (singularityTest));
                output.y = atan2f(yawY, yawX);
                output.z = atan2f(-2.0f * (quat.w * quat.x + quat.z * quat.y), float(1.f - 2.f * (sqx + sqz)));
        }

        return output;
}
