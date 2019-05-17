#pragma once
#include <math.h>
#include <stdint.h>
#undef max
#undef min

#include <algorithm>
#include "../../Rendering/Vertex.h"
#include "Transform.h"
class MathLibrary
{
    public:
        // Common Math Function
        float RandomFloatInRange(float min, float max);
        template <typename T>
        static T Warprange(T val, T min, T max)
        {
                return val - (max - min) * floor(val / (max - min));
        }
        // Matrix Function
        static void              OrthoNormalize(DirectX::XMVECTOR normal, DirectX::XMVECTOR& tangent);
        static DirectX::XMMATRIX LookAt(DirectX::XMVECTOR vPos, DirectX::XMVECTOR tPos, DirectX::XMVECTOR up);
        static void              TurnTo(DirectX::XMMATRIX& matrix, DirectX::XMVECTOR targetPosition, float speed);

        // Vector Function
        static DirectX::XMVECTOR GetClosestPointFromLine(DirectX::XMVECTOR startPoint,
                                                         DirectX::XMVECTOR endPoint,
                                                         DirectX::XMVECTOR point);
        static float             CalulateDistance(DirectX::XMVECTOR a, DirectX::XMVECTOR b);
        static float             CalulateDistanceSq(DirectX::XMVECTOR a, DirectX::XMVECTOR b);
        static float             CalulateVectorLength(DirectX::XMVECTOR vector);
        static float             VectorDotProduct(DirectX::XMVECTOR m, DirectX::XMVECTOR n);
        static FQuaternion       AverageQuaternion(DirectX::XMVECTOR& cumulative,
                                                   FQuaternion        newRotation,
                                                   FQuaternion        firstRotation,
                                                   int                addAmount);

        static FQuaternion		QuaternionNormalize(FQuaternion quaternion);
        static DirectX::XMVECTOR AveragePositionFromVector(DirectX::XMVECTOR vector);
        template <typename T>
        static T lerp(T v0, T v1, T t)
        {
                return fma(t, v1, fma(-t, v0, v0));
        }
		
        template <typename T>
        static T clamp(const T& n, const T& lower, const T& upper)
        {
                return std::max(lower, std::min(n, upper));
        }
};