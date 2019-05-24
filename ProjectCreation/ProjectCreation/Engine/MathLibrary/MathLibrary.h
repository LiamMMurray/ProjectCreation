#pragma once
#include <math.h>
#include <stdint.h>
#undef max
#undef min

#include <algorithm>
#include "../../Rendering/Vertex.h"
#include "Transform.h"
#include"../CollisionLibary/CollisionComponent.h"
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
        static float             ManhattanDistance(Shapes::FAabb& a, Shapes::FAabb& b);
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

        template <typename T>
        static void NormalizeArray(size_t size, T* data)
        {
                T sum = 0.0f;
                for (int i = 0; i < size; ++i)
                        sum += data[i];

                for (int i = 0; i < size; ++i)
                        data[i] /= sum;
        }

        template <typename T>
        static void NormalizeValues(T& a, T& b)
        {
                if (a == 0 && b == 0)
                {
                        a = 0.0f;
                        b = 0.0f;
                        return;
                }

                T sum = a + b;
                a /= sum;
                b /= sum;
        }
};