#include "SplineCluster.h"
#include <TerrainManager.h>
#include "..//MathLibrary/MathLibrary.h"

using namespace DirectX;

SplineCluster::SplineCluster(const FGoodSpline& _spline) : spline(_spline)
{}


void SplineCluster::BakeStartAndEnd()
{
        start           = spline.GetPointAtTime(0.0f);
        start           = XMVector3Transform(start, transform);
        XMVECTOR start2 = spline.GetPointAtTime(0.01f);
        start2          = XMVector3Transform(start2, transform);

        end           = spline.GetPointAtTime(1.0f);
        end           = XMVector3Transform(end, transform);
        XMVECTOR end2 = spline.GetPointAtTime(0.99f);
        end2          = XMVector3Transform(end2, transform);

        start -= XMVector3Normalize(start - start2) * 5.0f;
        end += XMVector3Normalize(end - end2) * 5.0f;
}

void SplineCluster::BakeNextPointOnSpline(DirectX::XMVECTOR& prev, DirectX::XMVECTOR& curr, DirectX::XMVECTOR& next)
{
        float baseLength = spline.GetLength();

        prev    = currPos;
        currPos = nextPos;
        curr    = currPos;

        nextPos     = spline.GetCurrentPoint();
        nextPos     = XMVector3Transform(nextPos, transform);
        float alpha = 1.0f - (cosf(spiralCount * XM_2PI * ((float)current / segments)) * 0.5f + 0.5f);
        nextPos     = XMVectorSetY(nextPos, maxHeight * alpha + TerrainManager::Get()->GetGroundOffset());
        spline.AdvanceDistance(baseLength / segments);
        current++;

        next = nextPos;
}
