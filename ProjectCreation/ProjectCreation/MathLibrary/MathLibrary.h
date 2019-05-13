#pragma once
#include <math.h>
#include <stdint.h>
#include <algorithm>
#include "../Rendering/Vertex.h"
#include "Transform.h"
class MathLibrary
{
    public:
        // Common Math Function
        float RandomFloatInRange(float min, float max);
        template <typename T>
        T Warprange(T val, T min, T max)
        {
                return val - (max - min) * floor(val / (max - min));
        }
        // Matrix Function
        static void              OrthoNormalize(DirectX::XMVECTOR normal, DirectX::XMVECTOR& tangent);
        DirectX::XMMATRIX LookAt(DirectX::XMVECTOR vPos, DirectX::XMVECTOR tPos, DirectX::XMVECTOR up);
        void              TurnTo(DirectX::XMMATRIX& matrix, DirectX::XMVECTOR targetPosition, float speed);
		//Vector Function
        DirectX::XMVECTOR getClosestPointFromLine(DirectX::XMVECTOR startPoint, DirectX::XMVECTOR endPoint, DirectX::XMVECTOR point);
};