#pragma once
#include <DirectXMath.h>
#include <vector>
#include <ECSTypes.h>
#include <FGoodSpline.h>
#include <Transform.h>

struct SplinePoint
{
        DirectX::XMVECTOR pos;
        unsigned int      color;
};

struct SplineCluster
{
        SplineCluster(const FGoodSpline&);

        std::vector<ComponentHandle> splineComponentList;
        std::vector<SplinePoint>     cachedPoints;
        FGoodSpline                  spline;
        DirectX::XMMATRIX            transform;
        DirectX::XMVECTOR            currPos;
        DirectX::XMVECTOR            nextPos;
        DirectX::XMVECTOR            originalWorldOffset;

        DirectX::XMVECTOR start;
        DirectX::XMVECTOR end;

        float spawnTimer;
        float spawnCD;
        int   current;
        int   deleteIndex      = 0;
        int   deleteSeparation = 0;
        int   segments;
        int   spiralCount;
        float maxHeight;
        int   color;
        int   targetColor;
        int   clearTailCurrent;
        int   clearHeadCurrent;
        bool  shouldDestroy;

		void BakeStartAndEnd();
        void BakeNextPointOnSpline(DirectX::XMVECTOR& prev, DirectX::XMVECTOR& curr, DirectX::XMVECTOR& next);
};