#include "SplineCluster.h"
#include "..//MathLibrary/MathLibrary.h"

using namespace DirectX;

SplineCluster::SplineCluster(const FGoodSpline& _spline) : spline(_spline)
{}

void SplineCluster::BakeNextPointOnSpline(DirectX::XMVECTOR& prev, DirectX::XMVECTOR& curr, DirectX::XMVECTOR& next)
{
        float baseLength = spline.GetLength();

        prev    = currPos;
        currPos = nextPos;
        curr    = currPos;

        nextPos     = spline.GetCurrentPoint();
        nextPos     = XMVector3Transform(nextPos, transform);
        float alpha = 1.0f - (cosf(spiralCount * XM_2PI * ((float)current / segments)) * 0.5f + 0.5f);
        nextPos     = XMVectorSetY(nextPos, maxHeight * alpha);
        spline.AdvanceDistance(baseLength / segments);
        current++;

        next = nextPos;
}
