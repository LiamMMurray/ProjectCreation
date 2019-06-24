#pragma once
#include <DirectXMath.h>
#include <vector>
#include "..//..//ECS/ECSTypes.h"
#include "..//MathLibrary/Splines/FGoodSpline.h"
#include "..//MathLibrary/Transform.h"
struct SplineCluster
{
        SplineCluster(const FGoodSpline&);

        std::vector<ComponentHandle> splineComponentList;
        FGoodSpline                  spline;
        DirectX::XMMATRIX            transform;
        DirectX::XMVECTOR            currPos;
        DirectX::XMVECTOR            nextPos;
        float                        spawnTimer;
        float                        spawnCD;
        int                          current;
        int                          segments;
        int                          spiralCount;
        float                        maxHeight;
        int                          color;
        int                          clearTailCurrent;
        int                          clearHeadCurrent;
        bool                         shouldDestroy;

        void BakeNextPointOnSpline(DirectX::XMVECTOR& prev, DirectX::XMVECTOR& curr, DirectX::XMVECTOR& next);
};