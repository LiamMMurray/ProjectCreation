#pragma once
#include <Windows.h>
#include <vector>
#include <DirectXMath.h>
#include "splines.hpp"

struct FGoodSpline
{
    public:
        FGoodSpline(const std::vector<DirectX::XMVECTOR>& points);
        DirectX::XMVECTOR GetCurrentPoint();
        void         AdvanceDistance(float distance);
        inline float GetCurrentDistance()
        {
                return pointer;
        }

		inline float GetLength()
        {
                return length;
        }

		inline void ResetPointer()
		{
                pointer = 0.0f;
		}

    private:
        float                            pointer;
        float                            length;
        int                              m_Subdivisions;
        Spline<DirectX::XMVECTOR, float> m_Spline;
};