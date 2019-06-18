#pragma once
#include "..//Transform.h"
#include "FGoodSpline.h"

namespace SplineUtility
{
        std::vector<DirectX::XMVECTOR> BakePointsFromSplineAndTransform(FGoodSpline&             spline,
                                                                        const DirectX::XMVECTOR& start,
                                                                        const DirectX::XMVECTOR& end,
                                                                        float                    width,
                                                                        unsigned int             subdivisions      = 10,
                                                                        unsigned int             heightWaveCount   = 1,
                                                                        float                    maxHeightVariance = 0.3f);
}