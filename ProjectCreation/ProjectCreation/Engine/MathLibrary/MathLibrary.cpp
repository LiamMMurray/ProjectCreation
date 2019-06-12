#include "MathLibrary.h"
#include <random>
using namespace DirectX;
using namespace std;

random_device         rd;
default_random_engine generator(rd());

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


XMMATRIX MathLibrary::LookAt(DirectX::XMVECTOR vPos, DirectX::XMVECTOR tPos, DirectX::XMVECTOR up)
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
        XMVECTOR length       = XMVector3Normalize(endPoint - startPoint);
        XMVECTOR targetVector = point - startPoint;
        XMVECTOR dotValue     = XMVector3Dot(length, targetVector);
        XMVECTOR distance     = XMVectorMultiply(length, dotValue);
        output                = startPoint + distance;
        XMVECTOR minv         = XMVectorMin(startPoint, endPoint);
        XMVECTOR maxv         = XMVectorMin(startPoint, endPoint);

        output = XMVectorMin(output, maxv);
        output = XMVectorMax(output, minv);

        return output;
}

DirectX::XMVECTOR MathLibrary::GetMidPointFromTwoVector(DirectX::XMVECTOR a, DirectX::XMVECTOR b)
{
        float x = (XMVectorGetX(a) + XMVectorGetX(b)) / 2;
        float y = (XMVectorGetY(a) + XMVectorGetY(b)) / 2;
        float z = (XMVectorGetZ(a) + XMVectorGetZ(b)) / 2;

        XMVECTOR output = XMVectorSet(x, y, z, 0.0f);
        return output;
}

float MathLibrary::CalulateDistance(DirectX::XMVECTOR a, DirectX::XMVECTOR b)
{
        float output;
        output = sqrtf(CalulateDistanceSq(a, b));
        return output;
}

float MathLibrary::CalulateDistanceSq(DirectX::XMVECTOR a, DirectX::XMVECTOR b)
{
        float    output;
        XMVECTOR temp = (a - b);
        temp          = XMVector3Dot(temp, temp);
        output        = XMVectorGetX(temp);
        return output;
}

float MathLibrary::CalulateDistanceIgnoreY(DirectX::XMVECTOR a, DirectX::XMVECTOR b)
{
        float output;
        output = sqrtf(CalulateDistanceSqIgnoreY(a, b));
        return output;
}

float MathLibrary::CalulateDistanceSqIgnoreY(DirectX::XMVECTOR a, DirectX::XMVECTOR b)
{
        float    output;
        XMVECTOR temp = (a - b);
        temp          = XMVectorSwizzle(temp, 0, 2, 2, 3);
        temp          = XMVector2Dot(temp, temp);
        output        = XMVectorGetX(temp);
        return output;
}

float MathLibrary::CalulateVectorLength(DirectX::XMVECTOR vector)
{
        return sqrtf(XMVectorGetX(vector) * XMVectorGetX(vector) + XMVectorGetY(vector) * XMVectorGetY(vector) +
                     XMVectorGetZ(vector) * XMVectorGetZ(vector));
}

float MathLibrary::VectorDotProduct(DirectX::XMVECTOR m, DirectX::XMVECTOR n)
{
        return ((XMVectorGetX(m) * XMVectorGetX(n)) + (XMVectorGetY(m) * XMVectorGetY(n)) +
                (XMVectorGetZ(m) * XMVectorGetZ(n)));
}

float MathLibrary::ManhattanDistance(Shapes::FAabb& a, Shapes::FAabb& b)
{
        float    output;
        XMVECTOR dis  = a.center - b.center;
        float    xDis = XMVectorGetX(dis);
        float    yDis = XMVectorGetY(dis);
        float    zDis = XMVectorGetZ(dis);
        output        = abs(xDis) + abs(yDis) + abs(zDis);

        return output;
}

float MathLibrary::GetRandomFloat()
{
        return RANDOMFLOAT;
}

float MathLibrary::GetRandomFloatInRange(float min, float max)
{
        float output = (rand() / (float)RAND_MAX * (max - min)) + min;
        return output;
}

double MathLibrary::GetRandomDouble()
{
        return RANDOMDOUBLE;
}

double MathLibrary::GetRandomDoubleInRange(double min, double max)
{
        double r      = (double)rand() / RAND_MAX;
        double output = min + r * (max - min);
        return output;
}

DirectX::XMVECTOR MathLibrary::GetRandomVector()
{
        return XMVectorSet(RANDOMFLOAT, RANDOMFLOAT, RANDOMFLOAT, 0.0f);
}

DirectX::XMVECTOR MathLibrary::GetRandomVectorInRange(float min, float max)
{
        float value = GetRandomFloatInRange(min, max);
        return XMVectorSet(value, value, value, 0.0f);
}

DirectX::XMVECTOR MathLibrary::GetRandomUnitVector()
{
        XMVECTOR output  = GetRandomVector();
        XMVECTOR unitVec = output / MathLibrary::VectorDotProduct(output, output);
        if (MathLibrary::CalulateVectorLength(unitVec) == 1)
        {
                return unitVec;
        }
}

float MathLibrary::MoveTowards(const float a, const float b, const float speed)
{
        float output;
        float dist = b - a;
        output     = a + std::copysignf(std::min(speed, fabsf(dist)), dist);
        return output;
}


// Smoothing functions from the "Fast and Funky 1D Nonlinear Transformations" GDC
//Powers 2 - 4 are faster than the Nth power
double MathLibrary::SmoothStart2(double x)
{
        return x * x;
}

double MathLibrary::SmoothStop2(double x)
{
        return 1 - ((1-x) * (1-x));
}

double MathLibrary::SmoothStart3(double x)
{
        return x * x * x;
}

double MathLibrary::SmoothStop3(double x)
{
        return 1 - ((1 - x) * (1 - x) * (1 - x));
}

double MathLibrary::SmoothStart4(double x)
{
        return x * x * x * x;
}

double MathLibrary::SmoothStop4(double x)
{
        return 1 - ((1 - x) * (1 - x) * (1 - x) * (1 - x));
}

double MathLibrary::SmoothStartN(double x, double power)
{
        return pow(x, power);
}

double MathLibrary::SmoothStopN(double x, double power)
{
        return 1 - pow(1 - x, power);
}

double MathLibrary::SmoothMix(double a, double b, double blend)
{
        return a + blend * (b - a);	
}

double MathLibrary::SmoothCrossfade(double a, double b, double time)
{
        return a + time * (b - a);
}

float MathLibrary::CalculateAngularDiameter(const DirectX::XMVECTOR& eye, const Shapes::FSphere& sphere)
{
        float distance = MathLibrary::CalulateDistance(eye, sphere.center);
        float diameter = sphere.radius * 2.0f;
        return 2.0f * asinf((diameter) / (2.0f * distance));
}

float MathLibrary::CalculateDistanceFromAngularDiameter(float angularDiameter, const Shapes::FSphere& sphere)
{
        float diameter = sphere.radius * 2.0f;
        return diameter / (2.0f*sinf(angularDiameter/2.0f));
}
