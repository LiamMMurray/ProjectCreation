#pragma once
#include <DirectXMath.h>
#include <vector>
struct SplineCluster
{
        std::vector<DirectX::XMVECTOR> spawnQueue;
        int                            current;
        int                            color;
};