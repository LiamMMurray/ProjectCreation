#pragma once

#define IS_FLAT (1 << 0)
#define IS_STEEP (1 << 1)

struct FInstanceData
{
        matrix mtx;
        uint   flags;
        float  lifeTime;
        float2 padding;
};
