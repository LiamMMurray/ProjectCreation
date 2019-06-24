#pragma once
#include <DirectXMath.h>
#include <vector>
#include "..//..//ECS/ECSTypes.h"
struct SplineCluster
{
        std::vector<DirectX::XMVECTOR> spawnQueue;
        std::vector<ComponentHandle>   splineComponentList;
        int                            current;
        int                            color;
};