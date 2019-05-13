#include "MathLibrary.h"
#include <random>
using namespace DirectX;
using namespace std;

random_device                rd;
default_random_engine        generator(rd());
uniform_real_distribution<float> distribution(0.0f, 1.0f);

float MathLibrary::RandomFloatInRange(float min, float max)
{
        return distribution(generator) * (max - min) + min;
}

void MathLibrary::OrthoNormalize(DirectX::XMVECTOR normal, DirectX::XMVECTOR& tangent)
{
        normal = XMVector3Normalize(normal);
        XMVECTOR proj;
        proj = XMVectorMultiply(normal, XMVector3Dot(tangent, normal));

        tangent = XMVector3Normalize(tangent - proj);
}


XMMATRIX MathLibrary::LookAt( DirectX::XMVECTOR vPos, DirectX::XMVECTOR tPos, DirectX::XMVECTOR up)
{
        XMMATRIX output;
        XMVECTOR zValue = tPos - vPos;
        zValue          = XMVector3Normalize(zValue);

        XMVECTOR xValue = XMVector3Cross(up, zValue);
        xValue          = XMVector3Normalize(xValue);

        XMVECTOR yValue = XMVector3Cross(zValue, xValue);
        yValue          = XMVector3Normalize(yValue);

        output.r[0] = xValue;
        output.r[1] = yValue;
        output.r[2] = zValue;
        output.r[3] = vPos;

		return output;

}

void MathLibrary::TurnTo(DirectX::XMMATRIX& matrix, DirectX::XMVECTOR targetPosition, float speed)
{
        XMVECTOR pos = matrix.r[3];
        matrix.r[3]  = XMVectorZero();

        XMVECTOR current = XMQuaternionRotationMatrix(matrix);
        XMVECTOR target  = XMQuaternionRotationMatrix(LookAt(pos, targetPosition, XMVectorSet(0, 1, 0, 0)));

        XMVECTOR blend = XMQuaternionSlerp(current, target, speed);

        XMMATRIX output = XMMatrixRotationQuaternion(blend);
        output.r[3]     = pos;

		matrix = output;


}

DirectX::XMVECTOR MathLibrary::GetClosestPointFromLine(DirectX::XMVECTOR startPoint,
                                                       DirectX::XMVECTOR endPoint,
                                                       DirectX::XMVECTOR point)
{
        XMVECTOR output;
        XMVECTOR length =XMVector3Normalize(endPoint - startPoint);
        XMVECTOR targetVector = point - startPoint;
        XMVECTOR dotValue     = XMVector3Dot(length, targetVector);
        XMVECTOR distance     = XMVectorMultiply(length,dotValue);
        output                = startPoint + distance;

        return output;
}
